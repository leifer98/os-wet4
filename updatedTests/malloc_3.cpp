#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <iostream>

using namespace std;

#define MAX_SIZE 100000000 // Define a maximum size limit for allocation which is 10^8

// Structure to hold metadata for each allocated block
typedef struct MallocMetadata
{
    size_t order;          // Order of the block size (used in buddy system)
    bool is_free;          // Indicates if the block is free or allocated
    MallocMetadata *next;  // Pointer to the next block in the list
    MallocMetadata *prev;  // Pointer to the previous block in the list
    MallocMetadata *buddy[9]; // Array of pointers to buddies in different levels
} metaData;

// Structure to hold statistics about the memory allocation
typedef struct stats
{
    size_t num_free_blocks;      // Number of free blocks
    size_t num_free_bytes;       // Number of free bytes
    size_t num_allocated_blocks; // Number of allocated blocks
    size_t num_allocated_bytes;  // Number of allocated bytes
    size_t num_meta_data_bytes;  // Number of metadata bytes
    size_t size_meta_data;       // Size of a single metadata structure
} metaStats;

metaData *arr[11] = {nullptr};                       // Array of lists for different block sizes
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
    {131072, 10}}; // Pairs of block sizes and their orders

bool created = false; // Indicates if the memory pool has been created

void printList(int i);

void printArr()
{
    std::cout << "=== Memory Blocks in arr ===" << std::endl;

    // Iterate over each order in arr
    for (int i = 0; i < 11; i++)
    {
        printList(i);
    }

    std::cout << "=== End of Memory Blocks ===" << std::endl;
}

/**
 * printList - Prints the list of all memory blocks.
 * This function is intended for debugging purposes to visualize the memory blocks and their metadata.
 */
void printList(int i)
{
        std::cout << "Order " << i << " (" << pairs[i][0] << " bytes):" << std::endl;

        metaData *current = arr[i];

        // If there are no blocks in the current order
        if (current == nullptr)
        {
            std::cout << "  No blocks in this order." << std::endl;
            return;
        }

        int block_num = 1;

        // Iterate through the linked list of blocks for the current order
        while (current != nullptr)
        {
            std::cout << "  Block " << block_num << ":" << std::endl;
            std::cout << "    Address: " << current << std::endl;
            // std::cout << "    Is Free: " << (current->is_free ? "Yes" : "No") << std::endl;
            // std::cout << "    Order: " << current->order << std::endl;
            // std::cout << "    Next: " << current->next << std::endl;
            // std::cout << "    Prev: " << current->prev << std::endl;
            // std::cout << "    Buddy: " << current->buddy << std::endl;

            // Move to the next block in the list
            current = current->next;
            block_num++;
        }
    }

/**
 * getOrder - Returns the order number for a given size.
 * @param size: The size for which the order is to be determined.
 * @return: The order number corresponding to the smallest block size that can fit the given size.
 */
int getOrder(size_t size)
{
    for (int i = 0; i < 11; i++)
    {
        if (size < pairs[i][0])
        {
            return i; // Return the order number
        }
    }
    // If the size is greater than or equal to the largest pair size, return the last order
    return 10;
}



/**
 * markAllBlocksNotFree - Marks all blocks in the list as not free.
 * This function iterates over all blocks and marks them as used, updating the statistics accordingly.
 */
void markAllBlocksNotFree()
{
    // TODO: Implement if needed
}

/**
 * create - Initializes the memory pool by allocating a large block of memory.
 * The block is split into smaller blocks, and the largest block is added to the array.
 */
void create()
{
    void *program_break = sbrk(131072 * 32); // Allocate 32 blocks of the largest size
    metaData *data = (metaData *)program_break;
    data->is_free = true;
    data->prev = nullptr;
    data->order = 10;
    for(int i = 0; i < 10; i++){
        arr[i] = nullptr;
    }
    arr[10] = data;
    metaData *temp = arr[10];
    for (size_t i = 0; i < 31; i++)
    {
        metaData *curr = (metaData *)((char *)temp + 131072);
        for (int i = 0; i < 10; i++){
            curr->buddy[i] = nullptr;
        }
        curr->is_free = true;
        curr->order = 10;
        temp->next = curr;
        curr->prev = temp;
        temp = temp->next;
    }
    printArr();
    created = true;
}

