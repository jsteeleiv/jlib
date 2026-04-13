#pragma once
#ifndef JVAL_H
#define JVAL_H

#include <stdbool.h>
#include "jdef.h"

typedef struct ValueType {
    typedef_t type;
    any_v cast;
} value_t;

bool val_setcast(value_t *val, kind_t k);
bool val_castas(value_t *val, kind_t k);

#endif /* JVAL_H */
#ifdef JVAL_IMPL

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

#endif /* JVAL_IMPL */