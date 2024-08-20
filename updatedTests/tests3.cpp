#include <cassert>
#include <math.h>
#include <iostream>

// Forward declarations of structures
typedef struct MallocMetadata metaData;

// Declaration of global variables
extern int md_size;

// Declaration of functions
size_t _size_meta_data();
void* smalloc(size_t size);
void* scalloc(size_t num, size_t size);
void sfree(void* p);
void* srealloc(void* oldp, size_t size);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();

using namespace std;
int md_size = _size_meta_data();

#define MAX_ALLOC_SIZE (100000000)
#define MAX_ORDER (10)
#define MIN_BLOCK_SIZE (128)
#define MAX_BLOCK_SIZE (128*1024)
#define METADATA_SIZE (sizeof(MallocMetadata))

#define GET_METADATA_PTR(ptr) ((void*)((char*)(ptr) - METADATA_SIZE))
#define GET_DATA_PTR(ptr) ((void*)((char*)(ptr) + METADATA_SIZE))

#define RUN_TEST(test) do { \
    std::cout << "Running " << #test << "... "; \
    if (test()) { \
        std::cout << "[PASS]" << std::endl; \
    } else { \
        std::cout << "[FAIL]" << std::endl; \
    } \
} while(0)


// Helper function to reset allocator state (if needed)
void reset_allocator() {
    // my_allocator = BuddyAllocator();
}

bool test_smalloc_basic() {
    reset_allocator();
    assert(_size_meta_data() == md_size);

    void* initial = sbrk(0);
    void* ptr = smalloc(10);
    assert((char *)ptr - (char*)initial == md_size);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (31 + 11) * METADATA_SIZE);
    assert(_num_free_blocks() == 31 + 10);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - MIN_BLOCK_SIZE);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    void* ptr2 = smalloc(20);
    block = (MallocMetadata*)((char*) ptr2 - md_size);
    assert(pairs[block->order][0] == MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (31 + 11) * METADATA_SIZE);
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - (2 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    return true;
}

bool test_smalloc_validity() {
    reset_allocator();
    // Too large
    void* ptr = smalloc(MAX_ALLOC_SIZE + 1);
    // cannot be 0
    assert(ptr == nullptr);
    ptr = smalloc(0);
    assert(ptr == nullptr);
    return true;
}

bool test_smalloc_mmap() {
    reset_allocator();
    // Too large
    void* ptr = smalloc(MAX_ALLOC_SIZE);
    MallocMetadata * block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == MAX_ALLOC_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + pairs[block->order][0]);
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    return true;
}

bool test_scalloc_basic() {
    reset_allocator();
    void* ptr = scalloc(5, 20);
    char* ptr_data = (char*) ptr;
    for (int i = 0; i < 100; ++i) {
        assert(ptr_data[i] == 0);
    }
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert((char*)ptr - (char*)block == md_size);
    assert(pairs[block->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (2 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);
    return true;
}

bool test_scalloc_validity() {
    reset_allocator();
    // Too large
    void* ptr = scalloc(10, MAX_ALLOC_SIZE);
    // cannot be 0
    assert(ptr == nullptr);
    ptr = scalloc(0, 0);
    assert(ptr == nullptr);
    return true;
}

bool test_scalloc_mmap() {
    reset_allocator();
    // Too large
    void* ptr = scalloc(2, MAX_BLOCK_SIZE);
    MallocMetadata * block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == 2 * MAX_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + pairs[block->order][0]);
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    char* data = (char*) ptr;
    for (int i = 0; i < 2 * MAX_BLOCK_SIZE; ++i) {
        assert(data[i] == 0);
    }
    return true;
}

bool test_sfree_basic() {
    reset_allocator();
    void* ptr = smalloc(26);

    sfree(ptr);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == MAX_BLOCK_SIZE);
    assert(block->is_free == true);
    assert(_num_allocated_blocks() == 32);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == _num_allocated_blocks());
    assert(_num_free_bytes() == _num_allocated_bytes());
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // making sure no exceptions are thrown
    sfree(ptr);

    // inserting another one in the same block
    void* ptr2 = smalloc(260);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == 2 * 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert(_num_allocated_blocks() == 31 + 9);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);
    return true;
}

bool test_sfree_validity() {
    reset_allocator();
    free(nullptr);
    // making sure no exception is thrown
    assert(true);
    void* ptr = scalloc(0, 0);
    assert(ptr == nullptr);
    return true;
}

bool test_sfree_mmap() {
    reset_allocator();
    void* ptr = smalloc(MAX_BLOCK_SIZE + MIN_BLOCK_SIZE);
    sfree(ptr);

    // checking that we are reverted back to initial state
    assert(_num_allocated_blocks() == 32);
    assert(_num_free_blocks() == 32);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    assert(_num_free_bytes() == 32 * MAX_BLOCK_SIZE - _num_meta_data_bytes());
    assert(_num_free_bytes() == _num_allocated_bytes());
    return true;
}

