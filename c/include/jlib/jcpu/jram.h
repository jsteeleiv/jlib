#pragma once
#ifndef JRAM_H
#define JRAM_H

#include <stdlib.h>

typedef uint64_t addr_t;

typedef struct AddressRange {
    addr_t start;
    addr_t size;
} addrrange_t;


// addr_add()
// addr_sub()
// addr_align_up()
// addr_align_down()
// addr_page_index()
// addr_page_offset()
// addr_in_range()



#endif /* JRAM_H */
// #define JRAM_IMPL // #debug-mode
#ifdef JRAM_IMPL



#endif /* JRAM_IMPL */