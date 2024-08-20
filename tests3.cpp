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
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
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
    std::cout << "starting, _size_meta_data():" << _size_meta_data() << std::endl;
    std::cout << "starting, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;
    //  4325376 (0x420000) == 4192624 (0x3ff970)
    void *ptr1 = smalloc(40);
    // REQUIRE(ptr1 != nullptr);
    //    verify_size(base);
    std::cout << "After one smalloc, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;

    // Allocate large block (order 10)
    printArr();
    void *ptr2 = smalloc((128 * 1024) + 100);
    // REQUIRE(ptr2 != nullptr);
    //    verify_size_with_large_blocks(base, (128 * 1024+100 +_size_meta_data()));
    // verify_block_by_order(1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 31, 0, 1, MAX_ELEMENT_SIZE + 100);
    std::cout << "ptr2: " << ptr2 << std::endl;
    std::cout << "After both smalloc, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;
    printArr();
    // Allocate another small block
    void *ptr3 = smalloc(50);
    // REQUIRE(ptr3 != nullptr);
    // verify_block_by_order(0, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 31, 0, 1, MAX_ELEMENT_SIZE + 100);
    std::cout << "ptr3: " << ptr3 << std::endl;
    std::cout << "After third smalloc, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;
    printArr();
    // Free the first small block
    sfree(ptr1);
    // verify_block_by_order(1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 31, 0, 1, MAX_ELEMENT_SIZE + 100);
    std::cout << "ptr1: " << ptr1 << std::endl;
    std::cout << "After one free, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;
    printArr();
    // Allocate another small block
    void *ptr4 = smalloc(40);
    // REQUIRE(ptr4 != nullptr);
    // verify_block_by_order(0, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 31, 0, 1, MAX_ELEMENT_SIZE + 100);
    std::cout << "ptr4: " << ptr4 << std::endl;
    std::cout << "After forth malloc, _num_allocated_bytes():" << _num_allocated_bytes() << std::endl;
    printArr();
    // Free all blocks
    sfree(ptr3);
    sfree(ptr4);
    // verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 1, MAX_ELEMENT_SIZE + 100);
    sfree(ptr1); // free again
    sfree(ptr2);
    // verify_block_by_order(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0);

    // void *ptr1 = smalloc(2);
    // std::cout << "ptr1: " << ptr1 << std::endl;
    // std::cout << "After one smalloc, _num_allocated_blocks():" << _num_allocated_blocks() << std::endl;
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
