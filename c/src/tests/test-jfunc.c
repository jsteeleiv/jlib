#define JFUNC_IMPL
#include "../../include/jlib/jtype/jfunc.h"

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

static void check_int(const char *label, int got, int want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %d\n", label, got);
    } else {
        printf("[FAIL] %s -> got=%d want=%d\n", label, got, want);
        tests_failed++;
    }
}

static void check_ptr(const char *label, const void *got, const void *want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %p\n", label, got);
    } else {
        printf("[FAIL] %s -> got=%p want=%p\n", label, got, want);
        tests_failed++;
    }
}

static void check_err(const char *label, closure_err got, closure_err want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %d\n", label, (int)got);
    } else {
        printf("[FAIL] %s -> got=%d want=%d\n", label, (int)got, (int)want);
        tests_failed++;
    }
}

static void check_envmode(const char *label, closure_env got, closure_env want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %d\n", label, (int)got);
    } else {
        printf("[FAIL] %s -> got=%d want=%d\n", label, (int)got, (int)want);
        tests_failed++;
    }
}

int main(void){
    printf("=== jfunc smoke test ===\n\n");

    printf("--- null safety ---\n");
    check_bool("closure_isnull(NULL) == true", closure_isnull(NULL));
    check_bool("closure_hasenv(NULL) == false", !closure_hasenv(NULL));
    check_bool("closure_ownenv(NULL) == false", !closure_ownenv(NULL));
    check_err("closure_call(NULL, NULL, &x) == CLSR_ERR_NULL",
        closure_call(NULL, NULL, &(int){0}),
        CLSR_ERR_NULL);
    check_err("closure_clone(NULL, &dst) == CLSR_ERR_NULL",
        closure_clone(NULL, &(closure_t){0}),
        CLSR_ERR_NULL);
    closure_drop(NULL);
    check_bool("closure_drop(NULL) does not crash", true);

    printf("\n--- closure_null ---\n");
    {
        closure_t c = closure_null();

        check_bool("closure_isnull(&c)", closure_isnull(&c));
        check_bool("closure_hasenv(&c) == false", !closure_hasenv(&c));
        check_bool("closure_ownenv(&c) == false", !closure_ownenv(&c));
        check_ptr("func == NULL after closure_null", (const void *)c.func, NULL);
        check_ptr("env == NULL after closure_null", c.env, NULL);
        check_ptr("kill == NULL after closure_null", (const void *)c.kill, NULL);
        check_ptr("clone == NULL after closure_null", (const void *)c.clone, NULL);
        check_envmode("mode after closure_null", c.mode, CLSR_ENV_NONE);
        check_int("flags after closure_null", (int)c.flags, CLSR_FLG_NONE);
        check_err("closure_call on null closure", closure_call(&c, NULL, &(int){0}), CLSR_ERR_CALL);
    }

    printf("\n--- make borrowed ---\n");
    {
        envadd_t env = { .offset = 42 };
        closure_t c = closure_make_brrwd(adder_call, &env);
        int x = 8;
        int y = 0;

        check_bool("borrowed closure is not null", !closure_isnull(&c));
        check_bool("borrowed closure has env", closure_hasenv(&c));
        check_bool("borrowed closure ownenv == false", !closure_ownenv(&c));
        check_envmode("borrowed mode", c.mode, CLSR_ENV_BRRWD);
        check_ptr("borrowed env pointer preserved", c.env, &env);
        check_ptr("borrowed kill == NULL", (const void *)c.kill, NULL);
        check_ptr("borrowed clone == NULL", (const void *)c.clone, NULL);

        check_err("closure_call borrowed adder", closure_call(&c, &x, &y), CLOSURE_OK);
        check_int("borrowed adder result", y, 50);

        closure_drop(&c);
        check_bool("closure_drop resets borrowed closure to null", closure_isnull(&c));
        check_int("borrowed env still valid after drop", env.offset, 42);
    }

    printf("\n--- make owned generic ---\n");
    {
        envadd_t *env = (envadd_t *)malloc(sizeof(*env));
        closure_t c;
        int x = 10;
        int y = 0;

        if (!env){
            printf("[FAIL] malloc for owned generic env\n");
            tests_run++;
            tests_failed++;
        } else {
            env->offset = 5;
            c = closure_make_owned(adder_call, env, adder_envkill, adder_envclone);

            check_bool("owned generic closure is not null", !closure_isnull(&c));
            check_bool("owned generic closure has env", closure_hasenv(&c));
            check_bool("owned generic closure ownenv == true", closure_ownenv(&c));
            check_envmode("owned generic mode", c.mode, CLSR_ENV_OWNED);
            check_ptr("owned generic env pointer preserved", c.env, env);
            check_ptr("owned generic kill set", (const void *)c.kill, (const void *)adder_envkill);
            check_ptr("owned generic clone set", (const void *)c.clone, (const void *)adder_envclone);

            check_err("closure_call owned generic", closure_call(&c, &x, &y), CLOSURE_OK);
            check_int("owned generic adder result", y, 15);

            closure_drop(&c);
            check_bool("closure_drop resets owned closure to null", closure_isnull(&c));
        }
    }

    printf("\n--- call failures ---\n");
    {
        closure_t c = closure_make_adder(3);
        check_err("closure_call rejects NULL dst", closure_call(&c, &(int){7}, NULL), CLSR_ERR_OUT);
        closure_drop(&c);
    }

    printf("\n--- adder factory ---\n");
    {
        closure_t add5 = closure_make_adder(5);
        int x = 10;
        int y = 0;

        check_bool("closure_make_adder returns non-null closure", !closure_isnull(&add5));
        check_bool("adder has env", closure_hasenv(&add5));
        check_bool("adder owns env", closure_ownenv(&add5));
        check_envmode("adder mode", add5.mode, CLSR_ENV_OWNED);

        check_err("closure_call add5", closure_call(&add5, &x, &y), CLOSURE_OK);
        check_int("add5(10) == 15", y, 15);

        x = -2;
        y = 0;
        check_err("closure_call add5 with negative input", closure_call(&add5, &x, &y), CLOSURE_OK);
        check_int("add5(-2) == 3", y, 3);

        closure_drop(&add5);
        check_bool("adder becomes null after drop", closure_isnull(&add5));
    }

    printf("\n--- multer factory ---\n");
    {
        closure_t mul3 = closure_make_multer(3);
        int x = 7;
        int y = 0;

        check_bool("closure_make_multer returns non-null closure", !closure_isnull(&mul3));
        check_bool("multer has env", closure_hasenv(&mul3));
        check_bool("multer owns env", closure_ownenv(&mul3));
        check_envmode("multer mode", mul3.mode, CLSR_ENV_OWNED);

        check_err("closure_call mul3", closure_call(&mul3, &x, &y), CLOSURE_OK);
        check_int("mul3(7) == 21", y, 21);

        x = -4;
        y = 0;
        check_err("closure_call mul3 with negative input", closure_call(&mul3, &x, &y), CLOSURE_OK);
        check_int("mul3(-4) == -12", y, -12);

        closure_drop(&mul3);
        check_bool("multer becomes null after drop", closure_isnull(&mul3));
    }

    printf("\n--- counter factory ---\n");
    {
        closure_t counter = closure_make_counter(0, 1);
        int y = 0;

        check_bool("closure_make_counter returns non-null closure", !closure_isnull(&counter));
        check_bool("counter has env", closure_hasenv(&counter));
        check_bool("counter owns env", closure_ownenv(&counter));
        check_envmode("counter mode", counter.mode, CLSR_ENV_OWNED);

        check_err("counter first call", closure_call(&counter, NULL, &y), CLOSURE_OK);
        check_int("counter first result", y, 0);

        check_err("counter second call", closure_call(&counter, NULL, &y), CLOSURE_OK);
        check_int("counter second result", y, 1);

        check_err("counter third call", closure_call(&counter, NULL, &y), CLOSURE_OK);
        check_int("counter third result", y, 2);

        closure_drop(&counter);
        check_bool("counter becomes null after drop", closure_isnull(&counter));
    }

    printf("\n--- clone owned ---\n");
    {
        closure_t src = closure_make_counter(100, 10);
        closure_t dst = closure_null();
        int a = 0;
        int b = 0;

        check_err("closure_clone owned counter", closure_clone(&src, &dst), CLOSURE_OK);
        check_bool("cloned dst is not null", !closure_isnull(&dst));
        check_bool("owned clone gets separate env", src.env != dst.env);

        check_err("src first call", closure_call(&src, NULL, &a), CLOSURE_OK);
        check_int("src first result", a, 100);

        check_err("src second call", closure_call(&src, NULL, &a), CLOSURE_OK);
        check_int("src second result", a, 110);

        check_err("dst first call", closure_call(&dst, NULL, &b), CLOSURE_OK);
        check_int("dst first result", b, 100);

        check_err("dst second call", closure_call(&dst, NULL, &b), CLOSURE_OK);
        check_int("dst second result", b, 110);

        closure_drop(&src);
        closure_drop(&dst);
    }

    printf("\n--- clone borrowed ---\n");
    {
        envadd_t env = { .offset = 9 };
        closure_t src = closure_make_brrwd(adder_call, &env);
        closure_t dst = closure_null();
        int x = 1;
        int y = 0;

        check_err("closure_clone borrowed adder", closure_clone(&src, &dst), CLOSURE_OK);
        check_bool("borrowed clone shares env pointer", src.env == dst.env);

        check_err("dst call before env mutation", closure_call(&dst, &x, &y), CLOSURE_OK);
        check_int("borrowed clone result before mutation", y, 10);

        env.offset = 20;

        check_err("dst call after env mutation", closure_call(&dst, &x, &y), CLOSURE_OK);
        check_int("borrowed clone result after mutation", y, 21);

        closure_drop(&src);
        closure_drop(&dst);
    }

    printf("\n--- clone failures ---\n");
    {
        closure_t dst = closure_null();
        closure_t nullc = closure_null();

        check_err("closure_clone rejects NULL dst", closure_clone(&nullc, NULL), CLSR_ERR_NULL);
        check_err("closure_clone rejects null closure func", closure_clone(&nullc, &dst), CLSR_ERR_CALL);
        check_bool("dst reset to null after bad clone", closure_isnull(&dst));
    }

    printf("\n--- no clone owned ---\n");
    {
        envadd_t *env = (envadd_t *)malloc(sizeof(*env));
        closure_t src;
        closure_t dst = closure_null();

        if (!env){
            printf("[FAIL] malloc for no-clone env\n");
            tests_run++;
            tests_failed++;
        } else {
            env->offset = 77;

            src = closure_make(
                adder_call,
                env,
                adder_envkill,
                NULL,
                CLSR_ENV_OWNED,
                CLSR_FLG_NONE
            );

            check_err("owned closure without clone hook rejects clone",
                closure_clone(&src, &dst),
                CLSR_ERR_NOCLONE);

            check_bool("dst is null after failed no-clone", closure_isnull(&dst));

            closure_drop(&src);
            closure_drop(&dst);
        }
    }

    printf("\n--- no kill flag ---\n");
    {
        envadd_t *env = (envadd_t *)malloc(sizeof(*env));
        closure_t c;

        if (!env){
            printf("[FAIL] malloc for no-kill env\n");
            tests_run++;
            tests_failed++;
        } else {
            env->offset = 123;

            c = closure_make(
                adder_call,
                env,
                adder_envkill,
                adder_envclone,
                CLSR_ENV_OWNED,
                CLSR_FLG_NOKILL
            );

            check_bool("NOKILL closure is owned", closure_ownenv(&c));
            closure_drop(&c);
            check_bool("NOKILL closure becomes null after drop", closure_isnull(&c));

            free(env);
            check_bool("manual free after NOKILL path", true);
        }
    }

    printf("\n=== summary ===\n");
    printf("tests run: %d\n", tests_run);
    printf("!failures: %d\n", tests_failed);

    return (tests_failed == 0) ? 0 : 1;
}
