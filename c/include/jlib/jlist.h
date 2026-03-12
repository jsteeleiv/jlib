#ifndef JLIST_H
#define JLIST_H

#include <stdlib.h>
#include <jlib/jlogs.h>

typedef struct Node {
    uintptr_t xord;
    void *next;
    void *prev;
    int value;
    const char *data;
    bool seen;
    

} node;

typedef struct LinkedList {
    Node *head;
    Node *tail;
    int size;
    const char *data;
    Error err;

} list;

#define ll_add_xor(v) _ll_add_xor(v, #v)

LinkedList new_ll();
Node *new_node(int val, const char *data);
void ll_del(LinkedList *ll);
void ll_add(LinkedList *ll, const char *data);
void ll_ins(LinkedList *ll, int idx, const char *data);
void _ll_add_xor(LinkedList *ll, int value, const char *name);


#ifdef JLIST_IMPL

Node *new_node(int val, const char *data){
    Node *node = (Node*)malloc(sizeof(*node));
    memset(node, 0, sizeof(*node));
    node->value = val;
    // node->seen = false;
    return node;
}

LinkedList new_ll(){
    LinkedList ll;
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

// void ll_ins(LinkedList *ll, int idx, const char *data){
//     Node *insert = (Node*)malloc(sizeof(Node));
//     if (insert == NULL) return;

//     insert->data = data;
//     insert->next = (*node);
//     (*node) = insert;
    
// }

void ll_add(LinkedList *ll, const char *data){
    if (ll->head == NULL) 
    Node *node = new_node(1, data);
}

void ll_del(LinkedList *ll){
    Node *tmp;
    while (ll->head != NULL){
        tmp = ll->head;
        ll->head = (Node*)ll->head->next;
        free(tmp);
    }
}

void _ll_add_xor(LinkedList *ll, int value, const char *name){
    if (ll->head == NULL){
        //printf("[error] jlib: NULL found at end of linked list {%s}!", name);
        ll->tail = new_node(value, name);
        ll->head = ll->tail;
        
    }else{
        Node *node = new_node(value, name);
        node->xord  = (uintptr_t)ll->tail;
        ll->tail->xord ^= (uintptr_t)node;
        ll->tail = node;
    }
}

#endif /* JLIST_IMPL */



#endif /* JLIST_H */