#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
int isDigit(char c)
{
    return (c >= '0' && c <= '9');
}
static int hexval(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}
int atoi(char* str)
{
    //assuming i will get a NULL ended string
    if((str == NULL) || ((*str) == '\0'))
        return -1;
    //skip leding space
    while((*str) == ' ')
        str++;
    
    int num = 0;
            //check sign
    char isNeg = 0;
    /* if hex val*/
    if( (str[0] == '0') && ((str[1] == 'x')||(str[1] == 'X')))
    {
        str += 2;
        while( (*str) && (isxdigit(*str)))
        {
            num = num * 16 + hexval((*str)++);
        } 

    }
    else
    {

        if((*str) == '-')
        {
            isNeg = 1;
            str++;
        }
        else if((*str) == '+')
        {
            str++;
        }
        // Check if the string contains valid digits
        if (!isDigit(*str)) {
            return -1; // Invalid input
        }
    
        while(*str && (isDigit(*str)))
        {
            int digit = *str - '0';
            // Overflow check
            /*
            num = num * 10 + digit;
            This is fine as long as the result doesn't go beyond INT_MAX (which is 2147483647 for 32-bit signed int). 
            But if num is too big, the next multiplication or addition will overflow silently in C — leading to garbage or undefined behavior.

        ✅ So what does this condition do?

         (num > (INT_MAX - digit) / 10)
            It asks:

            “Is num * 10 + digit going to be bigger than INT_MAX?”

             Since doing num * 10 + digit directly could overflow, we rearrange it algebraically into a safe form.*/
            if (num > (__INT_MAX__ - digit) / 10) 
                return -1;
            num = num * 10 + digit;
            str++;
        }
        while(*str)
        {
            if((*str) != ' ')
                return -1;
            str++;
        }

    }
    
    return isNeg ? -num : num;

   
}

void test_atoi() {
    // Test case 1: Positive number
    assert(atoi("123") == 123);

    // Test case 2: Negative number
    assert(atoi("-123") == -123);

    // Test case 3: Number with leading spaces
    assert(atoi("   456") == 456);

    // Test case 4: Number with trailing spaces
    assert(atoi("789   ") == 789);

    // Test case 5: Number with both leading and trailing spaces
    assert(atoi("   -321   ") == -321);

    // Test case 6: Empty string
    assert(atoi("") == -1);

    // Test case 7: String with invalid characters
    assert(atoi("12a34") == -1);

    // Test case 8: String with only invalid characters
    assert(atoi("abc") == -1);

    // Test case 9: String with only a negative sign
    assert(atoi("-") == -1);

    // Test case 10: String with only spaces
    assert(atoi("   ") == -1);

    printf("All test cases passed!\n");
}

int main() {
    test_atoi();
    return 0;
}