#pragma once
#ifndef JKIND_H
#define JKIND_H

#include <stdio.h>
#include <stdbool.h>

/* kinds of types */
typedef enum Kind {
    J_NULL = 0,
/* _Generic C options */
    J_BOOL, J_CHAR, J_SCHAR, J_UCHAR,
/* scalar values */
    J_SHORT, J_USHORT, J_INT, J_UINT,
    J_LONG, J_ULONG, J_LLONG, J_ULLONG,
    J_FLOAT, J_DOUBLE, J_LDOUBLE, 
/* signed integers */
    J_I8, J_I16, J_I32, J_I64,
/* unsigned integers */
    J_U8, J_U16, J_U32, J_U64,
/* floating point */
    J_F32, J_F64,
/* string types */
    J_CSTR, J_STR,
/* generic pointers */
    J_PTR, J_CONST_PTR,
/* fallback */
    EXPRESSION, UNKNOWN,
/* custom types */
    J_CUSTOM, // boundary marker
    J_ANSI, J_ERROR, J_LIST, 
    J_MAP, J_STACK, J_TIME,  J_VEC,
    J_LOG, J_BITS, J_STRUCT, J_FUNC, 
    J_ANY, J_VALUE, J_VOID, J_NONE,
/* size variable */
    J_KINDSIZE
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

static const char *kind_names[J_KINDSIZE] = {
    [J_NULL]      = "null",

    /* _Generic C options */
    [J_BOOL]      = "bool",
    [J_CHAR]      = "char",
    [J_SCHAR]     = "signed char",
    [J_UCHAR]     = "unsigned char",
    [J_SHORT]     = "short",
    [J_USHORT]    = "unsigned short",
    [J_INT]       = "int",
    [J_UINT]      = "unsigned int",
    [J_LONG]      = "long",
    [J_ULONG]     = "unsigned long",
    [J_LLONG]     = "long long",
    [J_ULLONG]    = "unsigned long long",
    [J_FLOAT]     = "float",
    [J_DOUBLE]    = "double",
    [J_LDOUBLE]   = "long double",

    /* signed integers */
    [J_I8]        = "i8",
    [J_I16]       = "i16",
    [J_I32]       = "i32",
    [J_I64]       = "i64",

    /* unsigned integers */
    [J_U8]        = "u8",
    [J_U16]       = "u16",
    [J_U32]       = "u32",
    [J_U64]       = "u64",

    /* floating point */
    [J_F32]       = "f32",
    [J_F64]       = "f64",

    /* string types */
    [J_CSTR]      = "const char *",
    [J_STR]       = "char *",

    /* generic pointers */
    [J_PTR]       = "void *",
    [J_CONST_PTR] = "const void *",
};

static const size_t kind_sizes[J_KINDSIZE] = {
    [J_NULL]      = 0,
    [J_BOOL]      = sizeof(bool),
    [J_CHAR]      = sizeof(char),
    [J_SCHAR]     = sizeof(signed char),
    [J_UCHAR]     = sizeof(unsigned char),
    [J_SHORT]     = sizeof(short),
    [J_USHORT]    = sizeof(unsigned short),
    [J_INT]       = sizeof(int),
    [J_UINT]      = sizeof(unsigned int),
    [J_LONG]      = sizeof(long),
    [J_ULONG]     = sizeof(unsigned long),
    [J_LLONG]     = sizeof(long long),
    [J_ULLONG]    = sizeof(unsigned long long),
    [J_FLOAT]     = sizeof(float),
    [J_DOUBLE]    = sizeof(double),
    [J_LDOUBLE]   = sizeof(long double),

    [J_I8]        = sizeof(int8_t),
    [J_I16]       = sizeof(int16_t),
    [J_I32]       = sizeof(int32_t),
    [J_I64]       = sizeof(int64_t),

    [J_U8]        = sizeof(uint8_t),
    [J_U16]       = sizeof(uint16_t),
    [J_U32]       = sizeof(uint32_t),
    [J_U64]       = sizeof(uint64_t),

    [J_F32]       = sizeof(float),
    [J_F64]       = sizeof(double),

    [J_CSTR]      = sizeof(const char *),
    [J_STR]       = sizeof(char *),

    [J_PTR]       = sizeof(void *),
    [J_CONST_PTR] = sizeof(const void *),

    [J_CUSTOM]    = 0,
    [J_ANSI]      = 0,
    [J_ERROR]     = 0,
    [J_LIST]      = 0,
    [J_MAP]       = 0,
    [J_STACK]     = 0,
    [J_TIME]      = 0,
    [J_VEC]       = 0,
    [J_LOG]       = 0,
    [J_BITS]      = 0,
    [J_STRUCT]    = 0,
    [J_FUNC]      = 0,
    [J_ANY]       = 0,
    [J_VALUE]     = 0,
    [J_VOID]      = 0
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
    return (kind_valid(k) ? ((unsigned)k > J_CUSTOM) :false);
}

bool kind_valid(kind_t k) {
    return (unsigned)k < J_KINDSIZE;
}
bool kind_scalar(kind_t k){
    return ((unsigned)k >= J_SHORT && (unsigned)k <= J_F64);
}

#endif /* JKIND_IMPL */