bool test_sfree_already_released() {
    reset_allocator();
    void* ptr = scalloc(20, 60);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(block->is_free == false);

    sfree(ptr);
    assert(block->is_free == true);

    sfree(ptr);
    // making sure no exception is thrown
    assert(true);
    return true;
}

bool test_srealloc_validity() {
    reset_allocator();
    void* dummy = smalloc(1);
    // Too large
    void* ptr = srealloc(dummy, MAX_ALLOC_SIZE + 1);
    assert( ptr == nullptr);
    // cannot be 0
    ptr = srealloc(dummy, 0);
    assert( ptr == nullptr);
    MallocMetadata* dummy_block = (MallocMetadata*) GET_METADATA_PTR(dummy);
    assert(pairs[dummy_block->order][0] == MIN_BLOCK_SIZE);
    assert(dummy_block->is_free == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 10);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[dummy_block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);
    return true;
}

bool test_srealloc_nullptr() {
    reset_allocator();

    void* ptr = srealloc(nullptr, MAX_BLOCK_SIZE - METADATA_SIZE);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == MAX_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 32);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE));
    assert(_num_free_blocks() == 31);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    return true;
}

bool test_srealloc_nullptr_mmap() {
    reset_allocator();

    // just adding 1 so it wouldn't fit in the largest order of block
    void* ptr = srealloc(nullptr, MAX_BLOCK_SIZE - METADATA_SIZE + 1);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == MAX_BLOCK_SIZE - METADATA_SIZE + 1);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + pairs[block->order][0]);
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());
    return true;
}

bool test_srealloc_same_block() {
    reset_allocator();

    // first allocation and same old asserts, will copy and paste them to see that data is persistent
    void* ptr = srealloc(nullptr, 120);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == _num_allocated_blocks() - 1);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    void* ptr2 = srealloc(ptr, MIN_BLOCK_SIZE - METADATA_SIZE + 1);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == _num_allocated_blocks() - 1);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    ptr2 = srealloc(ptr, (2 * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == _num_allocated_blocks() - 1);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    return true;
}

bool test_srealloc_same_block_mmap() {
    reset_allocator();

    // first allocation and same old asserts, will copy and paste them to see that data is persistent
    void* ptr = srealloc(nullptr, 8 * MAX_BLOCK_SIZE);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert(pairs[block->order][0] == 8 * MAX_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + (8 * MAX_BLOCK_SIZE));
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    void* ptr2 = srealloc(ptr, 1);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == 8 * MAX_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + (8 * MAX_BLOCK_SIZE));
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    ptr2 = srealloc(ptr, (2 * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == 8 * MAX_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == true);
    assert(_num_allocated_blocks() == 32 + 1);
    assert(_num_allocated_bytes() == 32 * (MAX_BLOCK_SIZE - METADATA_SIZE) + (8 * MAX_BLOCK_SIZE));
    assert(_num_free_blocks() == 32);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (METADATA_SIZE * _num_free_blocks()));
    assert(_num_meta_data_bytes() == METADATA_SIZE * _num_allocated_blocks());

    return true;
}

bool test_srealloc_merge_right() {
    reset_allocator();

    // occupying smallest block and then freeing it, so we can make sure that we are not getting the same pointer for the wrong reason
    void* occupy = smalloc(MAX_BLOCK_SIZE - METADATA_SIZE);
    assert(((MallocMetadata*) GET_METADATA_PTR(occupy))->order > 10 == false);
    void* ptr = smalloc (1);
    MallocMetadata* block = (MallocMetadata*) GET_METADATA_PTR(ptr);
    assert((char*)ptr > (char*)occupy);
    sfree(occupy);

    assert(pairs[block->order][0] == MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 10);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    void* ptr2 = srealloc(ptr, ((int)pow(2, 1) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 1) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 2) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 2) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 9);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 3) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 3) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 8);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 7);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 4) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 4) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 7);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 6);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 5) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 5) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 6);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 5);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 6) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 6) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 5);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 4);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 7) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 7) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 4);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 3);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 8) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 8) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 3);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 2);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 9) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 9) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 2);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 1);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    ptr2 = srealloc(ptr, ((int)pow(2, 10) * MIN_BLOCK_SIZE) - METADATA_SIZE);
    assert(ptr == ptr2);
    assert(pairs[block->order][0] == ((int)pow(2, 10) * MIN_BLOCK_SIZE));
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 1);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 0);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // More than this won't be checked
    return true;
}

