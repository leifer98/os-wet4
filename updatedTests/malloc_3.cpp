#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>

#define MAX_SIZE 100000000 // Define a maximum size limit for allocation which is 10^8

typedef struct MallocMetadata
{
    // size_t size;
    bool is_free;
    MallocMetadata *next;
    MallocMetadata *prev;
} metaData;

typedef struct stats
{
    size_t num_free_blocks;
    size_t num_free_bytes;
    size_t num_allocated_blocks;
    size_t num_allocated_bytes;
    size_t num_meta_data_bytes;
    size_t size_meta_data;
} metaStats;

/*metaData *head = nullptr;
metaData *tail = nullptr;*/
metaData *arr[11] = {nullptr};                       // Initialize the first element to nullptr, others will be initialized to 0 (nullptr in this case)
metaStats stats = {0, 0, 0, 0, 0, sizeof(metaData)}; // Initialize stats
int pairs[][2] = {
    {128, 0},
    {256, 1},
    {512, 2},
    {1024, 3},
    {2048, 4},
    {4096, 5},
    {8192, 6},
    {16384, 7},
    {32768, 8},
    {65536, 9},
    {131072, 10}};
bool created = false;

void printList()
{ // TODO: REMOVE BEFORE SUBMISSION
  //     int rank = 1; // Initialize rank number
  //     metaData *cur = head;

    //     printf("=== Memory Block List ===\n");

    //     while (cur != nullptr)
    //     {
    //         // Print all information on a single line
    //         printf("Block %d: Size: %ld bytes, Is Free: %s, Content (hex): 0x%lx\n",
    //                rank,
    //                cur->size,
    //                cur->is_free ? "Yes" : "No",
    //                *((unsigned long *)(cur + 1))); // Cast the content to an unsigned long and print as hex

    //         // Move to the next block
    //         cur = cur->next;
    //         rank++; // Increment rank number
    //     }

    //     printf("=========================\n");
    //     printf("Finished printing list.\n");
}
// Helper function to mark all blocks as not free
void markAllBlocksNotFree()
{
    //     metaData *current = head;

    //     while (current != nullptr)
    //     {
    //         if (current->is_free)
    //         {
    //             current->is_free = false;              // Mark the block as not free
    //             stats.num_free_blocks--;               // Decrease the count of free blocks
    //             stats.num_free_bytes -= current->size; // Decrease the free bytes count
    //         }
    //         current = current->next; // Move to the next block
    //     }
}

void create()
{
    void *program_break = sbrk((131072 * 32));
    metaData *data = (metaData *)program_break;
    data->is_free = true;
    arr[10] = data;
    metaData *temp = arr[10];
    for (size_t i = 0; i < 31; i++)
    {
        metaData *curr = (metaData *)(temp + 131072);
        curr->is_free = true;
        temp->next = curr;
        curr->prev = temp;
        temp = temp->next;
    }
    created = true;
}

// void

int getOrder(size_t size){
    for (int i = 0; i < length; i++) {
        if (size < pairs[i][0]) {
            return i; // Return the order number
        }
    }
    // If the size is greater than or equal to the largest pair size, return the last order
    int order = length - 1;
}

void* findFreeBlock(int order){ //finds the location for the data that will be inserted
    for (int i = order; i < 11; i++){
        if (arr[i] != nullptr){
            return arr[i];
        }
    }
    return nullptr;
}

