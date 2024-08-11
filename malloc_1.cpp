#include <unistd.h>

#define MAX_SIZE 100000000  // Define a maximum size limit for allocation which is 10^8

void *smalloc(size_t size)
{
    // Check if size is 0 or exceeds the maximum allowed size
    if (size <= 0 || size > MAX_SIZE)
    {
        return NULL;  // Return NULL if size is invalid
    }

    // Attempt to allocate memory by increasing the program break
    void *program_break = sbrk(size);

    // Check if sbrk failed (returns (void *)-1 on failure) according the assignment pdf
    if (program_break == (void *)(-1))
    {
        return NULL;  // Return NULL if memory allocation failed
    }
    
    return program_break;  // Return the pointer to the allocated memory
}
