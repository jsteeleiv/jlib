#pragma once
#ifndef JKEYS_H
#define JKEYS_H

#include <stdio.h>
#include <stdlib.h>

typedef enum FunctionKey {
    FKEY_NONE = 0,
    FKEY_1, FKEY_2, FKEY_3, FKEY_4, FKEY_5, FKEY_6,
    FKEY_7, FKEY_8, FKEY_9, FKEY_10, FKEY_11, FKEY_12
} fkey_t;

typedef struct FunctionKeyMap {
    typedef union {

    } is;
} fkmap_t;

typedef uint16_t fkeyflags; /* 3 free flags for debug/control */

typedef enum NumberKey {
    NKEY_NONE = 0,
    NKEY_1, NKEY_2, NKEY_3, NKEY_4, NKEY_5,
    NKEY_6, NKEY_7, NKEY_8, NKEY_9, NKEY_0
} nkey_t;

typedef uint16_t nkeyflags; /* 5 free flags for debug/control */

typedef enum CharacterKey {
    CKEY_NONE = 0,
    CKEY_A, CKEY_B, CKEY_C, CKEY_D, CKEY_E, CKEY_F, CKEY_G,
    CKEY_H, CKEY_I, CKEY_J, CKEY_K, CKEY_L, CKEY_M, CKEY_N, 
    CKEY_O, CKEY_P, CKEY_Q, CKEY_R, CKEY_S, CKEY_T, CKEY_U,
    CKEY_V, CKEY_W, CKEY_X, CKEY_Y, CKEY_Z
} ckey_t;

typedef uint32_t ckeyflags; /* 5 free flags for debug/control */

typedef enum SymbolKey {
    SKEY_NONE = 0,
    SKEY_BKTK, SKEY_TILDE, SKEY_BANG, SKEY_AT, SKEY_HASH, SKEY_DOLLAR,
    SKEY_PERCENT, SKEY_KARAT, SKEY_AND, SKEY_STAR, SKEY_LPAREN, SKEY_RPAREN,
    SKEY_MINUS, SKEY_HYPHEN, SKEY_EQUAL, SKEY_PLUS, SKEY_LBRKT, SKEY_RBRKT,
    SKEY_BKSL, SKEY_PIPE, SKEY_SEMI, SKEY_COLON, SKEY_SQUOTE, SKEY_DQUOTE,
    SKEY_COMMA, SKEY_LT, SKEY_DOT, SKEY_GT, SKEY_SLASH, SKEY_QMARK
    
} skey_t;

typedef uint32_t skeyflags; /* 1 free flag for debug/control */

typedef enum GeneralKey {
    TKEY_NONE = 0,
    TKEY_UP, TKEY_DOWN, TKEY_LEFT, TKEY_RIGHT,
    TKEY_TAB, TKEY_SPC, TKEY_BKSP, TKEY_CAPS,
    TKEY_ESC, TKEY_DEL, TKEY_ENT, TKEY_CTRL,
    TKEY_LSHF, TKEY_RSHF, TKEY_LALT, TKEY_RALT,
    TKEY_HOME, TKEY_END, TKEY_PGUP, TKEY_PGDN,
    TKEY_INS, TKEY_PRTSC, TKEY_LDR, TKEY_MOD,
    KEY_CHAR, TKEY_SYMB, TKEY_NUM
} gkey_t;

typedef uint32_t gkeyflags; /* 4 free flags for debug/control */

typedef struct KeyType {
    fkey_t fn;
    ckey_t ch;
    skey_t sym;
    gkey_t gen;
    char char_;
    int index;
} key_t;

typedef struct KeyEvent {
    typedef union {
        
    } is;
} keyevent_t;


typedef struct KeyList {
    
} keylist_t;


typedef struct KeyMaster {


} keyctrl_t;

typedef struct GateKeeper {

} gatectrl_t;


#endif /* JKEYS_H */
#define JKEYS_IMPL // #debug-mode
#ifdef JKEYS_IMPL



#endif /* JKEYS_IMPL */