void *smalloc(size_t size)
{
    if (!created)
        create();

    
    
    void * head = arr[order];

    //  Check if size is 0 or exceeds the maximum allowed size
    // if (size <= 0 || size > MAX_SIZE)
    // {
    //     return NULL; // Return NULL if size is invalid
    // }

    // if (head == nullptr) // First node in the list
    // {
    //     void *program_break = sbrk(size + sizeof(metaData));
    //     if (program_break == (void *)-1)
    //     {
    //         return NULL;
    //     }
    //     metaData *data = (metaData *)program_break;
    //     data->size = size;     // Set the size of the allocated block
    //     data->is_free = false; // Mark the block as allocated
    //     data->next = nullptr;
    //     data->prev = nullptr;
    //     head = data;
    //     tail = data;

    //     // Update stats for the first allocation
    //     stats.num_allocated_blocks++;
    //     stats.num_allocated_bytes += size;
    //     stats.num_meta_data_bytes += sizeof(metaData);

    //     return (void *)((char *)program_break + sizeof(metaData));
    // }

    // metaData *ptr = head;

    // // Iterate through the list to find a suitable free block
    // while (ptr && (!ptr->is_free || ptr->size < size))
    // {
    //     ptr = ptr->next;
    // }

    // if (ptr == nullptr) // No suitable free block found
    // {
    //     void *program_break = sbrk(size + sizeof(metaData));
    //     if (program_break == (void *)-1)
    //     {
    //         return NULL;
    //     }
    //     // Cast the allocated memory to metaData* and set its fields
    //     metaData *data = (metaData *)program_break;
    //     data->size = size;     // Set the size of the allocated block
    //     data->is_free = false; // Mark the block as allocated

    //     data->prev = tail;
    //     tail->next = data;
    //     data->next = nullptr;
    //     tail = data;

    //     // Update stats for the new allocation
    //     stats.num_allocated_blocks++;
    //     stats.num_allocated_bytes += size;
    //     stats.num_meta_data_bytes += sizeof(metaData);

    //     return (void *)((char *)program_break + sizeof(metaData));
    // }

    // // If a suitable free block is found
    // ptr->is_free = false;
    // stats.num_free_blocks--;
    // stats.num_free_bytes -= ptr->size;

    // return (void *)((char *)ptr + sizeof(metaData));
    return nullptr;
}

void *scalloc(size_t num, size_t size)
{
    // if (num == 0 || size == 0)
    // {
    //     return NULL;
    // }

    // void *ptr = smalloc(num * size);

    // if (ptr != nullptr)
    // {
    //     memset(ptr, 0, num * size); // Zero out the allocated memory
    // }

    // return ptr;
    return nullptr;
}

void sfree(void *p)
{
    // if (p == nullptr)
    //     return;

    // metaData *ptr = (metaData *)((char *)p - sizeof(metaData));

    // if (ptr->is_free)
    //     return;

    // ptr->is_free = true;

    // // Update stats for freeing the block
    // stats.num_free_blocks++;
    // stats.num_free_bytes += ptr->size;
}

void *srealloc(void *oldp, size_t size)
{
    // if (size == 0 || size > MAX_SIZE)
    // {
    //     return NULL;
    // }

    // if (oldp == nullptr)
    // {
    //     return smalloc(size); // If oldp is NULL, behave like smalloc
    // }

    // metaData *oldData = (metaData *)((char *)oldp - sizeof(metaData));

    // if (oldData->size >= size)
    // {
    //     return oldp; // If the existing block is big enough, return the old pointer
    // }

    // void *newp = smalloc(size);

    // if (newp == nullptr)
    // {
    //     return NULL;
    // }

    // memcpy(newp, oldp, oldData->size); // Copy old data to the new block
    // sfree(oldp);                       // Free the old block

    // return newp;
    return nullptr;
}

// Returns the number of allocated blocks in the heap that are currently free.
size_t _num_free_blocks()
{
    return stats.num_free_blocks;
}

// Returns the number of bytes in all allocated blocks in the heap that are currently free,
// excluding the bytes used by the meta-data structs.
size_t _num_free_bytes()
{
    return stats.num_free_bytes;
}

// Returns the overall (free and used) number of allocated blocks in the heap.
size_t _num_allocated_blocks()
{
    return stats.num_allocated_blocks;
}

// Returns the overall number (free and used) of allocated bytes in the heap, excluding
// the bytes used by the meta-data structs.
size_t _num_allocated_bytes()
{
    return stats.num_allocated_bytes;
}

// Returns the overall number of meta-data bytes currently in the heap.
size_t _num_meta_data_bytes()
{
    return stats.num_meta_data_bytes;
}

// Returns the number of bytes of a single meta-data structure in your system.
size_t _size_meta_data()
{
    return stats.size_meta_data;
}