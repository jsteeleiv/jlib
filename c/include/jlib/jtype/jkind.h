#pragma once
#ifndef JKIND_H
#define JKIND_H

#include <stdio.h>
#include <stdbool.h>

/* kinds of types */
typedef enum Kind {
    ISNULL = 0,
/* _Generic C options */
    BOOL, CHAR, SCHAR, UCHAR,
/* scalar values */
    SHORT, USHORT, INT, UINT,
    LONG, ULONG, LLONG, ULLONG,
    FLOAT, DOUBLE, LDOUBLE, 
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
/* fallback */
    EXPRESSION, UNKNOWN,
/* custom types */
    CUSTOM, // boundary marker
    ANSI, ERROR, LIST, 
    MAP, STACK, TIME,  VEC,
    LOG, BITS, STRUCT, FUNC, 
    ANY, VALUE, VOID, NONE,
/* size variable */
    KINDSIZE
} kind_t;

const char *kind_name(kind_t k);
size_t kind_size(kind_t k);
bool kind_custom(kind_t k);
bool kind_valid(kind_t k);
bool kind_scalar(kind_t k);



#endif /* JKIND_H */

#define JKIND_IMPL
#ifdef JKIND_IMPL

#include <stdlib.h>

static const char *kind_names[KINDSIZE] = {
    [ISNULL]      = "null",

    /* _Generic C options */
    [BOOL]      = "bool",
    [CHAR]      = "char",
    [SCHAR]     = "signed char",
    [UCHAR]     = "unsigned char",
    [SHORT]     = "short",
    [USHORT]    = "unsigned short",
    [INT]       = "int",
    [UINT]      = "unsigned int",
    [LONG]      = "long",
    [ULONG]     = "unsigned long",
    [LLONG]     = "long long",
    [ULLONG]    = "unsigned long long",
    [FLOAT]     = "float",
    [DOUBLE]    = "double",
    [LDOUBLE]   = "long double",

    /* signed integers */
    [I8]        = "i8",
    [I16]       = "i16",
    [I32]       = "i32",
    [I64]       = "i64",

    /* unsigned integers */
    [U8]        = "u8",
    [U16]       = "u16",
    [U32]       = "u32",
    [U64]       = "u64",

    /* floating point */
    [F32]       = "f32",
    [F64]       = "f64",

    /* string types */
    [CSTR]      = "const char *",
    [STR]       = "char *",

    /* generic pointers */
    [PTR]       = "void *",
    [CONST_PTR] = "const void *",
};

static const size_t kind_sizes[KINDSIZE] = {
    [ISNULL]      = 0,
    [BOOL]      = sizeof(bool),
    [CHAR]      = sizeof(char),
    [SCHAR]     = sizeof(signed char),
    [UCHAR]     = sizeof(unsigned char),
    [SHORT]     = sizeof(short),
    [USHORT]    = sizeof(unsigned short),
    [INT]       = sizeof(int),
    [UINT]      = sizeof(unsigned int),
    [LONG]      = sizeof(long),
    [ULONG]     = sizeof(unsigned long),
    [LLONG]     = sizeof(long long),
    [ULLONG]    = sizeof(unsigned long long),
    [FLOAT]     = sizeof(float),
    [DOUBLE]    = sizeof(double),
    [LDOUBLE]   = sizeof(long double),

    [I8]        = sizeof(int8_t),
    [I16]       = sizeof(int16_t),
    [I32]       = sizeof(int32_t),
    [I64]       = sizeof(int64_t),

    [U8]        = sizeof(uint8_t),
    [U16]       = sizeof(uint16_t),
    [U32]       = sizeof(uint32_t),
    [U64]       = sizeof(uint64_t),

    [F32]       = sizeof(float),
    [F64]       = sizeof(double),

    [CSTR]      = sizeof(const char *),
    [STR]       = sizeof(char *),

    [PTR]       = sizeof(void *),
    [CONST_PTR] = sizeof(const void *),

    [CUSTOM]    = 0,
    [ANSI]      = 0,
    [ERROR]     = 0,
    [LIST]      = 0,
    [MAP]       = 0,
    [STACK]     = 0,
    [TIME]      = 0,
    [VEC]       = 0,
    [LOG]       = 0,
    [BITS]      = 0,
    [STRUCT]    = 0,
    [FUNC]      = 0,
    [ANY]       = 0,
    [VALUE]     = 0,
    [VOID]      = 0
};

const char *kind_name(kind_t k){
    if (!kind_valid(k)) return "! Unsupported Kind";
    return kind_names[k] ? kind_names[k] : "! Be more `kind` 💀 : Let's Rewind 📼 ! ";
}

size_t kind_size(kind_t k){
    if (!kind_valid(k)) return 0;
    return kind_sizes[k];
}

bool kind_custom(kind_t k){
    return (kind_valid(k) ? ((unsigned)k > CUSTOM) :false);
}

bool kind_valid(kind_t k) {
    return (unsigned)k < KINDSIZE;
}
bool kind_scalar(kind_t k){
    return ((unsigned)k >= SHORT && (unsigned)k <= F64);
}

#endif /* JKIND_IMPL */