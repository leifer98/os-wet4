#include <iostream>
#include <cstring>

// Assuming the metadata structure
struct metaData
{
    size_t size;
    bool is_free;
    metaData *next;
    metaData *prev;
};
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

// // Test 1: Allocate a block of 100000001 bytes (should fail)
// void test1()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();

//     void *ptr = smalloc(100000001);

//     if (ptr == nullptr && _num_allocated_blocks() == initial_allocated_blocks)
//     {
//         std::cout << "Test 1 passed: Could not allocate 100000001 bytes (as expected)." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 1 failed: Unexpectedly allocated 100000001 bytes." << std::endl;
//     }
// }

// // Test 2: Allocate a block for "Sample string" and verify metadata
// void test2()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();
//     size_t initial_allocated_bytes = _num_allocated_bytes();

//     const char *test_str = "Sample string";
//     void *ptr = smalloc(strlen(test_str) + 1);

//     if (ptr != nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks + 1 &&
//         _num_allocated_bytes() == initial_allocated_bytes + strlen(test_str) + 1)
//     {
//         strcpy((char *)ptr, test_str);
//         metaData *meta = getMetaData(ptr);
//         if (meta->size == strlen(test_str) + 1 && !meta->is_free)
//         {
//             std::cout << "Test 2 passed: 'Sample string' allocated and metadata verified." << std::endl;
//         }
//         else
//         {
//             std::cerr << "Test 2 failed: Metadata mismatch for 'Sample string'." << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Test 2 failed: Could not allocate memory for 'Sample string'." << std::endl;
//     }
// }

// // Test 3: Allocate 0 bytes (should fail)
// void test3()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();

//     void *ptr = smalloc(0);

//     if (ptr == nullptr && _num_allocated_blocks() == initial_allocated_blocks)
//     {
//         std::cout << "Test 3 passed: Allocation of 0 bytes correctly returned NULL." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 3 failed: Allocation of 0 bytes should return NULL." << std::endl;
//     }
// }

// // Test 4: Allocate exactly 1 byte (edge case)
// void test4()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();
//     size_t initial_allocated_bytes = _num_allocated_bytes();

//     void *ptr = smalloc(1);

//     if (ptr != nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks + 1 &&
//         _num_allocated_bytes() == initial_allocated_bytes + 1)
//     {
//         std::cout << "Test 4 passed: Allocated 1 byte successfully." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 4 failed: Could not allocate 1 byte." << std::endl;
//     }
// }

// // Test 5: Allocate "Sample string" twice and verify metadata
// void test5()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();
//     size_t initial_allocated_bytes = _num_allocated_bytes();

//     const char *test_str = "Sample string";
//     void *ptr1 = smalloc(strlen(test_str) + 1);
//     void *ptr2 = smalloc(strlen(test_str) + 1);

//     if (ptr1 != nullptr && ptr2 != nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks + 2 &&
//         _num_allocated_bytes() == initial_allocated_bytes + 2 * (strlen(test_str) + 1))
//     {
//         strcpy((char *)ptr1, test_str);
//         strcpy((char *)ptr2, test_str);
//         metaData *meta1 = getMetaData(ptr1);
//         metaData *meta2 = getMetaData(ptr2);
//         if (meta1->size == strlen(test_str) + 1 && !meta1->is_free &&
//             meta2->size == strlen(test_str) + 1 && !meta2->is_free &&
//             meta1->next == meta2 && meta2->prev == meta1)
//         {
//             std::cout << "Test 5 passed: Multiple allocations of 'Sample string' succeeded and metadata verified." << std::endl;
//         }
//         else
//         {
//             std::cerr << "Test 5 failed: Metadata mismatch or linkage error for multiple 'Sample string' allocations." << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Test 5 failed: Multiple allocations of 'Sample string' did not succeed." << std::endl;
//     }
// }

// // Test 6: Allocate "Sample string", free it using sfree, and check metadata
// void test6()
// {
//     markAllBlocksNotFree();
//     size_t initial_free_blocks = _num_free_blocks();
//     size_t initial_free_bytes = _num_free_bytes();

//     const char *test_str = "Sample string";
//     void *ptr = smalloc(strlen(test_str) + 1);

