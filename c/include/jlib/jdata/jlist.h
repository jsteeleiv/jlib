#pragma once
#ifndef JLIST_H
#define JLIST_H

#include <jdata/jtype.h>
#include <jcore/jerr.h>


typedef struct Node {
    uintptr_t xord;     //
    const char *data;
    void *next;
    void *prev;
    int value;
    bool seen;
} node_t;

typedef struct List {
    const char *data;
    node_t *head;
    node_t *tail;
    int size;
} list_t;

typedef struct {
    Jerror errs;
    list_t *list;
    // list_t *lists;
} Jlist;

#define ll_add_xor(v) _ll_add_xor(v, #v)

Jlist new_ll();
node_t *new_node(int val, const char *data);
void ll_del(Jlist *ll);
void ll_add(Jlist *ll, const char *data);
void ll_ins(Jlist *ll, int idx, const char *data);
void _ll_add_xor(Jlist *ll, int value, const char *name);

/* ! IMPLEMENTATION ! */
#ifdef JLIST_IMPL
#include <stdlib.h>

node_t *new_node(int val, const char *data){
    node_t *node = (node_t*)malloc(sizeof(*node));
    memset(node, 0, sizeof(*node));
    node->value = val;
    // node->seen = false;
    return node;
}

Jlist new_ll(){
    Jlist ll;
    //char timestr[64];
    ll.head = NULL;
    ll.tail = NULL;
    ll.data = "hello, world\n";
    ll.size = 0;
    ll.err.systime = getnow();
    //ll.err.time = 
    ll.err.msg; // = "list created: ";
    return ll;
}

void ll_ins(Jlist *ll, int idx, const char *data){
//     node_t *insert = (node_t*)malloc(sizeof(node_t));
//     if (insert == NULL) return;

//     insert->data = data;
//     insert->next = (*node);
//     (*node) = insert;
    
}

void ll_add(Jlist *ll, const char *data){
    if (ll->head == NULL) 
    node_t *node = new_node(1, data);
}

void ll_del(Jlist *ll){
    node_t *tmp;
    while (ll->head != NULL){
        tmp = ll->head;
        ll->head = (node_t*)ll->head->next;
        free(tmp);
    }
}

void _ll_add_xor(Jlist *ll, int value, const char *name){
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



#endif /* JLIST_H */