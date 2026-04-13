#pragma once
#ifndef JDEF_H
#define JDEF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "jany.h"

#define JTYPE_FLAG_OWNS_DATA     (1u << 0)
#define JTYPE_FLAG_CONST         (1u << 1)
#define JTYPE_FLAG_ERROR         (1u << 2)
#define JTYPE_FLAG_LOGGED        (1u << 3)
#define JTYPE_FLAG_FINALIZED     (1u << 4)

typedef struct TypeInfo {
    const char *type;
    kind_t kind;
    size_t size;
    bool jtype;
} typeinfo_t;

typeinfo_t typeinfo_init(kind_t kind);

typedef struct TypeDefinition {
    typeinfo_t info;
    any_t any;
    any_v cast;
}typedef_t;


#endif /* JDEF_H */
#define JDEF_IMPL // for compiler assisted debugging shoudl be "off"
#ifdef JDEF_IMPL



typeinfo_t typeinfo_init(kind_t k){
    typeinfo_t info = {
        .type = kind_name(k),
        .size = kind_size(k),
        .kind = k,
        .jtype = kind_custom(k)
    };
    return info;
}
#endif /* JDEF_IMPL */
