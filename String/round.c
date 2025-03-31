#include <stdio.h>
#include <assert.h>


int custom_round(double val)
{
    int x = (int)val;
    double frac = val - (double)x;
    if((frac == 0.5) || (frac == (-0.5)))
    {
        if(x & 0x1)
            return (int)(val > 0) ? val + 0.5: val - 0.5;
        else
            return x;
    }
    else
    {
        if(val >= 0)
            return (int)(val + 0.5);
        else
            return (int)(val - 0.5);
    }

   
}


int main()
{
        // ✅ Regular rounding tests (not .5 cases)
    assert(custom_round(2.3) == 2);
    assert(custom_round(2.7) == 3);
    assert(custom_round(-2.3) == -2);
    assert(custom_round(-2.7) == -3);
    assert(custom_round(1.4) == 1);
    assert(custom_round(-1.6) == -2);
    assert(custom_round(0.49) == 0);
    assert(custom_round(-0.49) == 0);

    // ✅ Whole numbers
    assert(custom_round(3.0) == 3);
    assert(custom_round(-3.0) == -3);
    assert(custom_round(0.0) == 0);
    assert(custom_round(-0.0) == 0);

    // ✅ Round-half-to-even test cases
    assert(custom_round(2.5) == 2);   // even
    assert(custom_round(3.5) == 4);   // even
    assert(custom_round(4.5) == 4);   // even
    assert(custom_round(5.5) == 6);   // even
    assert(custom_round(-2.5) == -2); // even
    assert(custom_round(-3.5) == -4); // even
    assert(custom_round(-4.5) == -4); // even
    assert(custom_round(-5.5) == -6); // even

    // ✅ Edge test cases
    assert(custom_round(0.5) == 0);   // even
    assert(custom_round(1.5) == 2);   // even
    assert(custom_round(-0.5) == 0);  // even
    assert(custom_round(-1.5) == -2); // even
    printf("✅ All tests passed successfully!\n");

    return 0;
}

/* this does not help for negative number*/
#if 0
int custom_round(double val)
{
    int x = val;
    if((val - (double)x) >= 0.5)
        x++;
    return x;
}
#endif

/* the problem is 2.5 or -2.5 . it will take to next integer number 
the math library round function move it to nearset even number
-2.5 = -2
3.5 = 4(nearest even number)
-3.5 = 4
*/
#if 0
int custom_round(double val)
{
    if(val >= 0)
        return (int)(val + 0.5);
    else
        return (int)(val - 0.5);
}
#endif