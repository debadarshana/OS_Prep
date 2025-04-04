#include <stdio.h>
#include <stdint.h>
#if 0
int rev_bit(int num)
{

    int res = 0;
    for(int i = 0; i < 32; i++)
    {
        int bit = num & 1;
        res |= bit <<(31-i);
        num >>=1;

    }
    return res;
}
    #endif
uint8_t rev_byte[256] = {0};

#include <stdint.h>

uint8_t rev_byte[256];

void calc_tab() {
    for (uint16_t i = 0; i < 256; i++) {
        uint8_t x = i, rev = 0;
        for (uint8_t j = 0; j < 8; j++) {
            rev = (rev << 1) | (x & 1);
            x >>= 1;
        }
        rev_byte[i] = rev;
    }
}

uint32_t rev_bit(uint32_t num) {
    return  ((uint32_t)rev_byte[num & 0xff] << 24) |
            ((uint32_t)rev_byte[(num >> 8) & 0xff] << 16) |
            ((uint32_t)rev_byte[(num >> 16) & 0xff] << 8) |
            ((uint32_t)rev_byte[(num >> 24) & 0xff]);
}


void test_rev_bit()
{
    // Test case 1: Reverse bits of 0
    int num = 0;
    int expected = 0;
    int result = rev_bit(num);
    if (result == expected)
        printf("Test case 1 passed.\n");
    else
        printf("Test case 1 failed. Expected %d, got %d\n", expected, result);

    // Test case 2: Reverse bits of 1
    num = 1;
    expected = 0x80000000; // 1 reversed is the MSB set
    result = rev_bit(num);
    if (result == expected)
        printf("Test case 2 passed.\n");
    else
        printf("Test case 2 failed. Expected %x, got %x\n", expected, result);

    // Test case 3: Reverse bits of 0xFFFFFFFF
    num = 0xFFFFFFFF;
    expected = 0xFFFFFFFF; // All bits are 1, so reversed is the same
    result = rev_bit(num);
    if (result == expected)
        printf("Test case 3 passed.\n");
    else
        printf("Test case 3 failed. Expected %x, got %x\n", expected, result);

    // Test case 4: Reverse bits of 0x12345678
    num = 0x12345678;
    expected = 0x1E6A2C48; // Manually calculated reversed bits
    result = rev_bit(num);
    if (result == expected)
        printf("Test case 4 passed.\n");
    else
        printf("Test case 4 failed. Expected %x, got %x\n", expected, result);
}

int main()
{
    calc_tab();
    test_rev_bit();
    return 0;
}