#include <iostream>
#include <cstring>

// Structure to hold metadata for each allocated block
typedef struct MallocMetadata
{
    size_t order;          // Order of the block size or size itself
    bool is_free;          // Indicates if the block is free or allocated
    MallocMetadata *next;  // Pointer to the next block in the list
    MallocMetadata *prev;  // Pointer to the previous block in the list
    MallocMetadata *buddy; // Array of pointers to buddies in different levels
} metaData;

void printArr();
void printList();
void markAllBlocksNotFree();

// Forward declaration of smalloc function
void *smalloc(size_t size);

// Forward declaration of scalloc function
void *scalloc(size_t num, size_t size);

void sfree(void *p);

// Stats functions
size_t _num_free_blocks();
size_t _num_free_blocks();
size_t _num_free_blocks();
size_t _num_free_blocks();
size_t _num_meta_data_bytes();
size_t _size_meta_data();

// Function to read metadata from a memory block
metaData *getMetaData(void *ptr)
{
    return (metaData *)((char *)ptr - sizeof(metaData));
}

int main()
{

    std::cout << "Init" << std::endl;
    
    std::vector<void *> allocations;

    // Allocate 64 blocks of size 128 * 2^9 - 64
    for (int i = 0; i < 64; i++)
    {
        // printf("%d\n", i);
        fflush(stdout);
        void *ptr = smalloc(128 * std::pow(2, 9) - 64);
        REQUIRE(ptr != nullptr);
        allocations.push_back(ptr);
        verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, allocations.size() % 2, allocations.size(), 32 - (int)(i / 2) - 1, 0, 0, 0);
        // printf("OK\n");
        fflush(stdout);
    }

    REQUIRE(smalloc(40) == NULL);
    // Free the allocated blocks
    while (!allocations.empty())
    {
        void *ptr = allocations.back();
        allocations.pop_back();
        sfree(ptr);
        verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, allocations.size() % 2, allocations.size(), 32 - (int)(allocations.size() / 2) - (allocations.size() % 2), 0, 0, 0);
    }

    // Verify that all blocks are merged into a single large block
    verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0);

    for (int i = 0; i < 64; i++)
    {
        void *ptr = smalloc(128 * std::pow(2, 9) - 64);
        REQUIRE(ptr != nullptr);
        allocations.push_back(ptr);
        verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, allocations.size() % 2, allocations.size(), 32 - (int)(i / 2) - 1, 0, 0, 0);
    }
    REQUIRE(smalloc(40) == NULL);
    // Free the allocated blocks
    while (!allocations.empty())
    {
        void *ptr = allocations.front();
        allocations.erase(allocations.begin());
        sfree(ptr);
        verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, allocations.size() % 2, allocations.size(), 32 - (int)(allocations.size() / 2) - (allocations.size() % 2), 0, 0, 0);
    }
    verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0);

    // void *ptr1 = smalloc(2);
    // std::cout << "ptr1: " << ptr1 << std::endl;
    // std::cout << "After one smalloc, _num_free_blocks():" << _num_free_blocks() << std::endl;
    // printArr();
    // void *ptr2 = smalloc(2);
    // std::cout << "ptr2: " << ptr2 << std::endl;
    // std::cout << "After both smalloc:" << std::endl;
    // printArr();
    // sfree(ptr2);
    // std::cout << "After first sfree" << std::endl;
    // printArr();
    // sfree(ptr1);
    // std::cout << "After second sfree" << std::endl;
    // printArr();

    std::cout << "All tests completed." << std::endl;
    return 0;
}
