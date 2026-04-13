#pragma once
#ifndef JFUNC_H
#define JFUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum ClosureEnvironmentMode {
    CLSR_ENV_NONE = 0,
    CLSR_ENV_BRRWD,
    CLSR_ENV_OWNED,
    CLSR_ENV_CUSTOM = 3,
    /* Custom envs to consider */
    CLSR_ENV_COUNT = 16
} closure_env;

typedef enum ClosureErrorMode {
    CLOSURE_OK = 0,
    CLSR_ERR_NULL,
    CLSR_ERR_ARG,
    CLSR_ERR_ENV,
    CLSR_ERR_CALL,
    CLSR_ERR_OUT,
    CLSR_ERR_ALLOC,
    CLSR_ERR_NOCLONE,
    CLSR_ERR_CUSTOM = 8,
    /* Custom errors to consider */
    CLSR_ERR_COUNT = 16
} closure_err;

typedef enum ClosureFlag {
    CLSR_FLG_NONE     = 0,
    CLSR_FLG_CONSTENV = 1 << 0,
    CLSR_FLG_NOCLONE  = 1 << 1,
    CLSR_FLG_NOKILL   = 1 << 2,
    CLSR_FLG_TRIVIAL  = 1 << 3
} closure_flg;

// typedef struct ClosureStat {
//     closure_env mode;
//     closure_flg flag;
//     closure_err err;
// } closure_st;
// typedef closure_st (*closure_fn)(void *env, void *src, void *dst);

// typedef struct ClosureFunction {
//     /* reurns a call target + bound state. */
//     closure_st stat;
//     closure_env env;
// } closure_fn;
typedef void (*closure_fn)(void *env, void *src, void *dst);

// typedef struct ClosureOperation {
//     closure_fn call;
//     env_kill kill;
//     env_clone clone;
// } closure_op;

/* kill function for environment
--------------------------------
    if your environment was heap allocated
    or contains nested allocations

    Example values:
    free
    a custom adder_env_kill
    NULL if no destruction needed
*/
typedef void (*env_kill)(void *env);

/* clone function for environment
---------------------------------
    produce a logically equivalent copy
    return a new environment pointer
*/
typedef void *(*env_clone)(const void *env);

/* compare function
-------------------
    compare captured states
*/
typedef int (*env_compare)(const void *lhs, const void *rhs);

/* debug/inspection function
----------------------------
    printable closures
*/
typedef void (*env_dump)(const void *env, FILE *stream);



typedef struct Closure {
    closure_env mode;
    closure_fn func;
    //closure_op oper;
    env_compare cmp;
    env_clone clone;
    uint32_t flags;
    env_kill kill;
    env_dump dump;
    size_t arity;
    size_t envsz;
    void *env;
} closure_t;

/* Calling it becomes:
    int result = closure.fn(closure.env, 42);
*/

static inline closure_t closure_null(void);
static inline void closure_drop(closure_t *c);
static inline closure_t closure_make(closure_fn fn, void *env,
    env_kill kill, env_clone clone, closure_env mode, unsigned int flags
);
static inline closure_t closure_make_brrwd(closure_fn fn, void *env);
static inline closure_t closure_make_owned(
    closure_fn fn, void *env, env_kill kill, env_clone clone
);
static inline int closure_isnull(const closure_t *c);
static inline int closure_hasenv(const closure_t *c);
static inline int closure_ownenv(const closure_t *c);
static inline closure_err closure_call(const closure_t *c, void *arg, void *dst);

/* usage:
---------
    closure_t b;
    closure_clone(&a, &b);
*/
static inline closure_err closure_clone(const closure_t *src, closure_t *dst);

typedef struct EnvironmentAdder {
    int offset; /* pass by value */
} envadd_t;

static void adder_envkill(void *env);
static void *adder_envclone(const void *env);
static inline closure_t closure_make_adder(int offset);
static void adder_call(void *env, void *arg, void *dst);

typedef struct EnvrionmentMultiplier {
    int factor; /* pass by value */
} envmul_t;

static void multer_envkill(void *env);
static void *multer_envclone(const void *env);
static inline closure_t closure_make_multer(int factor);
static void multer_call(void *env, void *arg, void *dst);


typedef struct EnvironmentCounter {
    int curr;
    int step;
} envcount_t;

static void counter_call(void *env, void *arg, void *dst);
static void counter_envkill(void *env);
static void *counter_envclone(const void *env);
static inline closure_t closure_make_counter(int start, int step);

#endif /* JFUNC_H */
#define JFUNC_IMPL // #debug-mode
#ifdef JFUNC_IMPL

/* Example Usage
----------------
    closure_t add5 = closure_make_adder(5);

    int x = 10;
    int y = 0;

    if (closure_call(&add5, &x, &y) == CLOSURE_OK) {
        printf("%d\n", y);
    }

    closure_drop(&add5);
*/


static inline closure_t closure_null(void){
    closure_t c;
    c.func = NULL;
    c.env = NULL;
    c.kill = NULL;
    c.clone = NULL;
    c.mode = CLSR_ENV_NONE;
    c.flags = CLSR_FLG_NONE;
    return c;
}

