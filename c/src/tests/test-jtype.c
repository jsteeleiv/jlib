#define JTYPE_IMPL
#include "../../include/jlib/jdata/jtype.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

static int tests_run = 0;
static int tests_failed = 0;

static void check_bool(const char *label, bool expr){
    tests_run++;

    if (expr){
        printf("[PASS] %s\n", label);
    } else {
        printf("[FAIL] %s\n", label);
        tests_failed++;
    }
}

static void check_size(const char *label, size_t got, size_t want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %zu\n", label, got);
    } else {
        printf("[FAIL] %s -> got=%zu want=%zu\n", label, got, want);
        tests_failed++;
    }
}

static void check_kind(const char *label, kind_t got, kind_t want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %d\n", label, (int)got);
    } else {
        printf("[FAIL] %s -> got=%d want=%d\n", label, (int)got, (int)want);
        tests_failed++;
    }
}

static void check_str(const char *label, const char *got, const char *want){
    tests_run++;

    if (got && want && strcmp(got, want) == 0){
        printf("[PASS] %s -> \"%s\"\n", label, got);
        return;
    }

    if (!got && !want){
        printf("[PASS] %s -> NULL\n", label);
        return;
    }

    printf("[FAIL] %s -> got=%s want=%s\n",
        label,
        got ? got : "NULL",
        want ? want : "NULL");
    tests_failed++;
}

