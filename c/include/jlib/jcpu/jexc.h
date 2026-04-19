#pragma once
#ifndef JEXC_H
#define JEXC_H

#include "../jtype.h"

/* the semantic family */
typedef enum ExceptionFamily {
    EXC_FAULT,
    EXC_TRAP,
    EXC_ABORT,
    EXC_DIV0,
    EXC_MYN,
} excfam_t;

/* the actual exception */
typedef enum ExceptionKind {
    EXC_NONE = 0,
    EXC_DIV_BY_ZERO,
    EXC_OVERFLOW,
    EXC_UNDERFLOW,
    EXC_INVALID_OPCODE,
    EXC_PRIV_INSTRUCTION,
    EXC_ALIGNMENT_FAULT,
    EXC_PAGE_FAULT,
    EXC_SEGMENT_FAULT,
    EXC_STACK_FAULT,
    EXC_GEN_PROTECTION,
    EXC_BUS_FAULT,
    EXC_ADDRESS_ERROR,
    EXC_BREAKPOINT,
    EXC_SINGLE_STEP,
    EXC_DEBUG,
    EXC_SYSCALL,
    EXC_MACHINE_CHECK,
    EXC_DOUBLE_FAULT,
    EXC_WATCHDOG,
    EXC_SOFTWARE_TRAP,
    EXC_ARCH_SPECIFIC
} exckind_t;

/* helpful properties */
typedef struct ExceptionProperties {
    uint synchronous  : 1;
    uint resumable    : 1;
    uint precise      : 1;
    uint contributes_error_code : 1;
    uint contributes_fault_addr : 1;
} excprops_t;

typedef struct ExceptionFrame {
    uint has_ip   : 1;
    uint has_sp   : 1;
    uint has_flag : 1;
    uint has_err  : 1;
    uint has_addr : 1;
    uint has_priv : 1;
} escframe_t;

typedef enum exceptionActions {
    EXC_ACT_UNHANDLED = 0,
    EXC_ACT_RESUME,
    EXC_ACT_RETRY,
    EXC_ACT_HALT,
    EXC_ACT_PANIC,
    EXC_ACT_DELEGATE
} excacts_t;



typedef struct ExceptionInfo {
    escframe_t exc_frame;
    excfam_t family;
    exckind_t kind;
    void *instr_ptr;
    void *fault_addr;
    uint long errcode;
    uint long priv_lvl;
    const void *arch_frame;
    uint arch;
    uint vector;
    uint rawcode;
    uint fatal       : 1;
    uint precise     : 1;
    uint resumable   : 1;
    uint synchronous : 1;
} excinfo_t;

typedef excacts_t (*exc_handler_fn)(excinfo_t *info, void *user);

static inline int exc_install(exckind_t k, exc_handler_fn fn, void *usr);
static inline int exc_install_vector(uint vec, exc_handler_fn fn, void *usr);
static inline int exc_remove(exckind_t k);
static inline excfam_t exc_kind_fam(exckind_t k);
static inline const char *exc_kind_name(exckind_t k);
static inline uint exc_kindis_fault(exckind_t k);
static inline uint exc_kindis_abort(exckind_t k);
static inline uint exc_kindis_trap(exckind_t k);

static inline excacts_t exc_raise(excinfo_t *info);


#endif /* JEXC_H */

#ifdef JEXC_IMPL



#endif /* JEXC_IMPL */