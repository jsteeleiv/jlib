#pragma once
#ifndef JSET_H
#define JSET_H

#include "../jtype.h"

#ifndef SET_CAP
#define SET_CAP 8
#endif

typedef struct Set {
    val_t *data;
    size_t size;
    size_t cap;
} set_t;

/* lifecycle */
static inline set_t *set_mk(void);
static inline void set_die(set_t *set);

/* basic operations */
static inline bool set_ins(set_t *set, val_t val);
static inline bool set_del(set_t *set, val_t val);
static inline bool set_rsz(set_t *set, size_t cap);
static inline bool set_has(const set_t *set, val_t val);
static inline void set_print(const set_t *set);
static inline size_t set_size(const set_t *set);

/* set algebra */
static inline set_t *set_diff(const set_t *a, const set_t *b);
static inline set_t *set_union(const set_t *a, const set_t *b);
static inline set_t *set_intersect(const set_t *a, const set_t *b);

#endif /* JSET_H */
#define JSET_IMPL // #debug-mode
#ifdef JSET_IMPL


static inline set_t *set_mk(void){
    set_t *s = (set_t*) malloc(sizeof(set_t));
    if (!s) return NULL;
    s->data = (val_t*) malloc(SET_CAP * sizeof(val_t));
    if (!s->data){
        free(s);
        return NULL;
    }
    s->size = 0;
    s->cap = SET_CAP;
    return s;
}

static inline void set_die(set_t *s){
    if (!s) return;
    free(s->data);
    free(s);
}

static inline bool set_ins(set_t *s, val_t val){
    if (!s) return false;
    if (set_has(s, val)) return false;
    if (s->size == s->cap){
        if (!set_rsz(s, s->cap * 2))return false;
    }
    s->data[s->size++] = val;
    return true;
}

static inline bool set_del(set_t *s, val_t val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i].type.any.src == val.type.any.src){
            s->data[i] = s->data[s->size - 1];
            s->size--;
            return true;
        }
    }
    return false;
}

static inline bool set_rsz(set_t *s, size_t cap){
    val_t *val = (val_t*) realloc(s->data, cap * sizeof(val_t));
    if (!val) return false;
    s->data = val;
    s->cap = cap;
    return true;
}

static inline bool set_has(set_t *s, val_t val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i].type.any.src == val.type.any.src) return true;
    }
    return false;
}

static inline void set_print(const set_t *s){
    if (s == NULL){
        printf("{ }\n");
        return;
    }

    printf("{ ");
    for (size_t i = 0; i < s->size; i++) {
        printf("%d", s->data[i]);
        if (i + 1 < s->size) {
            printf(", ");
        }
    }
    printf(" }\n");
}

static inline size_t set_size(set_t *s){
    return s ? s->size : 0;
}

static inline set_t *set_diff(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_has(b, a->data[i])) set_ins(c, a->data[i]);
    }
    return c;
}

static inline set_t *set_union(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_ins(c, a->data[i])) {
            /* duplicate is fine, allocation failure is harder to distinguish here */
        }
    }
    for (size_t i = 0; i < b->size; i++) {
        if (!set_ins(c, b->data[i])) {
            /* duplicate is fine */
        }
    }
    return c;
}

static inline set_t *set_intersect(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++) {
        if (set_has(b, a->data[i])) set_ins(c, a->data[i]);
    }
    return c;
}

#endif /* JSET_IMPL */