#include <stdio.h>
#include <time.h>
#include <stddef.h>

// Function to calculate the time difference
void measure_execution_time(void (*func)(void *dst, const void *src, size_t n),
                            void *dst, const void *src, size_t n)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    func(dst, src, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %.15lf seconds\n", elapsed);
}

// Simple byte-by-byte memcpy
void custom_memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;

    while (n--) {
        *d++ = *s++;
    }
}

// Helper for chunk-based copying
void copy_chunk(char *d, const char *s, int chunk_size, int size)
{
    for (int i = 0; i < chunk_size; i++) {
        if (size == 4) {
            *(int *)d = *(const int *)s;
            d += sizeof(int);
            s += sizeof(int);
        } else if (size == 2) {
            *(short *)d = *(const short *)s;
            d += sizeof(short);
            s += sizeof(short);
        } else {
            *d = *s;
            d++;
            s++;
        }
    }
}

// Optimized memcpy with chunked copying
void custom_memcpy_op(void *dest, const void *src, size_t n)
{
    if (n == 0 || dest == NULL || src == NULL) return;

    char *d = (char *)dest;
    const char *s = (const char *)src;

    // Copy 4-byte chunks
    int chunk_4 = n / sizeof(int);
    copy_chunk(d, s, chunk_4, 4);

    int offset = chunk_4 * 4;

    // Copy 2-byte chunks
    int remaining = n - offset;
    int chunk_2 = remaining / sizeof(short);
    copy_chunk(d + offset, s + offset, chunk_2, 2);

    offset += chunk_2 * 2;

    // Copy 1-byte chunks
    remaining = n - offset;
    copy_chunk(d + offset, s + offset, remaining, 1);
}
/*
    This is optimized.However if the source address or destination address is not aligned to 4 bytes, it will not work properly.
    If ARM architecture is used, it will not work properly may result in bus error/fault or segmentation fault.
    This is because ARM architecture requires the address to be aligned to 4 bytes for 4 byte data type.
    one solution is present in freeRTOS code in the below linkwith most optimized way.
    https://github.com/aws/amazon-freertos/blob/main/vendors/xilinx/boards/microzed/aws_tests/application_code/xilinx_code/memcpy.c
    
*/
int main()
{
    const char src[20] = "1234567890123456789\0"; // Source string
    char dest[100] = {0}; // Initialize destination array to zero

    measure_execution_time(custom_memcpy, dest, src+1, sizeof(src));
    measure_execution_time(custom_memcpy_op, dest, src+1, sizeof(src));

    printf("Copied string: %s\n", dest);
    return 0;
}
