#define JANY_IMPL
#include "../../include/jlib/jtype/jany.h"
#include "../../include/jlib/jtype/jval.h"

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
    printf("=== jany smoke test ===\n\n");

    printf("--- null safety ---\n");
    check_kind("any_kind(NULL) == J_NULL", any_kind(NULL), J_NULL);
    check_bool("any_isnull(NULL) == false", !any_isnull(NULL));
    //check_bool("any_isnone(NULL) == false", !any_isnone(NULL));
    check_bool("any_setnull(NULL) == false", !any_setnull(NULL));
    //check_bool("any_setnone(NULL) == false", !any_setnone(NULL));
    check_bool("any_init(NULL, J_INT, &x) == false", !any_init(NULL, J_INT, &(int){42}));

    printf("\n--- set null ---\n");
    {
        any_t a;
        check_bool("any_setnull(&a)", any_setnull(&a));
        check_kind("kind after setnull", any_kind(&a), J_NULL);
        check_bool("any_isnull(&a)", any_isnull(&a));
        //check_bool("any_isnone(&a) == false", !any_isnone(&a));
        check_kind("info.kind after setnull", a.info.kind, J_NULL);
        check_size("info.size after setnull", a.info.size, 0);
        check_bool("src == NULL after setnull", a.src == NULL);
        check_bool("as.nil == NULL after setnull", a.type.as.null == NULL);
    }

    // printf("\n--- set none ---\n");
    // {
    //     any_t a;
    //     check_bool("any_setnone(&a)", any_setnone(&a));
    //     check_kind("kind after set_none", any_kind(&a), J_NULL);
    //     check_bool("any_isnone(&a)", any_isnone(&a));
    //     check_bool("any_isnull(&a) == false", !any_isnull(&a));
    //     check_kind("info.kind after set_none", a.info.kind, J_NULL);
    //     check_size("info.size after set_none", a.info.size, 0);
    //     check_bool("src == NULL after set_none", a.src == NULL);
    //     check_bool("as.none == NULL after set_none", a.type.as.null == NULL);
    // }

    printf("\n--- init failures ---\n");
    {
        any_t a;
        int x = 42;

        check_bool("any_init rejects NULL any", !any_init(NULL, J_INT, &x));
        check_bool("any_init rejects NULL src", !any_init(&a, J_INT, NULL));
        check_bool("any_init rejects J_NULL", !any_init(&a, J_NULL, &x));
        check_bool("any_init rejects J_NULL", !any_init(&a, J_NULL, &x));
        check_bool("any_init rejects zero-size custom kind", !any_init(&a, J_MAP, &x));
    }

    printf("\n--- init scalar int ---\n");
    {
        any_t a;
        int x = 42;

        check_bool("any_init int", any_init(&a, J_INT, &x));
        check_kind("kind after int init", any_kind(&a), J_INT);
        check_kind("info.kind after int init", a.info.kind, J_INT);
        check_size("info.size after int init", a.info.size, sizeof(int));
        check_bool("src points to x", a.src == &x);
        check_bool("copied int value == 42", a.type.as.i == 42);
        check_bool("not null after int init", !any_isnull(&a));
        //check_bool("not none after int init", !any_isnone(&a));
    }

    printf("\n--- init bool ---\n");
    {
        any_t a;
        bool b = true;

        check_bool("any_init bool", any_init(&a, J_BOOL, &b));
        check_kind("kind after bool init", any_kind(&a), J_BOOL);
        check_bool("copied bool value == true", a.type.as.boolean == true);
    }

    printf("\n--- init double ---\n");
    {
        any_t a;
        double d = 3.25;

        check_bool("any_init double", any_init(&a, J_DOUBLE, &d));
        check_kind("kind after double init", any_kind(&a), J_DOUBLE);
        check_bool("copied double value == 3.25", a.type.as.d == 3.25);
    }

    printf("\n--- init c string pointer ---\n");
    {
        any_t a;
        const char *s = "zorander";

        check_bool("any_init cstr", any_init(&a, J_CSTR, &s));
        check_kind("kind after cstr init", any_kind(&a), J_CSTR);
        check_str("copied cstr pointer target", a.type.as.cstr, "zorander");
        check_bool("src points to source pointer variable", a.src == &s);
    }

    printf("\n--- import macro ---\n");
    {
        any_t a;
        int n = 99;
        const char *msg = "zed";

        check_bool("any_import int", any_import(&a, n));
        check_kind("J_KINDOF(int) -> J_INT", any_kind(&a), J_INT);
        check_bool("imported int value == 99", a.type.as.i == 99);

        check_bool("any_import const char *", any_import(&a, msg));
        check_kind("J_KINDOF(const char *) -> J_CSTR", any_kind(&a), J_CSTR);
        check_str("imported cstr value", a.type.as.cstr, "zed");
    }

    printf("\n--- value_t cast marker ---\n");
    {
        value_t v;
        memset(&v, 0, sizeof(v));

        check_bool("val_setcast(&v, J_I32)", val_setcast(&v, J_I32));
        check_kind("value cast kind set to J_I32", v.cast.kind, J_I32);
        check_bool("val_setcast rejects invalid kind",
            !val_setcast(&v, (kind_t)-1));
    }

    printf("\n=== summary ===\n");
    printf("tests run: %d\n", tests_run);
    printf("!failures: %d\n", tests_failed);

    return (tests_failed == 0) ? 0 : 1;
}
