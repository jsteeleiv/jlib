#ifndef JSTK_H
#define JSTK_H

#include <jlib/jlib.h>

typedef struct Stack{
   void **items;
   size_t size;
   size_t max;
} stack_t;

typedef struct {
    Jerror jerr;
    info_t info;
    stack_t stack;
} Jstack;

#define STACK_MAX 16

Jstack *mk_stack(void);
Jerror init_stack(Jstack *s);
void rm_stack(Jstack *s);
void free_stack(Jstack *s);
static inline Jerror rsz_stack(Jstack *s);

/* operations */
Jerror push(stack_t *s, void *item);
Jerror pull(stack_t *s);
Jerror peek(stack_t *s);



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