#pragma once
#ifndef JSTK_H
#define JSTK_H

#include <stddef.h>
#include <stdbool.h>

#include "../jtype.h"

typedef struct Item {
    val_t val;
    void *abyss;
    size_t size;
    size_t cap;
    char *repr;
} item_t;

typedef struct Stack{
    item_t *items;
    void *abyss;
    size_t size;
    size_t cap;
} stack_t;

typedef struct {
    info_t info;
    stack_t stack;
} Jstack;

#define STACK_MAX 16

stack_t *mk_stack(void);
stack_t init_stack(Jstack *s);
void rm_stack(Jstack *s);
void free_stack(Jstack *s);
static inline error_t rsz_stack(Jstack *s);

/* operations */
bool push(stack_t *s, void *item);
val_t pull(stack_t *s);
val_t peek(stack_t *s);



#endif /* JSTK_H */


#ifdef JSTK_IMPL

static inline Jerror* rsz_stack(Jstack *s, size_t cap, Jerror *errs){
    void **new = realloc(s->items, cap * sizeof(void*));
    if (!new){
        SET_E(errs, "warning: could not reallocate new stack ... ", WARN, LOG_MEM, MEM_NULL, 0, false);
        return errs;
    }
    s->items = new;
    s->max = cap;
}

Jerror* init_stack(Jstack *s, Jerror *e){
    if (!s){
        SET_E(e, "warning: no stack provided ... ", WARN, LOG_ARGS, NULL_ARG, 0, false);
        return e;
    }

    s->items = malloc(STACK_MAX * sizeof(void *));
    if (!s->items){
        SET_E(e, "warning: unable to initialize stack ... ", WARN, LOG_MEM, MEM_NULL, 0, false);
        return e;
    }
}


#endif /* JSTK_IMPL */