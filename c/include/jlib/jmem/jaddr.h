#pragma once
#ifndef JADDR_H
#define JADDR_H

/* JADDR.H
----------
    Addressing primitives and foundation of JMEM system

    Responsibilities:
        - address types
        - offset trypes
        - address ranges
        - alignment helpers
        - block/page logic
        - utilities

    not intended to include anything in jlib,
        but rather included BY them. (jmem.h mostly)
*/

#include <stdlib.h>
#include <stdbool.h>

#include "../jtype.h"

typedef uint64_t addr_t;
typedef int64_t addrdiff_t;
typedef size_t offset_t;

typedef struct AddressRange {
    addr_t base;
    addr_t size;
} addrrange_t;

/* basic arithmetic 
-------------------
    addr_add() and addr_sub():
        let you move an address forward or backward by a signed amount.
    
    addr_t a = 0x1000;
    addr_t b = addr_add(a, 0x20);   0x1020 
    addr_t c =addr_add(a, -0x10);   0x0ff0
    ----
    addr_diff() gives the distance between two addresses.
    
    addrdiff_t d = addr_diff(0x1200, 0x1000); 0x200 
*/
static inline addrdiff_t addr_diff(addr_t a, addr_t b);
static inline addr_t addr_add(addr_t addr, addrdiff_t off);
static inline addr_t addr_sub(addr_t addr, addrdiff_t off);

/* alignment */
static inline bool addr_aligned(addr_t addr, size_t align);
static inline addr_t addr_alignup(addr_t addr, size_t align);
static inline addr_t addr_aligndown(addr_t addr, size_t align);

/* range helpeers */
static inline bool addr_rangehas(addr_t addr, addrrange_t range);
static inline bool addr_rangelaps(addrrange_t a, addrrange_t b);
static inline addr_t addr_rangetail(addrrange_t range);

/* page helpers */
static inline size_t addr_page_index(addr_t addr, size_t pgsz);
static inline size_t addr_page_offset(addr_t addr, size_t pgsz);

/* block helpers */
static inline size_t addr_block_index(addr_t addr, size_t blksz);
static inline size_t addr_block_index(addr_t addr, size_t blksz);


#endif /* JADDR_H */
#define JADDR_IMPL // #debug-mode
#ifdef JADDR_IMPL

/* basic arithmetic */
static inline addrdiff_t addr_diff(addr_t a, addr_t b){
    return (addrdiff_t)(a - b);
}

static inline addr_t addr_add(addr_t addr, addrdiff_t off){
    if (off >= 0 ) return addr + (addr_t)off;
    return addr - (addr_t)(-off);
}

static inline addr_t addr_sub(addr_t addr, addrdiff_t off){
    if (off >= 0 ) return addr - (addr_t)off;
    return addr + (addr_t)(-off);
}

/* alignment */
static inline bool addr_aligned(addr_t addr, size_t align){
    if (align == 0) return false;
    return (addr % (addr_t)align) == 0;
}

static inline addr_t addr_alignup(addr_t addr, size_t align){
    if (align == 0) return addr;
    addr_t mask = (addr_t)(align - 1);
    return (addr + mask) & ~mask;
}

static inline addr_t addr_aligndown(addr_t addr, size_t align){
    if (align == 0) return addr;
    addr_t mask = (addr_t)(align - 1);
    return addr & ~mask;
}

/* range helpeers */
static inline addr_t addr_rangeend(addrrange_t range){
    return range.base + (addr_t)range.size;
}

static inline bool addr_rangehas(addr_t addr, addrrange_t range){
    addr_t tail = addr_rangetail(range);
    return (addr >= range.base) && (addr < tail);
}

static inline bool addr_rangelaps(addrrange_t a, addrrange_t b){
    addr_t atail = addr_rangeend(a);
    addr_t btail = addr_rangeend(b);
    return (a.base < btail) && (b.base < atail);
}

/* page helpers */
static inline size_t addr_page_index(addr_t addr, size_t pgsz){
    if (pgsz == 0) return 0;
    return (size_t)(addr / (addr_t)pgsz);
}

static inline size_t addr_page_offset(addr_t addr, size_t pgsz){
    if (pgsz == 0) return 0;
    return (size_t)(addr / (addr_t)pgsz);    
}

/* block helpers */
static inline size_t addr_block_index(addr_t addr, size_t blksz){
    if (blksz == 0) return 0;
    return (size_t)(addr / (addr_t)blksz);
}

static inline size_t addr_block_index(addr_t addr, size_t blksz){
    if (blksz == 0) return 0;
    return (size_t)(addr / (addr_t)blksz); 
}




#endif /* JADDR_IMPL */
