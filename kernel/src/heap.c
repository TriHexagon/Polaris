#include <heap.h>

typedef struct MemoryNode
{
	struct MemoryNode* next;
	size_t size;
} MemoryNode;

static void* heapAddress;	//heap start address
static size_t heapSize;		//heap size
static MemoryNode* head;	//head of the free block linked list

void heap_init(void)
{
	heapAddress = (void*)&_heapStart;
	heapSize = (size_t)(&_heapSize);
	head = (void*)(&_heapStart);

	//initialize free block linked list
	head->size = (size_t)(&_heapSize);
	head->next = NULL;
}

void* heap_alloc(size_t size)
{
	//add block size variable (size_t)
	size += sizeof(size_t);

	//smallest allocable memory is size of memory node
	if (size < sizeof(MemoryNode))
		size = sizeof(MemoryNode);

	//make size multiple of 4 (garanties that all addresses are aligned with 4)
	uint32_t mod = size % 4;
	if (mod != 0)
		size += 4 - mod;

	//first fit algorithm
	//search for free block >= size
	MemoryNode *current = head, *prev = NULL;
	while (current != NULL)
	{
		if (current->size >= size)
			break;

		prev = current;
		current = current->next;
	}

	//if no block found (current = NULL), return NULL
	if (current == NULL)
		return NULL;

	//if block size is equal or lesser than block size + node size (because min size of block is size of MemoryNode), use full block
	if ( (current->size - size) < sizeof(MemoryNode) )
	{
		if (prev != NULL) //if current is not head
		{
			prev->next = current->next; //remove node current from list

		}
		else //if current is head, complete first free block is allocated now
		{
			head = current->next;
		}

		*((size_t*)current) = current->size; //write block size
		return (size_t*)current + 1; //first bytes represents size of block
	}
	else //if block is greater than block size + node size, split block
	{
		MemoryNode* node = (MemoryNode*)((uint8_t*)current + size);
		node->next = current->next;
		node->size = current->size - size;

		if (prev != NULL) //if current is not head
			prev->next = node;
		else //if current is head
			head = node;

		*((size_t*)current) = size;
		return (size_t*)current + 1;
	}
}

void heap_free(void* mem)
{
	if (mem == NULL)
		return;

	//set to real block address (first bytes are block size)
	mem = ((size_t*)mem - 1);

	//get block size
	size_t size = *((size_t*)mem);

	//initialize new node
	MemoryNode* current = mem;
	//next pointer and size will be inititalized later

	//search gap for inserting new node
	//free block list is ordered by address
	MemoryNode* prev = NULL, *next = head;
	while (next != NULL)
	{
		if ((uint8_t*)next > (uint8_t*)mem)
			break;

		prev = next;
		next = next->next;
	}

	//check if neighbour nodes need to merge with current
	if (next != NULL)
	{
		if (prev != NULL)
		{
			//next and prev aren't NULL

			//prev with current
			if ( ((uint8_t*)prev + prev->size) == (uint8_t*)current )
			{
				//current with next
				if ( ((uint8_t*)next - size) == (uint8_t*)current )
				{
					//all three nodes need to merge
					prev->size += size + next->size;
					prev->next = next->next;
					return;
				}
				else
				{
					//prev and current need to merge
					prev->size += size;
					//prev->next = next; already done
					return;
				}
			}
			else
			{
				//current with next
				if ( ((uint8_t*)next - size) == (uint8_t*)current )
				{
					//current and next need to merge
					current->size = size + next->size;
					current->next = next->next;
					prev->next = current;
					return;
				}
				else
				{
					//no need to merge
					current->size = size;
					current->next = next;
					prev->next = current;
					return;
				}
			}
		}
		else
		{
			//prev is NULL, next is not, current block is at heap start address

			//current with next
			if ( ((uint8_t*)next - size) == (uint8_t*)current )
			{
				//current and next need to merge
				current->size = size + next->size;
				current->next = next->next;
				head = current;
				return;
			}
			else
			{
				//no need to merge
				current->size = size;
				current->next = next;
				head = current;
				return;
			}
		}
	}
	else
	{
		//next is NULL, prev could be
		if (prev != NULL)
		{
			//next is NULL, prev is not, current block is new tail

			//prev with current
			if ( ((uint8_t*)prev + prev->size) == (uint8_t*)current )
			{
				prev->size += size;
				return;
			}
			else
			{
				//no need to merge
				current->size = size;
				current->next = NULL;
				prev->next = current;
				return;
			}

		}
		else
		{
			//next and prev are NULL, no block is free, current is new head
			current->size = size;
			current->next = NULL;
			head = current;
			return;
		}
	}
}

void heap_getStats(size_t* outHeapSize, size_t* outAllocMem, size_t* outFreeMem)
{
	if (outHeapSize != NULL)
		*outHeapSize = heapSize;

	if (outAllocMem != NULL || outFreeMem != NULL)
	{
		//calculate free memory
		size_t freeMem = 0;
		MemoryNode* current = head;
		while (current != NULL)
		{
			freeMem += current->size;
			current = current->next;
		}

		if (outAllocMem != NULL)
			*outAllocMem = heapSize - freeMem;

		if (outFreeMem != NULL)
			*outFreeMem = freeMem;
	}
}
