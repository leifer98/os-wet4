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

void printList();

void markAllBlocksNotFree();

// Forward declaration of smalloc function
void *smalloc(size_t size);

// Forward declaration of scalloc function
void *scalloc(size_t num, size_t size);

void sfree(void *p);

// Function to read metadata from a memory block
metaData *getMetaData(void *ptr)
{
    return (metaData *)((char *)ptr - sizeof(metaData));
}

// Test 1: Allocate a block of 100000001 bytes (should fail)
void test1()
{
    markAllBlocksNotFree();
    void *ptr = smalloc(100000001);
    if (ptr == nullptr)
    {
        std::cout << "Test 1 passed: Could not allocate 100000001 bytes (as expected)." << std::endl;
    }
    else
    {
        std::cerr << "Test 1 failed: Unexpectedly allocated 100000001 bytes." << std::endl;
    }
}

// Test 2: Allocate a block for "Sample string"
void test2()
{
    markAllBlocksNotFree();
    const char *test_str = "Sample string";
    void *ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr)
    {
        strcpy((char *)ptr, test_str);
        std::cout << "Test 2 passed: Allocated and stored 'Sample string'." << std::endl;
    }
    else
    {
        std::cerr << "Test 2 failed: Could not allocate memory for 'Sample string'." << std::endl;
    }
}

// Test 3: Allocate 0 bytes (should fail)
void test3()
{
    markAllBlocksNotFree();
    void *ptr = smalloc(0);
    if (ptr == nullptr)
    {
        std::cout << "Test 3 passed: Allocation of 0 bytes correctly returned NULL." << std::endl;
    }
    else
    {
        std::cerr << "Test 3 failed: Allocation of 0 bytes should return NULL." << std::endl;
    }
}

// Test 4: Allocate a block of 100000001 bytes (should fail)
void test4()
{
    markAllBlocksNotFree();
    void *ptr = smalloc(100000001);
    if (ptr == nullptr)
    {
        std::cout << "Test 4 passed: Could not allocate 100000001 bytes (as expected)." << std::endl;
    }
    else
    {
        std::cerr << "Test 4 failed: Unexpectedly allocated 100000001 bytes." << std::endl;
    }
}

// Test 5: Allocate exactly 1 byte (edge case)
void test5()
{
    markAllBlocksNotFree();
    void *ptr = smalloc(1);
    if (ptr != nullptr)
    {
        std::cout << "Test 5 passed: Allocated 1 byte successfully." << std::endl;
    }
    else
    {
        std::cerr << "Test 5 failed: Could not allocate 1 byte." << std::endl;
    }
}

// Test 6: Allocate "Sample string" twice
void test6()
{
    markAllBlocksNotFree();
    const char *test_str = "Sample string";
    void *ptr1 = smalloc(strlen(test_str) + 1);
    void *ptr2 = smalloc(strlen(test_str) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr)
    {
        strcpy((char *)ptr1, test_str);
        strcpy((char *)ptr2, test_str);
        std::cout << "Test 6 passed: Multiple allocations of 'Sample string' succeeded." << std::endl;
    }
    else
    {
        std::cerr << "Test 6 failed: Multiple allocations of 'Sample string' did not succeed." << std::endl;
    }
}

// Test 7: Insert and verify "Sample string"
void test7()
{
    markAllBlocksNotFree();
    const char *test_str = "Sample string";
    void *ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr)
    {
        strcpy((char *)ptr, test_str);
        metaData *meta = getMetaData(ptr);
        if (meta->size == strlen(test_str) + 1 && !meta->is_free)
        {
            std::cout << "Test 7 passed: 'Sample string' allocated and metadata verified." << std::endl;
        }
        else
        {
            std::cerr << "Test 7 failed: Metadata mismatch for 'Sample string'." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 7 failed: Could not allocate memory for 'Sample string'." << std::endl;
    }
}

