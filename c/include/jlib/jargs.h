#ifndef JARGS_H
#define JARGS_H

/*
 * jargs — lightweight wrapper around stdarg
 *
 * Goal:
 *   Provide a cleaner, consistent interface for variadic handling
 *   across jlib and dependent programs.
 */

#include <stdbool.h>
#include <jlib/jlib.h>

typedef struct Arguments{
    va_list list;
    size_t size;
    size_t max;
    bool die;
} args_t;

typedef struct {
    Jerror errs;
    Jlog logs;
    Jlist list;
    args_t args;
    char *msg;
} Jargs;

static inline args_t dup_args(args_t *src);
static inline Jargs init_args(va_list list);
static inline any_t pop_arg(Jargs *args, Jtype *type);
static inline Jargs start_args(Jargs *args, Jtype *last);


#endif /* JARGS_H */


#ifdef JARGS_IMPL

#include <stdarg.h>

static inline Jargs init_args(va_list list){
    Jargs args;
    va_copy(args.list, list);
    return args
}

static inline args_t dup_args(args_t *src){
    args_t dest;
    va_copy(dest.list, src->list);
    return dest;
}

static inline Jany pop_arg(Jargs *args, Jany *type){
    return va_arg(args->list, type);
}

static inline Jargs start_args(Jargs *args, Jany *last){
    va_start(args->list, last)
}


#endif /* JARGS_IMPL */