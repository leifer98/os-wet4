#include <iostream>

// Forward declaration of smalloc function
void* smalloc(size_t size);

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

// // Test 7: Simulate sbrk failure, cannot be perfomed without mocking
// void test7() {
//     std::cout << "Test 7: Simulate sbrk failure is not directly testable without mocking." << std::endl;
// }

// // Test 8: Test with size_t overflow (not directly testable without manipulating system limits)
// void test8() {
//     std::cout << "Test 8: Simulate size_t overflow is not directly testable without extreme cases." << std::endl;
// }

// // Test 9: Ensure allocated memory is properly aligned
// void test9() {
//     void* ptr = smalloc(4);
//     if (reinterpret_cast<uintptr_t>(ptr) % alignof(max_align_t) == 0) {
//         std::cout << "Test 9 passed: Allocated memory is properly aligned." << std::endl;
//     } else {
//         std::cerr << "Test 9 failed: Allocated memory is not properly aligned." << std::endl;
//     }
// }

int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    // test7();
    // test8();
    // test9();

    std::cout << "All tests completed." << std::endl;
    return 0;
}