bool test_srealloc_merge_left() {
    reset_allocator();

    // occupying smallest block and then freeing it, so we can make sure that we are not getting the same pointer for the wrong reason
    void *occupy = smalloc(MAX_BLOCK_SIZE - METADATA_SIZE);
    assert(((MallocMetadata *) GET_METADATA_PTR(occupy))->order > 10 == false);
    void *ptr1 = smalloc(1);
    MallocMetadata *block1 = (MallocMetadata *) GET_METADATA_PTR(ptr1);
    assert((char *) ptr1 > (char *) occupy);

    // checking only for 4 blocks (total of 3 merges with left buddy)
    void *ptr2 = scalloc(2, 40);
    void *ptr3 = srealloc(nullptr, 50);
    void *ptr4 = smalloc(12);
    assert(ptr1 < ptr2 && ptr2 < ptr3 && ptr3 < ptr4);
    sfree(occupy);

    // let the merges begin!

    // 1st free + merge, making sure not merging into the occupied one
    sfree(ptr1);
    assert(pairs[block1->order][0] == MIN_BLOCK_SIZE);
    assert(block1->is_free == true);
    assert(block1->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 12);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (3 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);
    void *merged_1_2 = srealloc(ptr2, 2 * MIN_BLOCK_SIZE - METADATA_SIZE);
    assert(ptr1 == merged_1_2);
    assert(pairs[block1->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block1->is_free == false);
    assert(block1->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (4 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // 2nd free + merge
    sfree(ptr3);
    MallocMetadata *block3 = (MallocMetadata *) GET_METADATA_PTR(ptr3);
    assert(pairs[block3->order][0] == MIN_BLOCK_SIZE);
    assert(block3->is_free == true);
    assert(block3->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (3 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    void *merged_3_4 = srealloc(ptr4, 2 * MIN_BLOCK_SIZE - METADATA_SIZE);
    assert(ptr3 == merged_3_4);
    assert(pairs[block3->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block3->is_free == false);
    assert(block3->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (4 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // 3rd free + merge
    sfree(ptr1);
    assert(pairs[block1->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block1->is_free == true);
    assert(block1->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 9);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (2 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    void *merged_1_2_3_4 = srealloc(ptr3, 4 * MIN_BLOCK_SIZE - METADATA_SIZE);
    assert(ptr1 == merged_1_2_3_4);
    assert(pairs[block1->order][0] == 4 * MIN_BLOCK_SIZE);
    assert(block1->is_free == false);
    assert(block1->order > 10 == false);
    assert(_num_allocated_blocks() == 31 + 9);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (4 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // finishing up and making sure we can merge all blocks again
    sfree(ptr1);
    assert(_num_allocated_blocks() == 32);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == _num_allocated_blocks());
    assert(_num_free_bytes() == _num_allocated_bytes());
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    return true;
}

bool test_srealloc_merge_blocked() {
    reset_allocator();

    // occupying smallest block and then freeing it, so we can make sure that we are not getting the same pointer for the wrong reason
    void *occupy = smalloc(2 * MIN_BLOCK_SIZE - METADATA_SIZE);
    void *blocking = smalloc(1);
    sfree(occupy);

    // using a filler to take up the othe MIN_BLOCK_SIZE that was split to create the blocking block
    scalloc(2, 40);
    void *ptr = scalloc(2, 40);
    assert((char*)ptr < (char*)blocking);
    MallocMetadata *block = (MallocMetadata *) GET_METADATA_PTR(ptr);
    // this should be possible, as it fits in exactly right
    void* merged = srealloc(ptr, 2 * MIN_BLOCK_SIZE - METADATA_SIZE);
    assert(ptr == merged);
    assert(pairs[block->order][0] == 2 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (4 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    // resetting
    sfree(ptr);
    ptr = scalloc(2, 40);
    assert((char*)ptr < (char*)blocking);
    // reallocating with a size that doesn't fit
    void* ptr2 = srealloc(ptr, 2 * MIN_BLOCK_SIZE - METADATA_SIZE + 1);
    assert((char*)ptr2 > (char*)blocking);
    block = (MallocMetadata *) GET_METADATA_PTR(ptr2);
    assert(ptr2 > blocking);
    assert(pairs[block->order][0] == 4 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 11);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (6 * MIN_BLOCK_SIZE));
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    return true;
}

bool test_srealloc_no_buddy_to_merge() {
    reset_allocator();

    // occupy buddy
    smalloc(2 * MIN_BLOCK_SIZE - METADATA_SIZE);

    void *ptr = smalloc(2 * MIN_BLOCK_SIZE - METADATA_SIZE);
    void* ptr2 = srealloc(ptr, 2 * MIN_BLOCK_SIZE - METADATA_SIZE + 1);
    assert((char*)ptr < (char*)ptr2);
    MallocMetadata *block = (MallocMetadata *) GET_METADATA_PTR(ptr2);
    assert(pairs[block->order][0] == 4 * MIN_BLOCK_SIZE);
    assert(block->is_free == false);
    assert((block->order > 10) == false);
    assert(_num_allocated_blocks() == 31 + 10);
    assert(_num_allocated_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_allocated_blocks() * METADATA_SIZE));
    assert(_num_free_blocks() == 31 + 8);
    assert(_num_free_bytes() == (32 * MAX_BLOCK_SIZE) - (_num_free_blocks() * METADATA_SIZE) - (2 * MIN_BLOCK_SIZE) - pairs[block->order][0]);
    assert(_num_meta_data_bytes() == _num_allocated_blocks() * METADATA_SIZE);

    return true;
}

bool test_srealloc_data_copy_on_merge() {
    reset_allocator();
    void* ptr = smalloc(50);
    char* data = (char*) ptr;
    for (int i = 0; i < 50; ++i) {
        data[i] = i;
    }
    void* ptr2 = srealloc(ptr, 200);
    assert(ptr == ptr2);
    assert(pairs[((MallocMetadata*) GET_METADATA_PTR(ptr))->order][0] == (2 * MIN_BLOCK_SIZE));
    data = (char*) ptr2;
    for (int i = 0; i < 50; ++i) {
        assert(data[i] == i);
    }
    return true;
}

bool test_srealloc_data_copy_new_block() {
    reset_allocator();
    // occupying buddy
    smalloc(1);
    void* ptr = smalloc(50);
    char* data = (char*) ptr;
    for (int i = 0; i < 50; ++i) {
        data[i] = i;
    }
    void* ptr2 = srealloc(ptr, 200);
    assert((char*) ptr2 > (char*) ptr);
    assert(pairs[((MallocMetadata*) GET_METADATA_PTR(ptr2))->order][0] == (2 * MIN_BLOCK_SIZE));
    data = (char*) ptr2;
    for (int i = 0; i < 50; ++i) {
        assert(data[i] == i);
    }
    return true;
}

bool test_srealloc_data_copy_mmap() {
    reset_allocator();

    void* ptr = smalloc(MAX_BLOCK_SIZE + 1);
    char* data = (char*) ptr;
    for (int i = 0; i < 127; ++i) {
        data[i] = i;
    }
    void* ptr2 = srealloc(ptr, MAX_BLOCK_SIZE + MIN_BLOCK_SIZE + 1);
    data = (char*) ptr2;
    for (int i = 0; i < 127; ++i) {
        assert((int)data[i] == i);
    }
    return true;
}

bool test_list_order() {
    reset_allocator();
    void* ptrs[16];
    for (int i = 0; i < 16; ++i) {
        ptrs[i] = smalloc(i + 1);
    }
    // freeing all of the even-indexed pointers "randomly" and checking that afterwards blocks are given in an ascending order
    sfree(ptrs[14]);
    sfree(ptrs[2]);
    sfree(ptrs[6]);
    sfree(ptrs[0]);
    sfree(ptrs[8]);
    sfree(ptrs[4]);
    sfree(ptrs[12]);
    sfree(ptrs[10]);

    for (int i = 0; i < 16; i += 2) {
        ptrs[i] = smalloc(i + 1);
    }
    for (int i = 2; i < 16; i += 2) {
        assert((char*) ptrs[i - 2] < (char*) ptrs[i]);
    }
    return true;
}


int main() {
    // probably throwing program break off alignment
    sbrk(5);
    cout << sbrk(0) <<endl;
    RUN_TEST(test_smalloc_basic);
    RUN_TEST(test_smalloc_validity);
    RUN_TEST(test_smalloc_mmap);

    RUN_TEST(test_scalloc_basic);
    RUN_TEST(test_scalloc_validity);
    RUN_TEST(test_scalloc_mmap);

    RUN_TEST(test_sfree_basic);
    RUN_TEST(test_sfree_already_released);
    RUN_TEST(test_sfree_validity);
    RUN_TEST(test_sfree_mmap);

    RUN_TEST(test_srealloc_validity);
    RUN_TEST(test_srealloc_nullptr);
    RUN_TEST(test_srealloc_nullptr_mmap);
    RUN_TEST(test_srealloc_same_block);
    RUN_TEST(test_srealloc_same_block_mmap);
    RUN_TEST(test_srealloc_merge_right);
    RUN_TEST(test_srealloc_merge_left);
    RUN_TEST(test_srealloc_merge_blocked);
    RUN_TEST(test_srealloc_no_buddy_to_merge);
    RUN_TEST(test_srealloc_data_copy_on_merge);
    RUN_TEST(test_srealloc_data_copy_new_block);
    RUN_TEST(test_srealloc_data_copy_mmap);

    RUN_TEST(test_list_order);

    cout << "All tests completed." << endl;
    return 0;
}