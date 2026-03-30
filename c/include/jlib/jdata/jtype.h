#pragma once
#ifndef JTYPE_H
#define JTYPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <jlib/jcore/jtime.h>
#include <jlib/jcore/jerr.h>
#include <jlib/jdata/jstk.h>
#include <jlib/jdata/jlist.h>
#include <jlib/jdata/jmap.h>

/* kinds of types | will grow over time ? */
typedef enum Kind {
    NONE = 0,
/* basic */
    BOOL,
    /* signed integers */
    I8, I16, I32, I64,
    /* unsigned integers */
    U8, U16, U32, U64,
    /* floating point */
    F32, F64,
    /* string types */
    CSTR, STR,
    /* generic pointers */
    PTR, CONST_PTR,
/* advanced */
    STRUCT, ARRAY, FUNC, 
/* custom types */
    ANY, VALUE, XPRESSN,
    /* fallback */
    OPAQUE_T, NULL_T
} kind_t;

typedef struct AnyValue {
    kind_t kind;
    union as {  // <-- https://www.youtube.com/@TsodingDaily
        bool boolean;
        int8_t i8; int16_t i16; int32_t i32; int64_t i64;
        uint8_t u8; uint16_t u16; uint32_t u32; uint64_t u64;
        float f32; double f64;
        const char *cstr; const void *cptr;
        char *str; void *ptr; void *nil;
    };
} any_v;

typedef struct Anything {
    uint32_t flags;
    size_t size;
    char *name;
    void *data;
    any_v as;
} any_t;

typedef struct TypeInfo {
    const char *name;
    uint8_t *hex;
    size_t size;
    error_t err;
    jtime_t time;
} typeinfo_t;

typedef struct Bitset {
    typeinfo_t type;
    uint8_t *data;
    size_t bits;
    size_t byte;
} bitset_t;

typedef struct Value {
    any_t anytype;
    typeinfo_t info;
    uintptr_t addr;
    bitset_t bitset;
} value_t;

typedef struct Random{
    Jtime time;
    int64_t seed;
    void *abyss;
    float fval;
    int ival;

} rand_t;

typedef struct {
    Jerror err;
    any_t cast;
    value_t val;
    rand_t rand;
} Jtype;

typedef struct {
    Jerror err;
    Jtime time;
    Jtype type;
    Jmap stars;
    char *omen;
    char *read;
    bool omen;
} Oracle;


#endif /* JTYPE_H */
#define JTYPE_IMPL
#ifdef JTYPE_IMPL

#include <stdlib.h>





/* function definitions */
void JTYPE_example(void)
{
    /* implementation */
}

#endif /* JTYPE_IMPL */