// Test 8: Allocate multiple "Sample string" and verify metadata in a sequence
void test8()
{
    markAllBlocksNotFree();
    const char *test_str = "Sample string";
    void *ptr1 = smalloc(strlen(test_str) + 1);
    void *ptr2 = smalloc(strlen(test_str) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr)
    {
        strcpy((char *)ptr1, test_str);
        strcpy((char *)ptr2, test_str);
        metaData *meta1 = getMetaData(ptr1);
        metaData *meta2 = getMetaData(ptr2);
        if (meta1->size == strlen(test_str) + 1 && !meta1->is_free &&
            meta2->size == strlen(test_str) + 1 && !meta2->is_free &&
            meta1->next == meta2 && meta2->prev == meta1)
        {
            std::cout << "Test 8 passed: Multiple 'Sample string' allocated and metadata verified." << std::endl;
        }
        else
        {
            std::cerr << "Test 8 failed: Metadata mismatch or list linkage error for multiple 'Sample string'." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 8 failed: Could not allocate memory for multiple 'Sample string'." << std::endl;
    }
}

// Test 9: Insert "Sample string", free it using sfree, and check metadata
void test9()
{
    markAllBlocksNotFree();
    const char *test_str = "Sample string";
    void *ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr)
    {
        strcpy((char *)ptr, test_str);
        sfree(ptr); // Use sfree to free the block
        metaData *meta = getMetaData(ptr);
        if (meta->is_free)
        {
            std::cout << "Test 9 passed: 'Sample string' allocated, freed using sfree, and metadata verified." << std::endl;
        }
        else
        {
            std::cerr << "Test 9 failed: Metadata not updated correctly on free." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 9 failed: Could not allocate memory for 'Sample string'." << std::endl;
    }
}

// Test 10: Allocate "Sample string", simulate fragmentation using sfree
void test10()
{
    markAllBlocksNotFree();
    const char *str1 = "Sample string";
    const char *str2 = "Sample string";
    void *ptr1 = smalloc(strlen(str1) + 1);
    void *ptr2 = smalloc(strlen(str2) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr)
    {
        strcpy((char *)ptr1, str1);
        strcpy((char *)ptr2, str2);
        sfree(ptr1); // Use sfree to free the first block
        metaData *meta1 = getMetaData(ptr1);
        metaData *meta2 = getMetaData(ptr2);
        if (meta1->is_free && !meta2->is_free)
        {
            std::cout << "Test 10 passed: Simulated fragmentation with sfree and verified metadata." << std::endl;
        }
        else
        {
            std::cerr << "Test 10 failed: Fragmentation simulation failed." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 10 failed: Could not allocate memory for 'Sample string'." << std::endl;
    }
}

// Test 11: Insert multiple "Sample string", free them using sfree, and check tail pointer consistency
void test11()
{
    markAllBlocksNotFree();
    const char *str1 = "Sample string";
    const char *str2 = "Sample string";
    const char *str3 = "Sample string";
    void *ptr1 = smalloc(strlen(str1) + 1);
    void *ptr2 = smalloc(strlen(str2) + 1);
    void *ptr3 = smalloc(strlen(str3) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr && ptr3 != nullptr)
    {
        strcpy((char *)ptr1, str1);
        strcpy((char *)ptr2, str2);
        strcpy((char *)ptr3, str3);
        sfree(ptr1); // Use sfree to free the first block
        sfree(ptr2); // Use sfree to free the second block
        metaData *meta1 = getMetaData(ptr1);
        metaData *meta2 = getMetaData(ptr2);
        metaData *meta3 = getMetaData(ptr3);
        if (meta3->prev == meta2)
        {
            std::cout << "Test 11 passed: Tail pointer consistency verified after using sfree." << std::endl;
        }
        else
        {
            std::cerr << "Test 11 failed: Tail pointer consistency failed." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 11 failed: Could not allocate memory for multiple 'Sample string'." << std::endl;
    }
}

// Test 12: Allocate a block with scalloc and check zero-initialization
void test12()
{
    markAllBlocksNotFree();
    size_t num = 10;
    size_t size = strlen("Sample string") + 1;
    char *ptr = (char *)scalloc(num, size);
    if (ptr != nullptr)
    {
        bool all_zero = true;
        for (size_t i = 0; i < num * size; ++i)
        {
            if (ptr[i] != 0)
            {
                all_zero = false;
                break;
            }
        }
        if (all_zero)
        {
            std::cout << "Test 12 passed: scalloc allocated and zero-initialized memory." << std::endl;
        }
        else
        {
            std::cerr << "Test 12 failed: scalloc did not zero-initialize memory." << std::endl;
        }
    }
    else
    {
        std::cerr << "Test 12 failed: Could not allocate memory with scalloc." << std::endl;
    }
}

// Test 13: Allocate with scalloc with num or size as 0 (should return NULL)
void test13()
{
    markAllBlocksNotFree();
    void *ptr1 = scalloc(0, strlen("Sample string") + 1);
    void *ptr2 = scalloc(10, 0);
    if (ptr1 == nullptr && ptr2 == nullptr)
    {
        std::cout << "Test 13 passed: scalloc returned NULL when num or size is 0." << std::endl;
    }
    else
    {
        std::cerr << "Test 13 failed: scalloc should return NULL when num or size is 0." << std::endl;
    }
}

// Test 14: Allocate with scalloc with size * num > 100000000 bytes (should return NULL)
void test14()
{
    markAllBlocksNotFree();
    size_t num = 100000001; // Choose values that exceed the 100000000-byte limit
    size_t size = 1;
    void *ptr = scalloc(num, size);
    if (ptr == nullptr)
    {
        std::cout << "Test 14 passed: scalloc returned NULL when size * num exceeds 100000000 bytes." << std::endl;
    }
    else
    {
        std::cerr << "Test 14 failed: scalloc should return NULL when size * num exceeds 100000000 bytes." << std::endl;
    }
}

// Test 15: Allocate a block with scalloc exactly equal to 100000000 bytes
void test15()
{
    markAllBlocksNotFree();
    size_t num = 100000000; // Allocate exactly 100000000 bytes
    size_t size = 1;
    void *ptr = scalloc(num, size);
    if (ptr != nullptr)
    {
        std::cout << "Test 15 passed: scalloc successfully allocated 100000000 bytes." << std::endl;
    }
    else
    {
        std::cerr << "Test 15 failed: scalloc should have allocated 100000000 bytes." << std::endl;
    }
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();

    printList();

    std::cout << "All tests completed." << std::endl;
    return 0;
}
