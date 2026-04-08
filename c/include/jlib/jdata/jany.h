#pragma once
#ifndef JANY_H
#define JANY_H

/*
    The char* `escape hatch`
    `C` gives you ONE universal aliasing type:
        unsigned char*
    This is special:
        It can legally access any object’s bytes

    int x = 0x12345678;
    unsigned char *p = (unsigned char*)&x;

    This is always safe.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "jtype.h"

#define J_KINDOF(x) _Generic((x), \
    _Bool: J_BOOL, \
    char: J_CHAR, \
    signed char: J_SCHAR, \
    unsigned char: J_UCHAR, \
    short: J_SHORT, \
    unsigned short: J_USHORT, \
    int: J_INT, \
    unsigned int: J_UINT, \
    long: J_LONG, \
    unsigned long: J_ULONG, \
    long long: J_LLONG, \
    unsigned long long: J_ULLONG, \
    float: J_FLOAT, \
    double: J_DOUBLE, \
    long double: J_LDOUBLE, \
    char *: J_STR, \
    const char *: J_CSTR, \
    void *: J_PTR, \
    const void *: J_CONST_PTR, \
    default: J_NULL)
/* #enddef */

typedef struct AnyValue {
    kind_t kind;
    union as {
        void *null; bool boolean; uint8_t bits;
        char ch; signed char sch; unsigned char uch;
        short sh; unsigned short ush; long l;
        int i; unsigned int ui; unsigned long ul; long double ld;
        long long ll; unsigned long long ull;
        float f; double d; long double ldd; float f32; double f64;
        const char *cstr; const void *cptr;
        char *str; void *ptr;
        /* fixed width */
        int8_t i8; int16_t i16; int32_t i32; int64_t i64;
        uint8_t u8; uint16_t u16; uint32_t u32; uint64_t u64;
    } as; // <-- https://www.youtube.com/@TsodingDaily
} any_v;

typedef struct Anything {
    typeinfo_t info;
    const void *src;
    any_v type;
} any_t;

bool any_init(any_t *any, kind_t k, const void *src);
kind_t any_kind(const any_t *any);
bool any_isnull(const any_t *any);
//bool any_isnone(const any_t *any);
bool any_setnull(any_t *any);
//bool any_setnone(any_t *any);

#define any_import(any, x) any_init((any), J_KINDOF(x), &(x))

typedef struct ValueType {
    any_t self;
    any_v cast;
} value_t;

bool val_setcast(value_t *val, kind_t k);


#endif /* JANY_H */
#ifdef JANY_IMPL

kind_t any_kind(const any_t *any){
    return any ? any->type.kind : J_NULL;
}

bool any_isnull(const any_t *any){
    return any && any->type.kind == J_NULL;
}

bool any_setnull(any_t *any){
    if (!any) return false;

    memset(any, 0, sizeof(*any));
    any->info = typeinfo_init(J_NULL);
    any->type.kind = J_NULL;
    any->src = NULL;

    return true;
}

// bool any_isnone(const any_t *any){
//     return any && any->type.kind == J_NULL;
// }

// bool any_setnone(any_t *any){
//     if (!any) return false;
//     memset(any, 0, sizeof(*any));
//     any->info = typeinfo_init(J_NULL);
//     any->type.kind = J_NULL;
//     any->src = NULL;

//     return true;
// }

bool any_init(any_t *any, kind_t k, const void *src){
    if (!any || !src || k == J_NULL) return false;
    
    size_t size = kind_size(k);
    if (size == 0 || size > sizeof(any->type.as)) return false;
    
    memset(any, 0, sizeof(*any));
    any->info = typeinfo_init(k);
    any->type.kind = k;
    any->src = src;

    memcpy(&any->type.as, src, size);

    return true;
}

bool val_setcast(value_t *val, kind_t k){
    if (!val || !kind_valid(k)) return false;
    val->cast.kind = k;
    return true;
}

bool val_castas(value_t *val, kind_t k){
    if (!val || !kind_valid(k)) return false;
    val->self.type.kind = k;
    return true;
}

#endif /* JANY_IMPL */
