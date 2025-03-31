#include <stdio.h>
#include <string.h>


typedef long word;
#define lsize sizeof(word)
#define lmask lsize-1
void* custom_memmove(void *dest, void const *src, size_t count)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    int len;

    if (count == 0 || dest == src)
        return dest;

    if ((long)d < (long)s) {
        if (((long)d | (long)s) & lmask) {
            // src and/or dest do not align on word boundary
            if ((((long)d ^ (long)s) & lmask) || (count < lsize))
                len = count; // copy the rest of the buffer with the byte mover
            else
                len = lsize - ((long)d & lmask); // move the ptrs up to a word boundary

            count -= len;
            for (; len > 0; len--)
                *d++ = *s++;
        }
        for (len = count / lsize; len > 0; len--) {
            *(word *)d = *(word *)s;
            d += lsize;
            s += lsize;
        }
        for (len = count & lmask; len > 0; len--)
            *d++ = *s++;
    } else {
        d += count;
        s += count;
        if (((long)d | (long)s) & lmask) {
            // src and/or dest do not align on word boundary
            if ((((long)d ^ (long)s) & lmask) || (count <= lsize))
                len = count;
            else
                len = ((long)d & lmask);

            count -= len;
            for (; len > 0; len--)
                *--d = *--s;
        }
        for (len = count / lsize; len > 0; len--) {
            d -= lsize;
            s -= lsize;
            *(word *)d = *(word *)s;
        }
        for (len = count & lmask; len > 0; len--)
            *--d = *--s;
    }

    return dest;
    
    
}

void test_custom_memmove() {
    char src1[] = "Hello, World!";
    char dest1[20];
    custom_memmove(dest1, src1, strlen(src1) + 1);
    printf("Test 1: %s\n", strcmp(dest1, src1) == 0 ? "Passed" : "Failed");

    char src2[] = "Overlap Test";
    custom_memmove(src2 + 3, src2, strlen(src2) + 1);
    printf("Test 2: %s\n", strcmp(src2, "OveOverlap Test") == 0 ? "Passed" : "Failed");

    char src3[] = "Backward Copy";
    custom_memmove(src3, src3 + 3, strlen(src3 + 3) + 1);
    printf("Test 3: %s\n", strcmp(src3, "ward Copy") == 0 ? "Passed" : "Failed");

    char src4[] = "Short";
    char dest4[10];
    custom_memmove(dest4, src4, 3);
    dest4[3] = '\0';
    printf("Test 4: %s\n", strcmp(dest4, "Sho") == 0 ? "Passed" : "Failed");

    char src5[] = "Aligned Test";
    char dest5[20];
    custom_memmove(dest5, src5, strlen(src5) + 1);
    printf("Test 5: %s\n", strcmp(dest5, src5) == 0 ? "Passed" : "Failed");
}

int main() {
    test_custom_memmove();
    return 0;
}