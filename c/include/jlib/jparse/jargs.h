#pragma once
#ifndef JARGS_H
#define JARGS_H

/*
 * jargs — lightweight wrapper around stdargs variadic arguments
 *
 * Goal:
 *   Provide a cleaner, consistent interface for variadic handling
 *   across jlibs and dependent libraries.
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <jdata/jvec.h>
#include <jstd/jerr.h>
#include <jdata/jlist.h>

typedef struct VariadicArguments{
    any_t *items;
    size_t size;
    size_t cap;
    bool die;
    bool init;
} vargs_t;

typedef struct {
    Jerror errs;
    Jlog logs;
    vargs_t *args;
    char *msg;
} Jarg;

static inline vargs_t vargs_init(va_list list);
static inline vargs_t vargs_dup(vargs_t *src);
static inline any_v vargs_pop(vargs_t *vargs, kind_t kind);

void _args(const char *init_arg, ...);

#endif /* JARGS_H */

#define JARGS_IMPL
#ifdef JARGS_IMPL

#include <stdarg.h>

static inline vargs_t vargs_init(va_list list){
    vargs_t vargs = {0};
    va_copy(vargs.list, list);
    return vargs;
}

static inline vargs_t vargs_dup(vargs_t *src){
    vargs_t dest;
    va_copy(dest.list, src->list);
    return dest;
}

static inline any_v vargs_pop(vargs_t *vargs, kind_t kind){
    any_v out = {0};
    out.kind = kind;
    switch (kind) {
        case BOOL:
            out.as.boolean = (bool)va_arg(vargs->list, int);
            break;
        case I8:
            out.as.i8 = (int8_t)va_arg(vargs->list, int);
            break;
        case I16:
            out.as.i16 = (int16_t)va_arg(vargs->list, int);
            break;
        case I32:
            out.as.i32 = (int32_t)va_arg(vargs->list, int);
            break;
        case I64:
            out.as.i64 = (int64_t)va_arg(vargs->list, long long);
            break;
        case U8:
            out.as.u8 = (uint8_t)va_arg(vargs->list, unsigned int);
            break;
        case U16:
            out.as.u16 = (uint16_t)va_arg(vargs->list, unsigned int);
            break;
        case U32:
            out.as.u32 = (uint32_t)va_arg(vargs->list, unsigned int);
            break;
        case U64:
            out.as.u64 = (uint64_t)va_arg(vargs->list, unsigned long long);
            break;
        case F32:
            out.as.f32 = (float)va_arg(vargs->list, double);
            break;
        case F64:
            out.as.f64 = va_arg(vargs->list, double);
            break;
        case CSTR:
            out.as.cstr = va_arg(vargs->list, const char *);
            break;
        case STR:
            out.as.str = va_arg(vargs->list, char *);
            break;
        case CONST_PTR:
            out.as.cptr = va_arg(vargs->list, const void *);
            break;
        case PTR:
            out.as.ptr = va_arg(vargs->list, void *);
            break;
        case NULL_T:
            out.as.nil = NULL;
            break;
        default:
            out.kind = OPAQUE_T;
            out.as.ptr = va_arg(vargs->list, void *);
            break;
    }
    return out;
}



#endif /* JARGS_IMPL */