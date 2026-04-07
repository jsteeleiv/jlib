#pragma once
#ifndef JTUPL_H
#define JTUPL_H

#include <jany.h>
#include <jstd/jerr.h>

typedef struct Tuple {
    value_t  *items;
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



#endif /* JTUPL_H */
#define JTUPL_IMPL
#ifdef JTUPL_IMPL


static inline tuple_t tpl_init(any_t *items, size_t size){

}
static inline error_t tpl_rsrv(tuple_t *tpl, size_t cap);
static inline const any_t *tpl_view(tuple_t *tpl, size_t idx);
static inline const any_t tpl_pull(tuple_t *tpl, size_t idx);
static inline error_t tpl_push(tuple_t *tpl, any_t val);
static inline void tpl_clear(tuple_t *tpl);
static inline void tpl_free(tuple_t *tpl);


#endif /* JTUPL_IMPL */