#pragma once
#ifndef JANY_H
#define JANY_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/* kinds of types */
typedef enum Kind {
    J_NONE = 0,
/* _Generic C options */
    J_BOOL, J_CHAR, J_SCHAR, J_UCHAR,
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
    J_NIL, EXPRESSION,
/* custom types */
    J_CUSTOM, // boundary marker
    J_ANSI, J_ERROR, J_LIST, 
    J_MAP, J_STACK, J_TIME,  J_VEC,
    J_LOG, J_BITS, J_STRUCT, J_FUNC, 
    J_ANY, J_VALUE, J_VOID,
/* size variable */
    J_KINDSIZE
} kind_t;

const char *kind_name(kind_t k);
size_t kind_size(kind_t k);
bool kind_custom(kind_t k);

#define J_KINDOF(x) _Generic((x), \
    void none, \
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
    default: J_NIL)

typedef struct TypeInfo {
    const char *type;
    bool custom;
    size_t size;
    kind_t kind;
} typeinfo_t;

typeinfo_t typeinfo_init(kind_t kind);

typedef struct AnyValue {
    //kind_t kind;
    union {  // <-- https://www.youtube.com/@TsodingDaily
        void *none; bool boolean; uint8_t bits;
        char ch; signed char sch; unsigned char uch;
        short sh; unsigned short ush; long l;
        int i; unsigned int ui; unsigned long ul; long double ld;
        long long ll; unsigned long long ull;
        float f; double d; long double ldd; float f32; double f64;
        const char *cstr; const void *cptr;
        char *str; void *ptr; void *nil;
        /* fixed width */
        int8_t i8; int16_t i16; int32_t i32; int64_t i64;
        uint8_t u8; uint16_t u16; uint32_t u32; uint64_t u64;
    } as;
} any_v;

typedef struct ValueType {
    typeinfo_t info;
    any_v type;
} value_t;

typedef struct Anything {
    value_t val;
    const void *src;
} any_t;

bool any_set_nil(any_t *any);
bool any_is_nil(const any_t *any);
bool any_init(any_t *any, kind_t k, const void *src);

#define any_import(any, x) \
    any_from_src((any), J_KINDOF(x), &(x))

#endif /* JANY_H */
#ifdef JANY_IMPL

#include <stdbool.h>
#include <string.h>

static const char *kind_names[J_KINDSIZE] = {
    [J_NONE]      = "none",

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
    [J_NIL]       = "nil"
};

static const size_t kind_sizes[J_KINDSIZE] = {
    [J_NONE]      = 0,
    [J_NIL]       = 0,
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
};

const char *kind_name(kind_t k){
    return kind_names[k] && (unsigned)k >= J_KINDSIZE ? \
        kind_names[k] : "! Unknown/Unsupported Kind";
}

size_t kind_size(kind_t k){
    if ((unsigned)k >= J_KINDSIZE) return 0;
    return kind_sizes[k];
}

bool kind_custom(kind_t k){
    return ((unsigned)k > J_CUSTOM);
}

typeinfo_t typeinfo_init(kind_t k){
    typeinfo_t info = {
        .type = kind_name(k),
        .size = kind_size(k),
        .kind = k,
        .custom = kind_custom(k)
    };
    return info;
}

kind_t any_kind(const any_t *any){
    return any ? any->val.info.kind : J_NONE;
}

bool any_is_nil(const any_t *any){
    return any && any->val.info.kind == J_NIL;
}

bool any_set_nil(any_t *any){
    if (!any) return false;
    memset(any, 0, sizeof(*any));
    any->val.info = typeinfo_init(J_NIL);
    any->val.type.as.nil = NULL;
    any->src = NULL;

    return true;
}

bool any_is_none(const any_t *any){
    return any && any->val.info.kind == J_NONE;
}

bool any_set_none(any_t *any)
{
    if (!any) return false;
    memset(any, 0, sizeof(*any));
    any->val.info = typeinfo_init(J_NONE);
    any->val.type.as.none = J_NONE;
    any->src = J_NONE;

    return true;
}

bool any_init(any_t *any, kind_t k, const void *src){
    if (!any || !src || k == J_NONE || k == J_NIL) return false;
    
    size_t size = kind_size(k);
    if (size == 0 || size > sizeof(any->val.type.as)) return false;

    any->val.info = typeinfo_init(k);
    any->src = src;

    memset(&any->val.type, 0, sizeof(any->val.type));
    memcpy(&any->val.type.as, src, size);

    return true;
}





#endif /* JANY_IMPL */