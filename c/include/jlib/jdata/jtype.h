#pragma once
#ifndef JTYPE_H
#define JTYPE_H


#include <jlib/jstd/jtime.h>
#include <jlib/jstd/jerr.h>
#include <jlib/jdata/jany.h>

typedef struct Tuple {
    any_t  *items;
    size_t  size;
    size_t  cap;
} tuple_t;

#define TPL_DEF_SZ  4
#define TPL_MAX_SZ  8

static inline tuple_t tpl_init(any_t *items, size_t size);
static inline error_t tpl_rsrv(tuple_t *tpl, size_t cap);
static inline const any_t *tpl_view(tuple_t *tpl, size_t idx);
static inline const any_t tpl_pull(tuple_t *tpl, size_t idx);
static inline error_t tpl_push(tuple_t *tpl, any_t val);
static inline void tpl_clear(tuple_t *tpl);
static inline void tpl_free(tuple_t *tpl);


/* custom return type to support multiple return values */
typedef struct ReturnValues {
    tuple_t tuple;
    size_t index;
    uint8_t flags;
    error_t err;
} return_t;

typedef struct {
    Jerror err;
    uint32_t id;    // debug tracing
    any_t any;
    kind_t as;
} Jtype;

#define JTYPE_FLAG_OWNS_DATA     (1u << 0)
#define JTYPE_FLAG_CONST         (1u << 1)
#define JTYPE_FLAG_ERROR         (1u << 2)
#define JTYPE_FLAG_LOGGED        (1u << 3)
#define JTYPE_FLAG_FINALIZED     (1u << 4)

bool jtype_cast(Jtype *jt, kind_t k);

typedef struct Bit {
    uint8_t byte;
    unsigned int val : 1; // occupies 1 bit
} bit_t; // 2 bytes, even with the single bit `val`

typedef struct Information {
    error_t error;       // custom error type
    const char *mod;
    const char *ctx;
    vec_t *data;
} info_t;

typedef struct Random{
    Jtime time;
    void *abyss;
    int seed;
    float fval;
    int ival;
} rand_t;

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


static inline tuple_t tpl_init(any_t *items, size_t size){

}
static inline error_t tpl_rsrv(tuple_t *tpl, size_t cap);
static inline const any_t *tpl_view(tuple_t *tpl, size_t idx);
static inline const any_t tpl_pull(tuple_t *tpl, size_t idx);
static inline error_t tpl_push(tuple_t *tpl, any_t val);
static inline void tpl_clear(tuple_t *tpl);
static inline void tpl_free(tuple_t *tpl);


#endif /* JTYPE_IMPL */