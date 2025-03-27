#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

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
typedef long word;
#define lsize sizeof(word)
#define lmask (lsize-1)
void custom_memcpy_op1(void *dest, const void *src, size_t n)
{
    /* The idea is to use the alignment to word
    if both source and destination are mis alligned, copy-byte by byte .
    if src and destination are realtively alligned. copy the unaligned number of bytes first and then copy word by word*/
    char *d = (char *)dest;
    const char *s = (char * )src;
    int len; // to get the raltive misalligned bytes
    if( (n == 0) || (dest == src))
        return;
    //get if src and dest word aligned or not
    /*if below is false means word alligned */
    if( ((long)d | (long)s) & lmask ) // (long)d ^ (long)s -> 00->aligned(can directly copy word by word) 01->not at all aligned 02->short aligned 
    {
        //check if src and destination are realtively alligned
        if(((long)d ^ (long)s) & lmask)
        {
            //raltively misalligned ,because if any bit is set means either one of the address have one bit set
            len = n;  //copy all byte by byte
        }
        else
        {
            //relatively alligned
            len = lsize - ((long)d & lmask);
        }
        n -= len;
        while(len--)
            *d++ = *s++;
    }
    //copy word-by word
    for(len = n / lsize; len > 0; len--)
    {
        *(word *)d = *(word *)s;
        d += lsize;
        s += lsize;
    }
    //for the reamianig
    for(len = n & lmask; len >0; len--)
        *d++ = *s++;


}
#define LARGE_SIZE (10 * 1024 * 1024) // 10 MB

int main()
{
    // Allocate large source and destination buffers
    char *src_large = (char *)malloc(LARGE_SIZE);
    char *dst1 = (char *)malloc(LARGE_SIZE);
    char *dst2 = (char *)malloc(LARGE_SIZE);
    char *dst3 = (char *)malloc(LARGE_SIZE);

    if (!src_large || !dst1 || !dst2 || !dst3) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Fill the source with some data
    for (size_t i = 0; i < LARGE_SIZE; ++i)
        src_large[i] = (char)(i & 0xFF);

    printf("\n--- Large Buffer Copy Benchmark (10 MB) ---\n");

    measure_execution_time(custom_memcpy, dst1, src_large, LARGE_SIZE);
    measure_execution_time(custom_memcpy_op, dst2, src_large, LARGE_SIZE);
    measure_execution_time(custom_memcpy_op1, dst3, src_large, LARGE_SIZE);

    // Verify all results are same
    if (memcmp(dst1, dst2, LARGE_SIZE) == 0 && memcmp(dst1, dst3, LARGE_SIZE) == 0)
        printf("✅ All buffers copied correctly!\n");
    else
        printf("❌ Mismatch in buffer copies!\n");

    free(src_large);
    free(dst1);
    free(dst2);
    free(dst3);

    return 0;
}

/* NOTES
1. Discuss the fallback mechanisim for _op.
As during misalligned access, ARM can through bus fault.Its better to handle by checking the allignment 
and if its not 4 byte alligned, move to byte by byte copy
if (((uintptr_t)d | (uintptr_t)s) & (sizeof(int) - 1)) {
    // fallback to byte-wise copy or safe memcpy
}
    
2. Why Use uintptr_t for Pointer Arithmetic or Bitwise Operations?
✅ TL;DR:
Use uintptr_t when you need to safely convert a pointer to an integer for:

Bitwise operations (like alignment checking)

Arithmetic (like offset computation)

Casting back to a pointer

🧠 Background: What's the Problem?
In your code, you originally used:

c
Copy
Edit
(long)d
This is unsafe or non-portable because:

long might not be big enough to hold a pointer on some architectures (e.g., 32-bit long on a 64-bit system).

It creates a type-punning risk and violates the strict aliasing rule if you're not careful.

It's not guaranteed by the standard to work safely across platforms.

✅ uintptr_t is the Correct Tool
uintptr_t is an unsigned integer type guaranteed to be large enough to hold any pointer value.

Defined in <stdint.h>:

c
Copy
Edit
#include <stdint.h>

uintptr_t d_addr = (uintptr_t)d;
Now, d_addr is an integer you can:

Apply bitwise operations to (like &, |, ^)

Mask with alignment masks

Compare addresses

Cast back safely: (void *)d_addr

💡 Example
c
Copy
Edit
if (((uintptr_t)d | (uintptr_t)s) & (sizeof(long) - 1)) {
    // At least one of the pointers is unaligned to word size
}
This is perfectly valid and portable.

📌 Summary
Feature	(long)d	(uintptr_t)d
Safe on all platforms	❌ No	✅ Yes
Handles 64-bit safely	❌ Maybe not	✅ Yes
Standards-compliant	❌ Not always	✅ Fully portable
Bitwise safe	❌ Risky	✅ Yes

You can impress by saying:

"Instead of casting to long, I used uintptr_t from <stdint.h> to ensure my code is portable across both 32-bit and 64-bit platforms and safe for pointer arithmetic and bitwise alignment checks."

🔥 That’s exactly the kind of detail that makes you stand out.*/
