#pragma once
#ifndef JMATH_H
#define JMATH_H

#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <jlib/jlib.h>

#ifndef SET_CAP
#define SET_CAP 8
#endif

#ifndef VEC_CAP
#define VEC_CAP 8
#endif

#ifndef MAT_CAP
#define MAT_CAP 8
#endif

#ifndef COORD_CAP
#define COORD_CAP 8
#endif

#define MAX_DIGITS 10

typedef struct Set {
    value_t *data;
    size_t size;
    size_t cap;
} set_t;

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

/* lifecycle */
static inline set_t *set_mk(void);
static inline void set_die(set_t *set);

/* basic operations */
static inline bool set_ins(set_t *set, value_t val);
static inline bool set_del(set_t *set, value_t val);
static inline bool set_rsz(set_t *set, size_t cap);
static inline bool set_has(const set_t *set, value_t val);
static inline void set_print(const set_t *set);
static inline size_t set_size(const set_t *set);

/* set algebra */
static inline set_t *set_diff(const set_t *a, const set_t *b);
static inline set_t *set_union(const set_t *a, const set_t *b);
static inline set_t *set_intersect(const set_t *a, const set_t *b);



typedef struct DigitArray {
    int digits[MAX_DIGITS];
    int length;
    bool negative;
} digitarr_t;

static inline digitarr_t int2arr(int n);
static inline int arr2int(digitarr_t arr);
static inline digitarr_t digitsort_asc(digitarr_t arr);
static inline digitarr_t digitsort_desc(digitarr_t arr);

#endif /* JMATH_H */
// #define JMATH_IMPL // #debug-mode
#ifdef JMATH_IMPL

// static inline int invsqroot = 0x5f3759df - (i >> 1);

static inline set_t *set_mk(void){
    set_t *s = (set_t*) malloc(sizeof(set_t));
    if (!s) return NULL;
    s->data = (value_t*) malloc(SET_CAP * sizeof(value_t));
    if (!s->data){
        free(s);
        return NULL;
    }
    s->size = 0;
    s->cap = SET_CAP;
    return s;
}

static inline void set_die(set_t *s){
    if (!s) return;
    free(s->data);
    free(s);
}

static inline bool set_ins(set_t *s, value_t val){
    if (!s) return false;
    if (set_has(s, val)) return false;
    if (s->size == s->cap){
        if (!set_rsz(s, s->cap * 2))return false;
    }
    s->data[s->size++] = val;
    return true;
}

static inline bool set_del(set_t *s, value_t val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i].type.any.src == val.type.any.src){
            s->data[i] = s->data[s->size - 1];
            s->size--;
            return true;
        }
    }
    return false;
}

static inline bool set_rsz(set_t *s, size_t cap){
    value_t *val = (value_t*) realloc(s->data, cap * sizeof(value_t));
    if (!val) return false;
    s->data = val;
    s->cap = cap;
    return true;
}

static inline bool set_has(set_t *s, value_t val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i].type.any.src == val.type.any.src) return true;
    }
    return false;
}

static inline void set_print(const set_t *s){
    if (s == NULL){
        printf("{ }\n");
        return;
    }

    printf("{ ");
    for (size_t i = 0; i < s->size; i++) {
        printf("%d", s->data[i]);
        if (i + 1 < s->size) {
            printf(", ");
        }
    }
    printf(" }\n");
}

static inline size_t set_size(set_t *s){
    return s ? s->size : 0;
}

static inline set_t *set_diff(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_has(b, a->data[i])) set_ins(c, a->data[i]);
    }
    return c;
}

static inline set_t *set_union(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_ins(c, a->data[i])) {
            /* duplicate is fine, allocation failure is harder to distinguish here */
        }
    }
    for (size_t i = 0; i < b->size; i++) {
        if (!set_ins(c, b->data[i])) {
            /* duplicate is fine */
        }
    }
    return c;
}

static inline set_t *set_intersect(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++) {
        if (set_has(b, a->data[i])) set_ins(c, a->data[i]);
    }
    return c;
}

static inline digitarr_t int2arr(int n){
    if (n < 0) n = -n;
    digitarr_t ret;
    ret.length = 0;
    if (n == 0) {
        ret.digits[0] = 0;
        ret.length = 1;
        ret.negative = false;
    }
    
    while (n > 0 && ret.length < MAX_DIGITS){
        ret.digits[ret.length++] = n % 10;
        n /= 10;
    }
    for (int i = 0; i < ret.length/2; i++){
        int tmp = ret.digits[i];
        ret.digits[i] = ret.digits[ret.length - i - 1];
        ret.digits[ret.length - i - 1] = tmp;
    }
    return ret;

}

static inline int arr2int(digitarr_t arr){
    int ret = 0;
    for (int i = 0; i < arr.length; i++){
        ret = ret * 10 + arr.digits[i];
    }
    return ret;
}

static inline digitarr_t digitsort_asc(digitarr_t arr){
    for (int i = 0; i < arr.length - 1; i++){
        for (int j = 0; j < arr.length - i - 1; j++){
            if (arr.digits[j] > arr .digits[j + 1]){
                int tmp = arr.digits[j];
                arr.digits[j] = arr.digits[j + 1];
                arr.digits[j + 1] = tmp;
            }
        }
    }
    return arr;
}

static inline digitarr_t digitsort_desc(digitarr_t arr){
    for (int i = 0; i < arr.length - 1; i++){
        for (int j = 0; j < arr.length - i - 1; j++){
            if (arr.digits[j] < arr .digits[j + 1]){
                int tmp = arr.digits[j];
                arr.digits[j] = arr.digits[j + 1];
                arr.digits[j + 1] = tmp;
            }
        }
    }
    return arr;
}

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