//     if (ptr != nullptr)
//     {
//         strcpy((char *)ptr, test_str);
//         sfree(ptr); // Use sfree to free the block
//         metaData *meta = getMetaData(ptr);
//         if (meta->is_free &&
//             _num_free_blocks() == initial_free_blocks + 1 &&
//             _num_free_bytes() == initial_free_bytes + strlen(test_str) + 1)
//         {
//             std::cout << "Test 6 passed: 'Sample string' allocated, freed using sfree, and metadata verified." << std::endl;
//         }
//         else
//         {
//             std::cerr << "Test 6 failed: Metadata not updated correctly on free." << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Test 6 failed: Could not allocate memory for 'Sample string'." << std::endl;
//     }
// }

// // Test 7: Allocate a block with scalloc and check zero-initialization
// void test7()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();
//     size_t initial_allocated_bytes = _num_allocated_bytes();

//     size_t num = 10;
//     size_t size = strlen("Sample string") + 1;
//     char *ptr = (char *)scalloc(num, size);

//     if (ptr != nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks + 1 &&
//         _num_allocated_bytes() == initial_allocated_bytes + num * size)
//     {
//         bool all_zero = true;
//         for (size_t i = 0; i < num * size; ++i)
//         {
//             if (ptr[i] != 0)
//             {
//                 all_zero = false;
//                 break;
//             }
//         }
//         if (all_zero)
//         {
//             std::cout << "Test 7 passed: scalloc allocated and zero-initialized memory." << std::endl;
//         }
//         else
//         {
//             std::cerr << "Test 7 failed: scalloc did not zero-initialize memory." << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Test 7 failed: Could not allocate memory with scalloc." << std::endl;
//     }
// }

// // Test 8: Allocate with scalloc with num or size as 0 (should return NULL)
// void test8()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();

//     void *ptr1 = scalloc(0, strlen("Sample string") + 1);
//     void *ptr2 = scalloc(10, 0);

//     if (ptr1 == nullptr && ptr2 == nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks)
//     {
//         std::cout << "Test 8 passed: scalloc returned NULL when num or size is 0." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 8 failed: scalloc should return NULL when num or size is 0." << std::endl;
//     }
// }

// // Test 9: Allocate with scalloc with size * num > 100000000 bytes (should return NULL)
// void test9()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();

//     size_t num = 100000001; // Choose values that exceed the 100000000-byte limit
//     size_t size = 1;
//     void *ptr = scalloc(num, size);

//     if (ptr == nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks)
//     {
//         std::cout << "Test 9 passed: scalloc returned NULL when size * num exceeds 100000000 bytes." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 9 failed: scalloc should return NULL when size * num exceeds 100000000 bytes." << std::endl;
//     }
// }

// // Test 10: Allocate a block with scalloc exactly equal to 100000000 bytes
// void test10()
// {
//     markAllBlocksNotFree();
//     size_t initial_allocated_blocks = _num_allocated_blocks();
//     size_t initial_allocated_bytes = _num_allocated_bytes();

//     size_t num = 100000000; // Allocate exactly 100000000 bytes
//     size_t size = 1;
//     void *ptr = scalloc(num, size);

//     if (ptr != nullptr &&
//         _num_allocated_blocks() == initial_allocated_blocks + 1 &&
//         _num_allocated_bytes() == initial_allocated_bytes + num * size)
//     {
//         std::cout << "Test 10 passed: scalloc successfully allocated 100000000 bytes." << std::endl;
//     }
//     else
//     {
//         std::cerr << "Test 10 failed: scalloc should have allocated 100000000 bytes." << std::endl;
//     }
// }

int main()
{
    /*test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();

    printList();*/
    //std::cout << "check1." << std::endl;
    std::cout << "Init" << std::endl;
    void *ptr1 = smalloc(2);
    std::cout << "ptr1: " << ptr1 << std::endl;
    std::cout << "After one smalloc:" << std::endl; 
    printArr();
    void *ptr2 = smalloc(2);
    std::cout << "ptr2: " << ptr2 << std::endl;
    std::cout << "After both smalloc:" << std::endl;
    printArr();
    sfree(ptr1);
    std::cout << "After first sfree" << std::endl;
    printArr();
    sfree(ptr2);
    std::cout << "After second sfree" << std::endl;
    printArr();

    std::cout << "All tests completed." << std::endl;
    return 0;
}
