#pragma once
#ifndef JTYPE_H
#define JTYPE_H

#include "jtype/jbyte.h"
#include "jtype/jfunc.h"
#include "jtype/jfut.h"
#include "jtype/jmisc.h"
#include "jtype/jstr.h"
#include "jtype/jval.h"

typedef unsigned int uint;

// typedef struct {
//     Jerror err;
//     uint32_t id;    // debug tracing
//     value_t val;
//     kind_t cast;
// } Jtype;

typedef struct Type {
    val_t val;
} type_t;



#endif /* JTYPE_H */


#ifdef JTYPE_IMPL



#endif /* JTYPE_IMPL */