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

/**
 * @file dev.h
 *
 * @brief Device module. The place where device drivers register itself.
 */

#ifndef DEV_H
#define DEV_H

#include <kernel.h>

#define ERROR_DEV_INVALID_DEVICE_STRUCTURE		(ERROR_MODULE_DEFINED)
#define ERROR_DEV_MEMORY_ALLOCATION_FAILED		(ERROR_MODULE_DEFINED+1)
#define ERROR_DEV_DEVICE_NOT_REGISTERED			(ERROR_MODULE_DEFINED+2)
#define ERROR_DEV_HANDLER_NOT_REGISTERED		(ERROR_MODULE_DEFINED+3)
#define ERROR_DEV_DEVICE_NAME_EXISTS_ALREADY	(ERROR_MODULE_DEFINED+4)

typedef enum DEV_DEVICE_EVENT
{
	DEV_DEVICE_EVENT_REGISTERED,	/**< A device has been registered**/
	DEV_DEVICE_EVENT_UNREGISTERED	/**< A device has been unregistered**/
} DEV_DEVICE_EVENT;

typedef enum DEV_DEVICE_TYPE
{
	DEV_DEVICE_TYPE_BLOCK,	/**< Read/Write operate on a fixed number of bytes**/
	DEV_DEVICE_TYPE_CHAR	/**< Read/Write operate on variable number of bytes**/
} DEV_DEVICE_TYPE;

typedef struct Dev_Device
{
	const char* name;		/**< Name of the device (for example 'dma') **/
	uint32_t number;		/**< Number of the device (name and number make the device name for example 'dma0')**/
	DEV_DEVICE_TYPE type;	/**< Device type, either block or char**/

	//operations
	size_t (*read)(struct Dev_Device*, void* buf, size_t size, size_t offset);			/**< Function pointer to read operation handler (if device doesn't support this operation, it must be NULL)**/
	size_t (*write)(struct Dev_Device*, const void* buf, size_t size, size_t offset);	/**< Function pointer to write operation handler (if device doesn't support this operation, it must be NULL)**/
	error_t (*ioctl)(struct Dev_Device*, uint32_t num, void* args);						/**< Function pointer to io-control operation handler (if device doesn't support this operation, it must be NULL)**/
} Dev_Device;

/**
 * @brief Device event handler.
 */
typedef void (*Dev_EventHandler)(Dev_Device* device, DEV_DEVICE_EVENT event);

/**
 * @brief Initialize device module.
 * @return
 */
void dev_init(void);

/**
 * @brief Deinitialize device module.
 */
void dev_deinit(void);

/**
 * @brief Drivers register their devices here.
 * @param device Pointer to a initialzed and valid Dev_Device structure. Must be not NULL.
 * @return ERROR_DEV_INVALID_DEVICE_STRUCTURE if Dev_Device structure contains invalid values.
 * ERROR_DEV_DEVICE_NAME_EXISTS_ALREADY if a device with the same device name (name + number) is already registered.
 * ERROR_DEV_MEMORY_ALLOCATION_FAILED if memory allocation failed.
 * Otherwise ERROR_NONE.
 */
error_t dev_registerDevice(Dev_Device* device);

/**
 * @brief Drivers unregister their devices here.
 * @param device Pointer to a Dev_Device structure which has been registered.
 * @return ERROR_DEV_INVALID_DEVICE_STRUCTURE if Dev_Device structure contains invalid values.
 * ERROR_DEV_DEVICE_NOT_REGISTERED if there is no device registered with the same device name.
 * Otherwise ERROR_NONE.
 */
error_t dev_unregisterDevice(Dev_Device* device);

/**
 * @brief Register an event handler. Registered event handlers will be called when a new device is registered or a device is unregistered (in this case handler will be called before device is removed from list).
 * It is allowed to register the same event handler more than once.
 * @param handler Event handler.
 * @return ERROR_INVALID_ADDRESS if handler is NULL.
 * ERROR_DEV_MEMORY_ALLOCATION_FAILED if memory allocation failed.
 * Otherwise ERROR_NONE.
 */
error_t dev_registerEventHandler(Dev_EventHandler handler);

/**
 * @brief Unregister an event handler.
 * @param handler Event handler.
 * @return ERROR_DEV_HANDLER_NOT_REGISTERED if the given handler is not registered.
 * Otherwise ERROR_NONE.
 */
error_t dev_unregisterEventHandler(Dev_EventHandler handler);

#endif // DEV_H

