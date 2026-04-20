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
    EXC_UNKNOWN = 0,
    EXC_FAULT,
    EXC_TRAP,
    EXC_ABORT,
    EXC_CUSTOM
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

typedef excacts_t (*exc_handle_fn)(excinfo_t *info, void *user);

typedef struct ExceptionHandler {
    exc_handle_fn fn;
    excinfo_t info;
    void *user;
    // void *excpt;
} exchandle_t;

/* State object
~~~~~~~~~~~~~~~
    Keeping state explicit is better than hidden globals because:
      - easier testing
      - multiple instances possible
      - clearer ownership
*/
typedef struct ExceptionState {
    exchandle_t handlers[EXC_KIND_SIZE];
    exchandle_t def_handler;
} excstate_t;

static inline int exc_install(excstate_t *st, exckind_t kind, exc_handle_fn fn, void *usr);
/* Register handler for specific semantic exception kind */
static inline int exc_install_default(excstate_t *st, exc_handle_fn fn, void *usr);
/* Install handler for raw architectural vector number; instead of semantic kind. */
static inline int exc_install_vector(uint vec, exc_handle_fn fn, void *usr);
static inline int exc_remove_vector(uint vec, exc_handle_fn fn, void *usr);
static inline int exc_remove(excstate_t *st, exckind_t kind);
static inline int exc_remove_default(excstate_t *st);
static inline void exc_info_init(excinfo_t *info);
static inline void exc_state_init(excstate_t *st);
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

static inline excfam_t exc_kind_family(exckind_t kind){
    switch (kind) {
        case EXC_KIND_DIV_BY_ZERO:
        case EXC_KIND_OVERFLOW:
        case EXC_KIND_UNDERFLOW:
        case EXC_KIND_INVALID_OPCODE:
        case EXC_KIND_PRIV_INSTRUCTION:
        case EXC_KIND_ALIGNMENT_FAULT:
        case EXC_KIND_PAGE_FAULT:
        case EXC_KIND_SEGMENT_FAULT:
        case EXC_KIND_STACK_FAULT:
        case EXC_KIND_GEN_PROTECTION:
        case EXC_KIND_BUS_FAULT:
        case EXC_KIND_ADDRESS_ERROR:
            return EXC_FAULT;

        case EXC_KIND_BREAKPOINT:
        case EXC_KIND_SINGLE_STEP:
        case EXC_KIND_DEBUG:
        case EXC_KIND_SYSCALL:
        case EXC_KIND_SOFTWARE_TRAP:
            return EXC_TRAP;

        case EXC_KIND_MACHINE_CHECK:
        case EXC_KIND_DOUBLE_FAULT:
        case EXC_KIND_WATCHDOG:
            return EXC_ABORT;

        case EXC_KIND_ARCH_SPECIFIC:
            return EXC_UNKNOWN;

        case EXC_KIND_NONE:
        case EXC_KIND_SIZE:
        default:
            return EXC_CUSTOM;
    }
}

static inline const char *exc_kind_name(exckind_t kind){
    switch (kind) {
        case EXC_KIND_NONE: return "none";
        case EXC_KIND_DIV_BY_ZERO: return "div_by_zero";
        case EXC_KIND_OVERFLOW: return "overflow";
        case EXC_KIND_UNDERFLOW:return "underflow";
        case EXC_KIND_INVALID_OPCODE:return "invalid_opcode";
        case EXC_KIND_PRIV_INSTRUCTION:return "priveleged_instruction";
        case EXC_KIND_ALIGNMENT_FAULT:return "alignment_fault";
        case EXC_KIND_PAGE_FAULT:return "page_fault";
        case EXC_KIND_SEGMENT_FAULT:return "segment_fault";
        case EXC_KIND_STACK_FAULT:return "stack_fault";
        case EXC_KIND_GEN_PROTECTION:return "general_protection";
        case EXC_KIND_BUS_FAULT:return "bus_fault";
        case EXC_KIND_ADDRESS_ERROR:return "address_error";
        case EXC_KIND_BREAKPOINT:return "breakpoint";
        case EXC_KIND_SINGLE_STEP:return "single_step";
        case EXC_KIND_DEBUG:return "debug";
        case EXC_KIND_SYSCALL:return "syscall";
        case EXC_KIND_SOFTWARE_TRAP:return "software_trap";
        case EXC_KIND_MACHINE_CHECK:return "machine_check";
        case EXC_KIND_DOUBLE_FAULT:return "double_fault";
        case EXC_KIND_WATCHDOG:return "watchdog";
        case EXC_KIND_ARCH_SPECIFIC:return "arch_specific";
        case EXC_KIND_SIZE:
        default: return "unknown";
    }
}

static inline uint exc_kindis_fault(exckind_t kind){
    return (excfam_t(kind) == EXC_FAULT) ? 1u : 0u;
}
static inline uint exc_kindis_trap(exckind_t kind){
    return (excfam_t(kind) == EXC_TRAP) ? 1u : 0u;
}

static inline uint exc_kindis_abort(exckind_t kind){
    return (excfam_t(kind) == EXC_ABORT) ? 1u : 0u;
}

static inline void exc_info_init(excinfo_t *info){
    if (!info) return;

    info->kind        = EXC_KIND_NONE;
    info->family      = EXC_UNKNOWN;
    info->vector      = 0u;
    info->arch        = 0u;
    info->rawcode     = 0u;
    info->instr_ptr   = NULL;
    info->fault_addr  = NULL;
    info->errcode     = 0ul;
    info->priv_lvl    = 0ul;
    info->synchronous = 0u;
    info->resumable   = 0u;
    info->precise     = 0u;
    info->fatal       = 0u;
    info->arch_frame  = NULL;
}

static inline void exc_state_init(excstate_t *st){
    if (!st) return;
    for (uint i = 0; i < EXC_KIND_SIZE; ++i){
        st->handlers[i].fn = NULL;
        st->handlers[i].user = NULL;
    }
    st->def_handler.fn = NULL;
    st->def_handler.user = NULL;
    exc_info_init(&st->def_handler.info);
}

static inline int exc_install(excstate_t *st, exckind_t kind, exc_handle_fn fn, void *usr){
    if (!st || !fn) return EXC_ERR_NULL;
    if (!exc_kind_valid(kind)) return EXC_ERR_RANGE;
    st->handlers[kind].fn   = fn;
    st->handlers[kind].user = usr;
    return EXC_ERR_OK;
}

static inline int exc_remove(excstate_t *st, exckind_t kind){
    if (!st) return EXC_ERR_NULL;
    if (!exc_kind_valid(kind)) return EXC_ERR_RANGE;
    st->handlers[kind].fn   = NULL;
    st->handlers[kind].user = NULL;
    return EXC_ERR_OK;
}

static inline int exc_install_default(excstate_t *st, exc_handle_fn fn, void *usr){
    if (!st || !fn) return EXC_ERR_NULL;
    st->def_handler.fn   = fn;
    st->def_handler.user = usr;
    return EXC_ERR_OK;
}

static inline int exc_remove_default(excstate_t *st){
    if (!st) return EXC_ERR_NULL;
    st->def_handler.fn = NULL;
    st->def_handler.user = NULL;
    return EXC_ERR_OK;
}

#endif /* JEXC_IMPL */
