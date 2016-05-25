/* The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Christopher Cichos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <dev.h>
#include <heap.h>
#include <util.h>

typedef struct Dev_DeviceEntry
{
	struct Dev_DeviceEntry* next;
	Dev_Device* device;
} Dev_DeviceEntry;

typedef struct Dev_EventHandlerEntry
{
	struct Dev_EventHandlerEntry* next;
	Dev_EventHandler handler;
} Dev_EventHandlerEntry;

const char* moduleName = "dev";
Dev_DeviceEntry* deviceEntries = NULL;
Dev_EventHandlerEntry* eventHandlers = NULL;

/* Subroutine to check if a given device name already exists in registered device list */
static bool deviceIdExists(const Dev_Device* device)
{
	for (const Dev_DeviceEntry* entry = deviceEntries; entry != NULL; entry = entry->next)
	{
		//first check device number (integer comparison is faster than string)
		if (entry->device->number == device->number)
		{
			//then check device name is equal, if they are equal -> device exists already
			if (util_strcmp(entry->device->name, device->name) == 0)
				return true;
		}
	}

	return false;
}

void dev_init(void)
{
	//null linked lists
	deviceEntries = NULL;
	eventHandlers = NULL;
}

void dev_deinit(void)
{
	//unregister left devices
	while (deviceEntries != NULL)
		dev_unregisterDevice(deviceEntries->device);

	//unregister left event handlers
	while (eventHandlers != NULL)
		dev_unregisterEventHandler(eventHandlers->handler);
}

error_t dev_registerDevice(Dev_Device* device)
{
	//check device parameter
	if (device->name == NULL || (device->type != DEV_DEVICE_TYPE_BLOCK && device->type != DEV_DEVICE_TYPE_CHAR))
		return ERROR_DEV_INVALID_DEVICE_STRUCTURE;

	//check if device name (name, number) exists already
	if (deviceIdExists(device))
		return ERROR_DEV_DEVICE_NAME_EXISTS_ALREADY;

	//create new device entry node for linked list
	Dev_DeviceEntry* newDeviceEntry = heap_alloc(sizeof(Dev_DeviceEntry));
	if (newDeviceEntry == NULL)
		return ERROR_DEV_MEMORY_ALLOCATION_FAILED;

	newDeviceEntry->device = device;
	newDeviceEntry->next = NULL;

	//append new node
	if (deviceEntries == NULL) //if no entry
	{
		deviceEntries = newDeviceEntry;
	}
	else //if one or more entries
	{
		Dev_DeviceEntry* deviceEntry;
		for (deviceEntry = deviceEntries; deviceEntry->next != NULL; deviceEntry = deviceEntry->next)
			;

		deviceEntry->next = newDeviceEntry;
	}

	//call eventhandlers because of registered device event
	for (Dev_EventHandlerEntry* eventHandler = eventHandlers; eventHandler != NULL; eventHandler = eventHandler->next)
		eventHandler->handler(newDeviceEntry->device, DEV_DEVICE_EVENT_REGISTERED);

	return ERROR_NONE;
}

error_t dev_unregisterDevice(Dev_Device* device)
{
	//check device parameter
	if (device->name == NULL || (device->type != DEV_DEVICE_TYPE_BLOCK && device->type != DEV_DEVICE_TYPE_CHAR))
		return ERROR_DEV_INVALID_DEVICE_STRUCTURE;

	//if no entry
	if (deviceEntries == NULL)
		return ERROR_DEV_DEVICE_NOT_REGISTERED;

	//find entry
	Dev_DeviceEntry* prevEntry = NULL;
	Dev_DeviceEntry* curEntry = deviceEntries;
	while (curEntry != NULL)
	{
		if (curEntry->device == device) //entry found
			break;

		if (curEntry->next == NULL) //end of list -> not found
			return ERROR_DEV_DEVICE_NOT_REGISTERED;

		//go to next node
		prevEntry = curEntry;
		curEntry = curEntry->next;
	}

	//call eventhandlers because of unregistered device event
	for (Dev_EventHandlerEntry* eventHandler = eventHandlers; eventHandler != NULL; eventHandler = eventHandler->next)
		eventHandler->handler(curEntry->device, DEV_DEVICE_EVENT_UNREGISTERED);

	//remove entry
	Dev_DeviceEntry* tmpEntry;
	if (prevEntry == NULL) //case: node is at the beginning of list
	{
		tmpEntry = curEntry;
		deviceEntries = curEntry->next;
		heap_free(tmpEntry);
	}
	else //case: node is at the middle or end of list
	{
		tmpEntry = curEntry;
		prevEntry->next = curEntry->next;
		heap_free(tmpEntry);
	}

	return ERROR_NONE;
}

error_t dev_registerEventHandler(Dev_EventHandler handler)
{
	//check parameter
	if (handler == NULL)
		return ERROR_INVALID_ADDRESS;

	//create eventhandler node
	Dev_EventHandlerEntry* newEventHandler = heap_alloc(sizeof(Dev_EventHandlerEntry));
	if (newEventHandler == NULL)
		return ERROR_DEV_MEMORY_ALLOCATION_FAILED;

	newEventHandler->handler = handler;
	newEventHandler->next = NULL;

	//append node
	if (eventHandlers == NULL) //if no entry
	{
		eventHandlers = newEventHandler;
	}
	else //if one or more entries
	{
		Dev_EventHandlerEntry* eventHandler;
		for (eventHandler = eventHandlers; eventHandler->next != NULL; eventHandler = eventHandler->next)
			;

		eventHandler->next = newEventHandler;
	}

	return ERROR_NONE;
}

error_t dev_unregisterEventHandler(Dev_EventHandler handler)
{
	//if no entry
	if (eventHandlers == NULL)
		return ERROR_DEV_HANDLER_NOT_REGISTERED;

	//find entry
	Dev_EventHandlerEntry* prevEntry = NULL;
	Dev_EventHandlerEntry* curEntry = eventHandlers;
	while (curEntry != NULL)
	{
		if (curEntry->handler == handler) //entry found
			break;

		if (curEntry->next == NULL) //end of list -> not found
			return ERROR_DEV_HANDLER_NOT_REGISTERED;

		//go to next node
		prevEntry = curEntry;
		curEntry = curEntry->next;
	}

	//remove entry
	Dev_EventHandlerEntry* tmpEntry;
	if (prevEntry == NULL) //case: node is at the beginning of list
	{
		tmpEntry = curEntry;
		eventHandlers = curEntry->next;
		heap_free(tmpEntry);
	}
	else //case: node is at the middle or end of list
	{
		tmpEntry = curEntry;
		prevEntry->next = curEntry->next;
		heap_free(tmpEntry);
	}

	return ERROR_NONE;
}