int main(void){
    printf("=== jtype smoke test ===\n\n");

    printf("--- validity ---\n");
    check_bool("J_NONE is valid", kind_valid(J_NONE));
    check_bool("J_INT is valid", kind_valid(J_INT));
    check_bool("J_CUSTOM is valid", kind_valid(J_CUSTOM));
    check_bool("J_VOID is valid", kind_valid(J_VOID));
    check_bool("J_KINDSIZE is invalid", !kind_valid(J_KINDSIZE));
    check_bool("negative kind is invalid", !kind_valid((kind_t)-1));

    printf("\n--- names ---\n");
    check_str("kind_name(J_NONE)", kind_name(J_NONE), "none");
    check_str("kind_name(J_BOOL)", kind_name(J_BOOL), "bool");
    check_str("kind_name(J_INT)", kind_name(J_INT), "int");
    check_str("kind_name(J_U64)", kind_name(J_U64), "u64");
    check_str("kind_name(J_CSTR)", kind_name(J_CSTR), "const char *");
    check_str("kind_name(J_PTR)", kind_name(J_PTR), "void *");
    check_str("kind_name(J_NULL)", kind_name(J_NULL), "null");
    check_bool("invalid kind_name returns error string",
        strcmp(kind_name((kind_t)-1), "! Unsupported Kind") == 0);

    printf("\n--- sizes ---\n");
    check_size("kind_size(J_NONE)", kind_size(J_NONE), 0);
    check_size("kind_size(J_BOOL)", kind_size(J_BOOL), sizeof(bool));
    check_size("kind_size(J_CHAR)", kind_size(J_CHAR), sizeof(char));
    check_size("kind_size(J_SHORT)", kind_size(J_SHORT), sizeof(short));
    check_size("kind_size(J_INT)", kind_size(J_INT), sizeof(int));
    check_size("kind_size(J_LONG)", kind_size(J_LONG), sizeof(long));
    check_size("kind_size(J_LLONG)", kind_size(J_LLONG), sizeof(long long));
    check_size("kind_size(J_FLOAT)", kind_size(J_FLOAT), sizeof(float));
    check_size("kind_size(J_DOUBLE)", kind_size(J_DOUBLE), sizeof(double));
    check_size("kind_size(J_LDOUBLE)", kind_size(J_LDOUBLE), sizeof(long double));

    check_size("kind_size(J_I8)", kind_size(J_I8), sizeof(int8_t));
    check_size("kind_size(J_I16)", kind_size(J_I16), sizeof(int16_t));
    check_size("kind_size(J_I32)", kind_size(J_I32), sizeof(int32_t));
    check_size("kind_size(J_I64)", kind_size(J_I64), sizeof(int64_t));

    check_size("kind_size(J_U8)", kind_size(J_U8), sizeof(uint8_t));
    check_size("kind_size(J_U16)", kind_size(J_U16), sizeof(uint16_t));
    check_size("kind_size(J_U32)", kind_size(J_U32), sizeof(uint32_t));
    check_size("kind_size(J_U64)", kind_size(J_U64), sizeof(uint64_t));

    check_size("kind_size(J_F32)", kind_size(J_F32), sizeof(float));
    check_size("kind_size(J_F64)", kind_size(J_F64), sizeof(double));

    check_size("kind_size(J_CSTR)", kind_size(J_CSTR), sizeof(const char *));
    check_size("kind_size(J_STR)", kind_size(J_STR), sizeof(char *));
    check_size("kind_size(J_PTR)", kind_size(J_PTR), sizeof(void *));
    check_size("kind_size(J_CONST_PTR)", kind_size(J_CONST_PTR), sizeof(const void *));
    check_size("kind_size(J_CUSTOM)", kind_size(J_CUSTOM), 0);
    check_size("kind_size(J_MAP)", kind_size(J_MAP), 0);
    check_size("kind_size(J_VOID)", kind_size(J_VOID), 0);
    check_size("kind_size(invalid)", kind_size((kind_t)-1), 0);

    printf("\n--- custom classification ---\n");
    check_bool("J_NONE is not custom", !kind_custom(J_NONE));
    check_bool("J_INT is not custom", !kind_custom(J_INT));
    check_bool("J_CUSTOM is not custom boundary", !kind_custom(J_CUSTOM));
    check_bool("J_ANSI is custom", kind_custom(J_ANSI));
    check_bool("J_MAP is custom", kind_custom(J_MAP));
    check_bool("J_VOID is custom", kind_custom(J_VOID));

    printf("\n--- scalar classification ---\n");
    check_bool("J_SHORT is scalar", kind_scalar(J_SHORT));
    check_bool("J_INT is scalar", kind_scalar(J_INT));
    check_bool("J_U64 is scalar", kind_scalar(J_U64));
    check_bool("J_F64 is scalar", kind_scalar(J_F64));

    check_bool("J_BOOL is not scalar", !kind_scalar(J_BOOL));
    check_bool("J_CHAR is not scalar", !kind_scalar(J_CHAR));
    check_bool("J_CSTR is not scalar", !kind_scalar(J_CSTR));
    check_bool("J_PTR is not scalar", !kind_scalar(J_PTR));
    check_bool("J_MAP is not scalar", !kind_scalar(J_MAP));

    printf("\n--- typeinfo_init ---\n");
    {
        typeinfo_t info = typeinfo_init(J_INT);
        check_str("typeinfo_init(J_INT).type", info.type, "int");
        check_size("typeinfo_init(J_INT).size", info.size, sizeof(int));
        check_kind("typeinfo_init(J_INT).kind", info.kind, J_INT);
        check_bool("typeinfo_init(J_INT).custom=false", !info.custom);
    }

    {
        typeinfo_t info = typeinfo_init(J_MAP);
        check_str("typeinfo_init(J_MAP).type fallback string",
            info.type,
            "! Be more `kind` 💀 : Let's Rewind 📼 ! ");
        check_size("typeinfo_init(J_MAP).size", info.size, 0);
        check_kind("typeinfo_init(J_MAP).kind", info.kind, J_MAP);
        check_bool("typeinfo_init(J_MAP).custom=true", info.custom);
    }

    {
        typeinfo_t info = typeinfo_init((kind_t)-1);
        check_str("typeinfo_init(invalid).type", info.type, "! Unsupported Kind");
        check_size("typeinfo_init(invalid).size", info.size, 0);
        check_kind("typeinfo_init(invalid).kind preserved", info.kind, (kind_t)-1);
        check_bool("typeinfo_init(invalid).custom=false", !info.custom);
    }

    printf("\n=== summary ===\n");
    printf("tests ran: %d\n", tests_run);
    printf("!failures: %d\n", tests_failed);

    return (tests_failed == 0) ? 0 : 1;
}