/**
 * addCellToArr - Adds a memory block to the appropriate order list.
 * @param cell: Pointer to the metadata of the memory block to be added.
 */
void addCellToArr(metaData *cell)
{
    //cout << "cell info: " << cell << ",prev: " << cell->prev << ", next: " << (cell->next == nullptr) << endl;
    int order = cell->order;
    //cout << "Adding block to order " << order << endl;
    metaData *cur = arr[order];
    if (cur == nullptr)
    {
        arr[order] = cell;
        return;
    }
    //cout << cur << endl;
    while (cur < cell)
    {
        if (cur->next == nullptr)
        {
            cur->next = cell;
            cell->prev = cur;
            cell->next = nullptr;
            return;
        }
        cur = cur->next;
    }
    if (cur->prev == nullptr)
    {
        arr[order] = cell;
        cur->prev = cell;
        cell->next = cur;
        return;
    }
    cell->prev = cur->prev;
    cell->prev->next = cell;
    cur->prev = cell;
    cell->next = cur;
}

/**
 * splitSingleCell - Splits a memory block until it matches the desired order.
 * @param order: The desired order of the block.
 * @param currentMeta: Pointer to the metadata of the block to be split.
 */
void splitSingleCell(size_t order, metaData *currentMeta)
{
    //cout << "check 123 : Splitting block with order " << currentMeta->order << " to match order " << order << endl;
    if (order >= currentMeta->order){
        //cout << "removing from arr. prev,next are: " << currentMeta->prev << "," << currentMeta->next << std::endl;
        metaData* bef = currentMeta->prev;
        metaData* aft = currentMeta->next;
        if (bef != nullptr){
            bef->next = aft;
        }
        else{
            arr[currentMeta->order] = aft;
        }
        if (aft != nullptr){
            aft->prev = bef;
        }
        //cout << "HERE4" << endl;
        //cout << "\nfinished\n" << endl;
        //printArr();
        currentMeta->prev = nullptr;
        currentMeta->next = nullptr;
        return;
    }
    currentMeta->order--;
    metaData *newCell = (metaData *)((char *)currentMeta + pairs[currentMeta->order][0]);
    newCell->order = currentMeta->order;
    for (int i = 0; i < 10; i++){
        if (i == newCell->order){
            newCell->buddy[i] = currentMeta;
            currentMeta->buddy[i] = newCell;
        }
        else{
            newCell->buddy[i] = nullptr;
        }
    }
    //cout << "new order is " << newCell->order << endl;
    newCell->is_free = true;
    metaData* prev = currentMeta->prev;
    metaData* next = currentMeta->next;
    if (prev == nullptr){
        arr[currentMeta->order + 1] = next;
    }
    else{
        prev->next = next;
    }
    if (next != nullptr){
        next->prev = prev;
    }
    //cout << "\n print list 10" << endl;
    //printList(10);
    //cout << endl;
    currentMeta->next = nullptr;
    currentMeta->prev = nullptr;
    newCell->next = nullptr;
    newCell->prev = nullptr;
    addCellToArr(currentMeta);
    //cout << "addCellToArr(currentMeta);" << endl;
    //printList(10);
    addCellToArr(newCell);
    //cout << "addCellToArr(newCell);" << endl;
    //printList(10);
    //cout << "check 123 : Splitting address " << currentMeta << "& order " << currentMeta->order << " to order " << order << endl;
    splitSingleCell(order, currentMeta);
}

/**
 * findandRemoveFreeBlock - Finds and removes a free block of the required order.
 * @param order: The order of the block to find.
 * @return: Pointer to the metadata of the found block.
 */
metaData *findandRemoveFreeBlock(int order)
{
    cout << "Looking for a free block with order " << order << endl;
    for (int i = order; i < 11; i++)
    {
        if (arr[i] != nullptr)
        {
            metaData *toReturn = arr[i];
            splitSingleCell(order, toReturn);
            return toReturn;
        }
    }
    return nullptr;
}

