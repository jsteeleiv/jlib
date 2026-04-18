#pragma once
#ifndef JMATH_H
#define JMATH_H

#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#ifndef VEC_CAP
#define VEC_CAP 8
#endif

#ifndef MAT_CAP
#define MAT_CAP 8
#endif

#ifndef COORD_CAP
#define COORD_CAP 8
#endif

#ifndef max
#define max( a,b ) ( (( a ) > ( b )) ? ( a ) : ( b ) )
#endif
#ifndef min
#define min( a,b ) ( (( a ) < ( b )) ? ( a ) : ( b ) )
#endif

typedef struct Vector {

} vector_t;

typedef struct Matrix {

} matrix_t;

typedef struct Rational {

} rational_t;

typedef struct Polynomial {

} polynomial_t;

typedef struct Complex {

} complex_t;

typedef struct Coordinate {

} coord_t;

typedef struct Fibonacci {

} fibonacci_t;


#endif /* JMATH_H */
// #define JMATH_IMPL // #debug-mode
#ifdef JMATH_IMPL

// static inline int invsqroot = 0x5f3759df - (i >> 1);


/* kaprekar's constant {6174}
-----------------------------
    start with a 4 digit number, must not be the same (i.e. 1111, 2222)
    sort form hi to low, sort another from low to hi
    subtract low2hi from hi2lo
    repeat process with new value increment counter
    will always end on `6174`
*/

static inline int kaprekar(int val){
    digitarr_t arr = int2arr(val);
    int result = val;
    int count = 0;
    while (result != 6174){
        int hi2lo = arr2int(digitsort_desc(arr));
        int lo2hi = arr2int(digitsort_asc(arr));

        if (hi2lo == lo2hi){
            printf("Identical digits. Unable to compute Kaprekar's constant!");
            return 0;
        }
        result = hi2lo - lo2hi;
        arr = int2arr(result);
        count++;
    }
    
    printf("result: %d | iterations: %d\n", result, count);
    return result;

}

#endif /* JMATH_IMPL */