#pragma once
#ifndef JLIST_H
#define JLIST_H

#include "../jstd/jerr.h"

typedef struct Node {
    uintptr_t xord;
    const char *data;
    void *next;
    void *prev;
    int value;
    bool seen;
} node_t;

node_t *new_node(int val, const char *data);

typedef struct List {
    node_t *head;
    node_t *tail;
    char *tmpbuf;
    size_t bufcap;
    int size;
} list_t;

#define ll_add_xor(v) _ll_add_xor(v, #v)

static inline list_t ll_new();
static inline void ll_del(list_t *ll);
static inline void ll_add(list_t *ll, const char *data);
static inline void ll_ins(list_t *ll, int idx, const char *data);
static inline void _ll_add_xor(list_t *ll, int value, const char *name);

typedef struct {
    error_t err;
    list_t list;
    list_t *lists;
} Jlist;

#endif /* JLIST_H */
// #define JLIST_IMPL // #debug mode, normally doesn't exist
#ifdef JLIST_IMPL
/* ! IMPLEMENTATION ! */
#include <stdlib.h>

node_t *new_node(int val, const char *data){
    node_t *node = (node_t*)malloc(sizeof(*node));
    memset(node, 0, sizeof(*node));
    node->value = val;
    // node->seen = false;
    return node;
}

static inline list_t ll_new(){
    list_t ll;
    ll.head->data = "hello, world!\n";
    ll.tail = NULL;
    ll.size++;

    return ll;
}

void ll_ins(list_t *ll, int idx, const char *data){
//     node_t *insert = (node_t*)malloc(sizeof(node_t));
//     if (insert == NULL) return;

//     insert->data = data;
//     insert->next = (*node);
//     (*node) = insert;
    
}

void ll_add(list_t *ll, const char *data){
    if (ll->head == NULL) 
    node_t *node = new_node(1, data);
}

void ll_del(list_t *ll){
    node_t *tmp;
    while (ll->head != NULL){
        tmp = ll->head;
        ll->head = (node_t*)ll->head->next;
        free(tmp);
    }
}

void _ll_add_xor(list_t *ll, int value, const char *name){
    if (ll->head == NULL){
        //printf("[error] jlib: NULL found at end of linked list {%s}!", name);
        ll->tail = new_node(value, name);
        ll->head = ll->tail;
        
    }else{
        node_t *node = new_node(value, name);
        node->xord  = (uintptr_t)ll->tail;
        ll->tail->xord ^= (uintptr_t)node;
        ll->tail = node;
    }
}

#endif /* JLIST_IMPL */