static inline closure_t closure_make(closure_fn fn, void *env,
    env_kill kill, env_clone clone, closure_env mode, unsigned int flags){

    closure_t c;
    c.func = fn;
    c.env = env;
    c.kill = kill;
    c.clone = clone;
    c.mode = mode;
    c.flags = flags;
    return c;
}

static inline closure_t closure_make_brrwd(closure_fn fn, void *env){
    return closure_make(
        fn, env, NULL, NULL, CLSR_ENV_BRRWD, CLSR_FLG_NONE
    );
}

static inline closure_t closure_make_owned(closure_fn fn, void *env,
    env_kill kill, env_clone clone){

    return closure_make(
        fn, env, kill, clone, CLSR_ENV_OWNED, CLSR_FLG_NONE
    );
}

static inline int closure_isnull(const closure_t *c){
    return (c == NULL) || (c->func == NULL);
}

static inline int closure_hasenv(const closure_t *c){
    return (c != NULL) && (c->env != NULL);
}

static inline int closure_ownenv(const closure_t *c){
    return (c != NULL) && (c->mode == CLSR_ENV_OWNED);
}

static inline closure_err closure_call(const closure_t *c, void *arg, void *dst){
    if (c == NULL) return CLSR_ERR_NULL;
    if (c->func == NULL) return CLSR_ERR_CALL;
    if (dst == NULL) return CLSR_ERR_OUT;
    c->func(c->env, arg, dst);
    return CLOSURE_OK;
}

static inline void closure_drop(closure_t *c){
    if (c == NULL) return;
    if ((c->mode == CLSR_ENV_OWNED) &&
        (c->env != NULL) &&
        (c->kill != NULL) &&
        !(c->flags & CLSR_FLG_NOKILL)) {
        c->kill(c->env);
    }
    *c = closure_null();
}

static inline closure_err closure_clone(const closure_t *src, closure_t *dst){

    if (src == NULL || dst == NULL) return CLSR_ERR_NULL;
    if (src->func == NULL) {
        *dst = closure_null();
        return CLSR_ERR_CALL;
    }
    *dst = *src;
    // dst->func.stat.err = CLOSURE_OK;
    if (src->mode == CLSR_ENV_OWNED) {
        if (src->env == NULL) {
            dst->env = NULL;
            return CLOSURE_OK;
        }

        if (src->clone == NULL || (src->flags & CLSR_FLG_NOCLONE)) {
            *dst = closure_null();
            return CLSR_ERR_NOCLONE;
        }

        dst->env = src->clone(src->env);
        if (dst->env == NULL) {
            *dst = closure_null();
            return CLSR_ERR_ALLOC;
        }
    }
    return CLOSURE_OK;
}

/* Explanation
--------------
    cast env back to envadd_t *
    cast arg to int *
    cast dst to int *
    compute result
*/
static inline closure_t closure_make_adder(int offset){envadd_t *env = (envadd_t *)malloc(sizeof(*env));
    if (env == NULL) return closure_null();
    env->offset = offset;
    return closure_make_owned(adder_call, env, adder_envkill, adder_envclone);
}

static void adder_call(void *env, void *arg, void *dst){
    envadd_t *e = (envadd_t *)env;
    int *x = (int *)arg;
    int *result = (int *)dst;

    *result = *x + e->offset;
}

static void adder_envkill(void *env){free(env);}

static void *adder_envclone(const void *env){
    const envadd_t *src = (const envadd_t *)env;
    envadd_t *dst = (envadd_t *)malloc(sizeof(*dst));
    if (dst == NULL) return NULL;

    *dst = *src;
    return dst;
}

static void multer_call(void *env, void *arg, void *dst){
    envmul_t *e = (envmul_t *)env;
    int *x = (int *)arg;
    int *res = (int *)dst;
    *res = (*x) * e->factor;
}

static void multer_envkill(void *env){free(env);}

static void *multer_envclone(const void *env){
    const envmul_t *src = (const envmul_t *)env;
    envmul_t *dst = (envmul_t *)malloc(sizeof(*dst));
    if (dst == NULL) return NULL;
    *dst = *src;
    return dst;
}

static inline closure_t closure_make_multer(int factor){
    envmul_t *env = (envmul_t *)malloc(sizeof(*env));
    if (env == NULL) return closure_null();
    env->factor = factor;
    return closure_make_owned(multer_call, env, multer_envkill, multer_envclone);
}

static void counter_call(void *env, void *arg, void *dst){
    (void)arg;
    envcount_t *e = (envcount_t *)env;
    int *result = (int *)dst;
    *result = e->curr;
    e->curr += e->step;
}

static void counter_envkill(void *env){free(env);}

static void *counter_envclone(const void *env){
    const envcount_t *src = (const envcount_t *)env;
    envcount_t *dst = (envcount_t *)malloc(sizeof(*dst));
    if (dst == NULL) return NULL;
    *dst = *src;
    return dst;
}

static inline closure_t closure_make_counter(int start, int step){
    envcount_t *env = (envcount_t *)malloc(sizeof(*env));
    if (env == NULL) return closure_null();
    env->curr = start;
    env->step = step;
    return closure_make_owned(counter_call, env, counter_envkill, counter_envclone);
}

#endif /* JFUNC_IMPL */
