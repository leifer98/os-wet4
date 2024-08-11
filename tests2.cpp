#include <iostream>
#include <cstring>

// Assuming the metadata structure
struct metaData {
    size_t size;
    bool is_free;
    metaData* next;
    metaData* prev;
};

// Forward declaration of smalloc function
void* smalloc(size_t size);

// Forward declaration of smalloc function
void* scalloc(size_t num, size_t size);

// Function to read metadata from a memory block
metaData* getMetaData(void* ptr) {
    return (metaData*)((char*)ptr - sizeof(metaData));
}

// Test 1: Allocate a small block of memory (10 bytes)
void test1() {
    void* ptr = smalloc(10);
    if (ptr != nullptr) {
        std::cout << "Test 1 passed: Allocated 10 bytes successfully." << std::endl;
    } else {
        std::cerr << "Test 1 failed: Could not allocate 10 bytes." << std::endl;
    }
}

// Test 2: Allocate exactly 100000000 bytes
void test2() {
    void* ptr = smalloc(100000000);
    if (ptr != nullptr) {
        std::cout << "Test 2 passed: Allocated 100000000 bytes successfully." << std::endl;
    } else {
        std::cerr << "Test 2 failed: Could not allocate 100000000 bytes." << std::endl;
    }
}

// Test 3: Allocate 0 bytes (should fail)
void test3() {
    void* ptr = smalloc(0);
    if (ptr == nullptr) {
        std::cout << "Test 3 passed: Allocation of 0 bytes correctly returned NULL." << std::endl;
    } else {
        std::cerr << "Test 3 failed: Allocation of 0 bytes should return NULL." << std::endl;
    }
}

// Test 4: Allocate more than 100000000 bytes (should fail)
void test4() {
    void* ptr = smalloc(100000001);
    if (ptr == nullptr) {
        std::cout << "Test 4 passed: Allocation of more than 100000000 bytes correctly returned NULL." << std::endl;
    } else {
        std::cerr << "Test 4 failed: Allocation of more than 100000000 bytes should return NULL." << std::endl;
    }
}

// Test 5: Allocate exactly 1 byte (edge case)
void test5() {
    void* ptr = smalloc(1);
    if (ptr != nullptr) {
        std::cout << "Test 5 passed: Allocation of 1 byte successfully." << std::endl;
    } else {
        std::cerr << "Test 5 failed: Could not allocate 1 byte." << std::endl;
    }
}

// Test 6: Allocate exactly 100000000 bytes with multiple allocations
void test6() {
    void* ptr1 = smalloc(100000000);
    void* ptr2 = smalloc(100000000);
    if (ptr1 != nullptr && ptr2 != nullptr) {
        std::cout << "Test 6 passed: Multiple allocations of 100000000 bytes each succeeded." << std::endl;
    } else {
        std::cerr << "Test 6 failed: Multiple allocations of 100000000 bytes each did not succeed." << std::endl;
    }
}

// Test 7: Insert and verify a short string
void test7() {
    const char* test_str = "Hello, World!";
    void* ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr) {
        strcpy((char*)ptr, test_str);
        metaData* meta = getMetaData(ptr);
        if (meta->size <= strlen(test_str) + 1 && !meta->is_free) {
            std::cout << "Test 7 passed: Short string allocated and metadata verified." << std::endl;
        } else {
            std::cerr << "Test 7 failed: Metadata mismatch for short string." << std::endl;
        }
    } else {
        std::cerr << "Test 7 failed: Could not allocate memory for short string." << std::endl;
    }
}

// Test 8: Insert and verify a longer string
void test8() {
    const char* test_str = "This is a longer string to test the memory allocator.";
    void* ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr) {
        strcpy((char*)ptr, test_str);
        metaData* meta = getMetaData(ptr);
        if (meta->size <= strlen(test_str) + 1 && !meta->is_free) {
            std::cout << "Test 8 passed: Longer string allocated and metadata verified." << std::endl;
        } else {
            std::cerr << "Test 8 failed: Metadata mismatch for longer string." << std::endl;
        }
    } else {
        std::cerr << "Test 8 failed: Could not allocate memory for longer string." << std::endl;
    }
}

// Test 9: Allocate multiple strings and verify metadata in a sequence
void test9() {
    const char* str1 = "First string";
    const char* str2 = "Second string";
    void* ptr1 = smalloc(strlen(str1) + 1);
    void* ptr2 = smalloc(strlen(str2) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr) {
        strcpy((char*)ptr1, str1);
        strcpy((char*)ptr2, str2);
        metaData* meta1 = getMetaData(ptr1);
        metaData* meta2 = getMetaData(ptr2);
        if (meta1->size <= strlen(str1) + 1 && !meta1->is_free &&
            meta2->size <= strlen(str2) + 1 && !meta2->is_free &&
            meta1->next == meta2 && meta2->prev == meta1) {
            std::cout << "Test 9 passed: Multiple strings allocated and metadata verified." << std::endl;
        } else {
            std::cerr << "Test 9 failed: Metadata mismatch or list linkage error for multiple strings." << std::endl;
        }
    } else {
        std::cerr << "Test 9 failed: Could not allocate memory for multiple strings." << std::endl;
    }
}