void combine(metaData* first){
    metaData* prev = first->prev;
    metaData* next = first->next->next;
    if (prev != nullptr){
        prev->next = next;
    }
    else{
        arr[first->order] = next;
    }
    if (next != nullptr){
        next->prev = prev;
    }
}
void freeHelper(metaData* meta){
    if (meta->order == 10) return;
    metaData* buddy = meta->buddy[meta->order];
    if (!buddy->is_free) return;
    if (meta < buddy){
        combine(meta);
    }
    else{
        combine(buddy);
        meta = buddy;
    }  
    //cout << "order is " << meta->order << endl;
    //printList(meta->order);
    meta->buddy[meta->order] = nullptr;
    meta->order++;
    addCellToArr(meta);
    freeHelper(meta);
}

/**
 * smalloc - Allocates a memory block of the given size.
 * @param size: The size of the memory block to allocate.
 * @return: Pointer to the allocated memory block, or NULL if allocation fails.
 */
void *smalloc(size_t size)
{
    if (!created)
        create();
    //printArr();
    int order = getOrder(size + sizeof(metaData));
    //cout << "Allocating block with order " << order << " for size " << size << endl;
    metaData *newData = findandRemoveFreeBlock(order);
    if (newData == nullptr)
        return nullptr;
    newData->is_free = false;
    //cout << "Allocation successful. Returning pointer to block." << endl;
    return (void *)((char *)newData + sizeof(metaData));
}

/**
 * scalloc - Allocates a memory block for an array of elements, each of a given size, and zeroes it out.
 * @param num: Number of elements.
 * @param size: Size of each element.
 * @return: Pointer to the allocated and zero-initialized memory block.
 */
void *scalloc(size_t num, size_t size)
{
    void *ptr = smalloc(num * size);
    if (ptr != nullptr)
    {
        memset(ptr, 0, num * size);
    }
    return ptr;
}

/**
 * sfree - Frees a memory block.
 * @param p: Pointer to the memory block to be freed.
 */
void sfree(void *p)
{
    if (p == nullptr)
        return;
    metaData *ptr = (metaData *)((char *)p - sizeof(metaData));
    if (ptr->is_free)
        return;
    ptr->is_free = true;
    addCellToArr(ptr);
    freeHelper(ptr);
    stats.num_free_blocks++;
    stats.num_free_bytes += pairs[ptr->order][0];
}

/**
 * srealloc - Resizes a memory block to a new size.
 * @param oldp: Pointer to the old memory block.
 * @param size: New size for the memory block.
 * @return: Pointer to the resized memory block, or NULL if resizing fails.
 */
void *srealloc(void *oldp, size_t size)
{
    if (size == 0 || size > MAX_SIZE)
        return NULL;
    if (oldp == nullptr)
        return smalloc(size);
    metaData *oldData = (metaData *)((char *)oldp - sizeof(metaData));
    if (oldData->order >= getOrder(size))
        return oldp;
    void *newp = smalloc(size);
    if (newp == nullptr)
        return NULL;
    memcpy(newp, oldp, pairs[oldData->order][0] - sizeof(metaData));
    sfree(oldp);
    return newp;
}

/**
 * _num_free_blocks - Returns the number of free blocks.
 * @return: The number of currently free blocks.
 */
size_t _num_free_blocks()
{
    return stats.num_free_blocks;
}

/**
 * _num_free_bytes - Returns the number of free bytes.
 * @return: The number of bytes in all free blocks.
 */
size_t _num_free_bytes()
{
    return stats.num_free_bytes;
}

/**
 * _num_allocated_blocks - Returns the number of allocated blocks.
 * @return: The total number of allocated blocks.
 */
size_t _num_allocated_blocks()
{
    return stats.num_allocated_blocks;
}

/**
 * _num_allocated_bytes - Returns the number of allocated bytes.
 * @return: The total number of allocated bytes, excluding metadata.
 */
size_t _num_allocated_bytes()
{
    return stats.num_allocated_bytes;
}

/**
 * _num_meta_data_bytes - Returns the number of metadata bytes.
 * @return: The total number of metadata bytes currently in use.
 */
size_t _num_meta_data_bytes()
{
    return stats.num_meta_data_bytes;
}

/**
 * _size_meta_data - Returns the size of a single metadata structure.
 * @return: The size of one metadata structure.
 */
size_t _size_meta_data()
{
    return stats.size_meta_data;
}
