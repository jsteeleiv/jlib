#pragma once
#ifndef JEXC_H
#define JEXC_H

#include "../jtype.h"

#define EXC_ERR_OK     0
#define EXC_ERR_INVAL -1
#define EXC_ERR_RANGE -2
#define EXC_ERR_NULL  -3

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
    EXC_KIND_NONE = 0,
    EXC_KIND_DIV_BY_ZERO,
    EXC_KIND_OVERFLOW,
    EXC_KIND_UNDERFLOW,
    EXC_KIND_INVALID_OPCODE,
    EXC_KIND_PRIV_INSTRUCTION,
    EXC_KIND_ALIGNMENT_FAULT,
    EXC_KIND_PAGE_FAULT,
    EXC_KIND_SEGMENT_FAULT,
    EXC_KIND_STACK_FAULT,
    EXC_KIND_GEN_PROTECTION,
    EXC_KIND_BUS_FAULT,
    EXC_KIND_ADDRESS_ERROR,
    EXC_KIND_BREAKPOINT,
    EXC_KIND_SINGLE_STEP,
    EXC_KIND_DEBUG,
    EXC_KIND_SYSCALL,
    EXC_KIND_MACHINE_CHECK,
    EXC_KIND_DOUBLE_FAULT,
    EXC_KIND_WATCHDOG,
    EXC_KIND_SOFTWARE_TRAP,
    EXC_KIND_ARCH_SPECIFIC,
    EXC_KIND_SIZE
} exckind_t;

typedef enum exceptionActions {
    EXC_ACT_UNHANDLED = 0,
    EXC_ACT_RESUME,
    EXC_ACT_RETRY,
    EXC_ACT_HALT,
    EXC_ACT_PANIC,
    EXC_ACT_DELEGATE
} excacts_t;

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
} excframe_t;

typedef struct ExceptionInfo {
    excfam_t family;
    exckind_t kind;
    excacts_t actions;
    excprops_t props;
    excframe_t frame;
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

typedef struct ExceptionHandler {
    exc_handler_fn fn;
    excinfo_t info;
    void *user;
    void *excpt;
} exchandle_t;

typedef struct ExceptionState {
    exchandle_t handlers[EXC_KIND_SIZE];
    exchandle_t def_handler;
} excstate_t;

static inline int exc_install(exckind_t kind, exc_handler_fn fn, void *usr);
/* Register handler for specific semantic exception kind */
static inline int exc_install_default(exc_handler_fn fn, void *usr);
/* Install handler for raw architectural vector number; instead of semantic kind. */
static inline int exc_install_vector(uint vec, exc_handler_fn fn, void *usr);
static inline int exc_remove_vector(uint vec, exc_handler_fn fn, void *usr);
static inline int exc_remove(exckind_t kind);
static inline int exc_remove_default(void);
static inline void exc_info_init(excinfo_t *info);
static inline uint exc_kind_valid(exckind_t kind);
static inline excfam_t exc_kind_family(exckind_t kind);
static inline const char *exc_kind_name(exckind_t kind);
static inline uint exc_kindis_fault(exckind_t kind);
static inline uint exc_kindis_abort(exckind_t kind);
static inline uint exc_kindis_trap(exckind_t kind);
static inline excacts_t exc_raise(excinfo_t *info);


#endif /* JEXC_H */
#define JEXC_IMPL
#ifdef JEXC_IMPL


static inline uint exc_kind_valid(exckind_t kind){
    return (kind > EXC_KIND_NONE && kind < EXC_KIND_SIZE) ? 1u : 0u;
}




#endif /* JEXC_IMPL */