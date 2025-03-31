#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
int isSpace(char c)
{
    if(c == ' ')
        return 1;
    return 0;
}


long int strtol(const char *sptr, char **endptr, int base)
{
    int neg = 0;
    long int ret = 0;
   // int base = 0; //0,8,16
    if((sptr == NULL ) || ((*sptr) == '\0'))
    {
        printf("Error input\n");
        if(endptr)
            *endptr = NULL;
        return -1;
    }
    // check for leading space
    while(isSpace(*sptr))
        sptr++;
    //check for -ve flag
    if((*sptr) == '+')
        sptr++;
    else if((*sptr) == '-')
    {
        neg = 1;
        sptr++;
    }

    //what is the base value 
    if( ((base == 0) || (base == 16)) && ((sptr[0] == '0') && ( (sptr[1] == 'x')  || (sptr[1] == 'X'))))
    {
        base = 16;
        sptr += 2;
    }
    else if (  ((base == 0) || (base == 8)) &&  (sptr[0] == '0') )
    {
        base = 8;
        sptr++;
    }
    else if(base == 0)
        base = 10;

    while((*sptr))
    {
        char c= (*sptr);
        int v = -1;
        long new_ret;
        if((c >= 'a') && (c <= 'z'))
            v = c - 'a' + 10;
        else if((c >= 'A') && (c <= 'Z'))
            v = c - 'A' + 10;
        else if((c >= '0') && (c <= '9'))
            v = c - '0';
        if((v < 0) || (v >=base) )
        {
            if(endptr)
                *endptr = (char *)sptr;
            
            break;
        }
        if(ret > ((LONG_MAX - v) / base))
        {
            errno = ERANGE;
            if (neg)
                return LONG_MIN;
            else
                return LONG_MAX;
        }

 
        ret = ret * base + v;
        sptr++;
    }
    if(endptr)
        *endptr = (char *)sptr;
    if(neg)
         ret = -ret;
    return ret;
    
    
        
    
} 

void test_strtol() {
    char *endptr;

        // Test case 5: Invalid characters
    assert(strtol("123abc", &endptr, 10) == 123);
    assert(*endptr == 'a');

    // Test case 1: Decimal input
    assert(strtol("123", &endptr, 10) == 123);
    assert(*endptr == '\0');

    // Test case 2: Hexadecimal input
    assert(strtol("0x1A3", &endptr, 16) == 419);
    assert(*endptr == '\0');

    // Test case 3: Octal input
    assert(strtol("075", &endptr, 8) == 61);
    assert(*endptr == '\0');

    // Test case 4: Negative number
    assert(strtol("-456", &endptr, 10) == -456);
    assert(*endptr == '\0');

    // Test case 5: Invalid characters
    assert(strtol("123abc", &endptr, 10) == 123);
    assert(*endptr == 'a');

    // Test case 6: Leading spaces
    assert(strtol("   789", &endptr, 10) == 789);
    assert(*endptr == '\0');

    // Test case 7: Base 0 (auto-detect)
    assert(strtol("0xFF", &endptr, 0) == 255);
    assert(*endptr == '\0');
    assert(strtol("077", &endptr, 0) == 63);
    assert(*endptr == '\0');
    assert(strtol("123", &endptr, 0) == 123);
    assert(*endptr == '\0');

    // Test case 8: Overflow
    errno = 0; // Reset errno before the call
    assert(strtol("9223372036854775808", &endptr, 10) == LONG_MAX); // Overflow case
    assert(errno == ERANGE); // Check that errno is set to ERANGE
 


    // Test case 9: Empty string
    assert(strtol("", &endptr, 10) == -1);
    assert(endptr == NULL);

    // Test case 10: NULL input
    assert(strtol(NULL, &endptr, 10) == -1);
    assert(endptr == NULL);

    printf("All test cases passed!\n");
}

int main() {
    test_strtol();
    return 0;
}