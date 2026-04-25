#pragma once
#ifndef JPTR_H
#define JPTR_H

#include "../jtype.h"

/* destructor function pointer */
typedef void (*free_fn)(void*);


typedef struct RawPointer {
    /* T* 
    ~~~~~
    can be null and does not imply ownership by itself.
    does not point to an object && 
        dereferencing it == undefined behavior
    */
   
   
} rawptr_t;

typedef struct UniquePointer {
    void *addr;
    free_fn free;
} uniqptr_t;

static inline uniqptr_t unique_ptr_make(void *ptr, free_fn free);
static inline void unique_ptr_reset(uniqptr_t *p, void *newp);
static inline void thing();

typedef struct VoidPointer {
    void *here;
} voidptr_t;
typedef struct SmartPointer;
typedef struct SharedPointer;

typedef struct PeekPointer; /* weak pointer */

typedef struct ObserverPointer {

} obsptr_t;

/* interop adapters; adapt smart pointers to C-style APIs that expect 'T**'
    let you keep unique_ptr-based ownership while still talking to older C libraries
*/
typedef struct OutPointer {
    /* out_ptr is for output-only setters */
} outptr_t;

typedef struct InOutPointer {
    /* inout_ptr is for APIs that may read/replace the current pointer
        captures reset arguments by reference
        intended to be used as a temporary
        Use with shared_ptr is forbidden for inout_ptr ...
            because that operation requires unique ownership
    */
} inoutptr_t;

typedef struct SharedFromThisPointer {
    /* may turn into a function in the future, or func * ... 
        not a pointer itself, but it is extremely relevant around shared_ptr.
        If a type publicly inherits from it 
            and object is already managed by a shared_ptr,
            calling shared_from_this() gives you another shared_ptr
            that pointer shares ownership with the existing one.
            There is also weak_from_this() since C++17

            solves the ...
                “I’m inside the object and need a shared_ptr<this>” problem safely.
            The unsafe version is constructing a fresh shared_ptr<T>(this),
            which creates a separate control block and can cause double-deletion;
            enable_shared_from_this exists specifically to avoid that pattern.
            The safe behavior comes from sharing ownership with existing shared_ptr
    */

} shrfrmthis_t;

/* referencing/dereferencing */
typedef struct PointerReference {
    /* T&
    ~~~~~
        A reference is a non-null alias
        best parameter type when callee should not own object
            and null is not meaningful
    */

} ptrref_t;

/* newer heap owning wrappers */
typedef struct IndirectPointer {
    /*
        owns a dynamically allocated object and gives a value-like wrapper around it
        manages the lifetime of the owned object and can be valueless only after move
        useful for polymorphic objects
            where you want value behavior instead of manual pointer plumbing
    */
} indirptr_t;

typedef struct PolyMorphicPointer {
    /* 
        a wrapper containing a dynamically allocated object
            of possibly different types with value-like semantics
    */
} polyptr_t;

/*

Use T& when null is not allowed and you do not own.
Use T* when null is meaningful and you do not own.
Use std::unique_ptr<T> for ownership by default.
Use std::shared_ptr<T> only when lifetime is truly shared.
Use std::weak_ptr<T> only alongside shared_ptr, mainly to observe or break cycles.
Use observer_ptr only if your codebase wants an explicit non-owning vocabulary type and accepts the experimental status.
Use out_ptr/inout_ptr only for C API boundaries

*/

#endif /* JPTR_H */
#define JPTR_IMPL
#ifdef JPTR_IMPL

/* unique pointer implementations */
static inline uniqptr_t unique_ptr_make(void *ptr, free_fn free){
    uniqptr_t p;
    p.addr = ptr;
    p.free = free;
    return p;
}

static inline void unique_ptr_reset(uniqptr_t *p, void *newp){
    if (!p) return;
    if (p->addr && p->free) p->free(p->addr);
    p->addr = newp;
}




#endif /* JPTR_IMPL */