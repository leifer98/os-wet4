#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100000000 // Define a maximum size limit for allocation which is 10^8

typedef struct MallocMetadata
{
    size_t size;
    bool is_free;
    MallocMetadata *next;
    MallocMetadata *prev;
} metaData;

struct stats{
    size_t num_free_blocks;
    size_t num_free_bytes;
    size_t num_allocated_blocks;
    size_t num_allocated_bytes;
    size_t num_meta_data_bytes;
    //size_t size_meta_data;
};

metaData *head = nullptr;
metaData *tail = nullptr;
int check = 0;

void printList()
{                 // TODO: REMOVE BEFORE SUBMISSION
    int rank = 1; // Initialize rank number
    metaData *cur = head;
    while (cur != nullptr)
    {
        printf("%d: size, isFree: %ld, %d\n", rank, cur->size, cur->is_free);
        cur = cur->next;
        rank++; // Increment rank number
    }
    printf("finished\n");
}

void *smalloc(size_t size)
{
    //  Check if size is 0 or exceeds the maximum allowed size
    if (size <= 0 || size > MAX_SIZE)
    {
        return NULL; // Return NULL if size is invalid
    }


    if (head == nullptr) // first node in list
    {
        // Cast the allocated memory to metaData* and set its fields
        void *program_break = sbrk(size + sizeof(metaData));
        if (program_break == (void *)-1)
        {
            return NULL;
        }
        metaData *data = (metaData *)program_break;
        data->size = size;     // Set the size of the allocated block
        data->is_free = false; // Mark the block as allocated
        data->next = nullptr;
        data->prev = nullptr;
        head = data;
        tail = data;
        // printList();
        return (void *)((char *)program_break + sizeof(metaData));
    }

    metaData *ptr = head;

    printf("Starting iterating... head pointer: %p\n", (void *)head);
    while (ptr && (!ptr->is_free || ptr->size < size))
    {
        ptr = ptr->next;
    }
    printf("check");
    if (ptr == nullptr)
    {
        void *program_break = sbrk(size + sizeof(metaData));
        if (program_break == (void *)-1)
        {
            return NULL;
        }
        // Cast the allocated memory to metaData* and set its fields
        metaData *data = (metaData *)program_break;
        data->size = size;     // Set the size of the allocated block
        data->is_free = false; // Mark the block as allocated

        data->prev = tail;
        tail->next = data;
        data->next = nullptr;
        tail = data;

        // printList();
        return (void *)((char *)program_break + sizeof(metaData));
    }
    ptr->is_free = false;
    // printList();
    return (void *)((char *)ptr + sizeof(metaData));
}

void *scalloc(size_t num, size_t size)
{
    if (num == 0 || size == 0)
    {
        return NULL;
    }
    return smalloc(num * size);
}

void sfree(void *p)
{
    metaData* ptr = (metaData*)p;
    if (ptr == nullptr || ptr->is_free) return;
    ptr->is_free = true;
}
void *srealloc(void *oldp, size_t size)
{
    if (size == 0 || size > MAX_SIZE){
        return NULL;
    }


    metaData* oldData = (metaData*)oldp;
    if (oldData->size > size){
        return oldp;
    }
    oldData->is_free = true;

    metaData* cur = head;
    while(cur != nullptr && !(cur->size > size && cur->is_free)){
        cur = cur->next;
    }
    if (cur == nullptr){
        void *program_break = sbrk(size + sizeof(metaData));
        if (program_break == (void *)-1)
        {
            return NULL;
        }
        // Cast the allocated memory to metaData* and set its fields
        metaData *data = (metaData *)program_break;
        data->size = size;     // Set the size of the allocated block
        data->is_free = false; // Mark the block as allocated

        data->prev = tail;
        tail->next = data;
        data->next = nullptr;
        tail = data;

        //printList();
        return (void *)((char *)program_break + sizeof(metaData));
    }
    memmove((cur + sizeof(metaData)), (oldData + sizeof(metaData)), oldData->size);
    cur->is_free = false;
    return nullptr;
}
size_t _num_free_blocks()
{
    return 0;
}
// ● Returns the number of allocated blocks in the heap that are currently free.
size_t _num_free_bytes()
{
    return 0;
}
// ● Returns the number of bytes in all allocated blocks in the heap that are currently free,
// excluding the bytes used by the meta - data structs.size_t _num_allocated_blocks()
// ● Returns the overall (free and used) number of allocated blocks in the heap.
size_t _num_allocated_bytes()
{
    return 0;
}
// ● Returns the overall number (free and used) of allocated bytes in the heap, excluding
// the bytes used by the meta - data structs.size_t _num_meta_data_bytes()
// ● Returns the overall number of meta-data bytes currently in the heap.
size_t _size_meta_data()
{
    return 0;
}
// ● Returns the number of bytes of a single meta-data structure in your system.