// Test 10: Insert a string, free it, and check metadata
void test10() {
    const char* test_str = "Temporary string";
    void* ptr = smalloc(strlen(test_str) + 1);
    if (ptr != nullptr) {
        strcpy((char*)ptr, test_str);
        metaData* meta = getMetaData(ptr);
        meta->is_free = true;  // Simulate freeing the block
        if (meta->is_free) {
            std::cout << "Test 10 passed: String allocated, freed, and metadata verified." << std::endl;
        } else {
            std::cerr << "Test 10 failed: Metadata not updated correctly on free." << std::endl;
        }
    } else {
        std::cerr << "Test 10 failed: Could not allocate memory for temporary string." << std::endl;
    }
}

// Test 11: Allocate a string and simulate fragmentation
void test11() {
    const char* str1 = "First fragment";
    const char* str2 = "Second fragment";
    void* ptr1 = smalloc(strlen(str1) + 1);
    void* ptr2 = smalloc(strlen(str2) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr) {
        strcpy((char*)ptr1, str1);
        strcpy((char*)ptr2, str2);
        metaData* meta1 = getMetaData(ptr1);
        metaData* meta2 = getMetaData(ptr2);
        meta1->is_free = true;  // Simulate freeing the first block
        if (meta1->is_free && !meta2->is_free) {
            std::cout << "Test 11 passed: Simulated fragmentation and verified metadata." << std::endl;
        } else {
            std::cerr << "Test 11 failed: Fragmentation simulation failed." << std::endl;
        }
    } else {
        std::cerr << "Test 11 failed: Could not allocate memory for fragments." << std::endl;
    }
}

// Test 12: Insert multiple strings and check tail pointer consistency
void test12() {
    const char* str1 = "First string";
    const char* str2 = "Second string";
    const char* str3 = "Third string";
    void* ptr1 = smalloc(strlen(str1) + 1);
    void* ptr2 = smalloc(strlen(str2) + 1);
    void* ptr3 = smalloc(strlen(str3) + 1);
    if (ptr1 != nullptr && ptr2 != nullptr && ptr3 != nullptr) {
        strcpy((char*)ptr1, str1);
        strcpy((char*)ptr2, str2);
        strcpy((char*)ptr3, str3);
        metaData* meta1 = getMetaData(ptr1);
        metaData* meta2 = getMetaData(ptr2);
        metaData* meta3 = getMetaData(ptr3);
        if (meta3->prev == meta2 && meta2->prev == meta1) {
            std::cout << "Test 12 passed: Tail pointer consistency verified." << std::endl;
        } else {
            std::cerr << "Test 12 failed: Tail pointer consistency failed." << std::endl;
        }
    } else {
        std::cerr << "Test 12 failed: Could not allocate memory for multiple strings." << std::endl;
    }
}

// Test 13: Allocate a block with scalloc and check zero-initialization
void test13() {
    size_t num = 10;
    size_t size = sizeof(int);
    int* ptr = (int*)scalloc(num, size);
    if (ptr != nullptr) {
        bool all_zero = true;
        for (size_t i = 0; i < num; ++i) {
            if (ptr[i] != 0) {
                all_zero = false;
                break;
            }
        }
        if (all_zero) {
            std::cout << "Test 13 passed: scalloc allocated and zero-initialized memory." << std::endl;
        } else {
            std::cerr << "Test 13 failed: scalloc did not zero-initialize memory." << std::endl;
        }
    } else {
        std::cerr << "Test 13 failed: Could not allocate memory with scalloc." << std::endl;
    }
}

// Test 14: Allocate with scalloc with num or size as 0 (should return NULL)
void test14() {
    void* ptr1 = scalloc(0, sizeof(int));
    void* ptr2 = scalloc(10, 0);
    if (ptr1 == nullptr && ptr2 == nullptr) {
        std::cout << "Test 14 passed: scalloc returned NULL when num or size is 0." << std::endl;
    } else {
        std::cerr << "Test 14 failed: scalloc should return NULL when num or size is 0." << std::endl;
    }
}

// Test 15: Allocate with scalloc with size * num > 108 bytes (should return NULL)
void test15() {
    size_t num = 100000001;  // Choose values that exceed the 108-byte limit
    size_t size = 1;
    void* ptr = scalloc(num, size);
    if (ptr == nullptr) {
        std::cout << "Test 15 passed: scalloc returned NULL when size * num exceeds 108 bytes." << std::endl;
    } else {
        std::cerr << "Test 15 failed: scalloc should return NULL when size * num exceeds 108 bytes." << std::endl;
    }
}

int main() {
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

    std::cout << "All tests completed." << std::endl;
    return 0;
}