#define JFUNC_IMPL
#include "../../include/jlib/jtype/jfunc.h"

#include <stdio.h>
#include <stdlib.h>

static void print_divider(const char *title){
    printf("\n=== %s ===\n", title);
}

static void explain(const char *text){
    printf("-> %s\n", text);
}

static void run_adder_demo(void){
    print_divider("adder closure");
    explain("We create an owned closure that captures offset = 5.");
    explain("This behaves like: x -> x + 5");

    closure_t add5 = closure_make_adder(5);

    int x = 10;
    int y = 0;

    if (closure_call(&add5, &x, &y) == CLOSURE_OK){
        printf("add5(%d) = %d\n", x, y);
    }

    x = -2;
    y = 0;

    if (closure_call(&add5, &x, &y) == CLOSURE_OK){
        printf("add5(%d) = %d\n", x, y);
    }

    explain("Now we destroy the closure. Because it owns its env, drop will free it.");
    closure_drop(&add5);

    printf("closure_isnull(add5) -> %s\n", closure_isnull(&add5) ? "true" : "false");
}

static void run_multiplier_demo(void){
    print_divider("multiplier closure");
    explain("We create an owned closure that captures factor = 3.");
    explain("This behaves like: x -> x * 3");

    closure_t mul3 = closure_make_multer(3);

    int x = 7;
    int y = 0;

    if (closure_call(&mul3, &x, &y) == CLOSURE_OK){
        printf("mul3(%d) = %d\n", x, y);
    }

    x = -4;
    y = 0;

    if (closure_call(&mul3, &x, &y) == CLOSURE_OK){
        printf("mul3(%d) = %d\n", x, y);
    }

    closure_drop(&mul3);
    printf("closure_isnull(mul3) -> %s\n", closure_isnull(&mul3) ? "true" : "false");
}

static void run_counter_demo(void){
    print_divider("counter closure");
    explain("This closure captures mutable state: current value and step.");
    explain("Each call returns the current value, then advances the internal state.");

    closure_t counter = closure_make_counter(0, 1);
    int y = 0;

    if (closure_call(&counter, NULL, &y) == CLOSURE_OK){
        printf("counter() -> %d\n", y);
    }

    if (closure_call(&counter, NULL, &y) == CLOSURE_OK){
        printf("counter() -> %d\n", y);
    }

    if (closure_call(&counter, NULL, &y) == CLOSURE_OK){
        printf("counter() -> %d\n", y);
    }

    explain("Notice how the closure remembers state between calls.");
    closure_drop(&counter);
}

static void run_borrowed_demo(void){
    print_divider("borrowed environment");
    explain("Here the closure does NOT own its environment.");
    explain("The environment lives outside the closure and remains valid after drop.");

    envadd_t external = { .offset = 42 };
    closure_t add42 = closure_make_brrwd(adder_call, &external);

    int x = 8;
    int y = 0;

    if (closure_call(&add42, &x, &y) == CLOSURE_OK){
        printf("add42(%d) = %d\n", x, y);
    }

    explain("We now mutate the external environment directly.");
    external.offset = 100;

    if (closure_call(&add42, &x, &y) == CLOSURE_OK){
        printf("after external.offset = 100, add42(%d) = %d\n", x, y);
    }

    explain("Dropping a borrowed closure does not free the external env.");
    closure_drop(&add42);

    printf("external.offset is still %d\n", external.offset);
    printf("closure_isnull(add42) -> %s\n", closure_isnull(&add42) ? "true" : "false");
}

static void run_clone_demo(void){
    print_divider("cloning owned closures");
    explain("We clone a counter closure.");
    explain("Because it owns its env, clone performs a deep copy.");
    explain("That means both closures start equal, then evolve independently.");

    closure_t original = closure_make_counter(100, 10);
    closure_t copy = closure_null();

    closure_err err = closure_clone(&original, &copy);
    printf("closure_clone(original, copy) -> %d\n", err);

    if (err == CLOSURE_OK){
        int a = 0;
        int b = 0;

        closure_call(&original, NULL, &a);
        closure_call(&original, NULL, &a);

        closure_call(&copy, NULL, &b);
        closure_call(&copy, NULL, &b);

        printf("original counter values: 100, 110\n");
        printf("copy     counter values: 100, 110\n");
        printf("original.env == copy.env -> %s\n",
            (original.env == copy.env) ? "true" : "false");
    }

    closure_drop(&original);
    closure_drop(&copy);
}

static void run_no_clone_demo(void){
    print_divider("failed clone");
    explain("Now we build an owned closure manually without a clone hook.");
    explain("Owned closures without clone support should reject cloning.");

    envadd_t *env = (envadd_t *)malloc(sizeof(*env));
    if (!env){
        printf("allocation failed; skipping this demo\n");
        return;
    }

    env->offset = 77;

    closure_t src = closure_make(
        adder_call,
        env,
        adder_envkill,
        NULL,
        CLSR_ENV_OWNED,
        CLSR_FLG_NONE
    );

    closure_t dst = closure_null();
    closure_err err = closure_clone(&src, &dst);

    printf("closure_clone(src, dst) -> %d\n", err);
    printf("expected CLSR_ERR_NOCLONE -> %d\n", CLSR_ERR_NOCLONE);
    printf("closure_isnull(dst) -> %s\n", closure_isnull(&dst) ? "true" : "false");

    closure_drop(&src);
    closure_drop(&dst);
}

static void run_nokill_demo(void){
    print_divider("NOKILL flag");
    explain("This shows that ownership mode and destruction policy can be separated.");
    explain("The closure is marked as owned, but the NOKILL flag prevents auto-free on drop.");

    envadd_t *env = (envadd_t *)malloc(sizeof(*env));
    if (!env){
        printf("allocation failed; skipping this demo\n");
        return;
    }

    env->offset = 123;

    closure_t c = closure_make(
        adder_call,
        env,
        adder_envkill,
        adder_envclone,
        CLSR_ENV_OWNED,
        CLSR_FLG_NOKILL
    );

    int x = 1;
    int y = 0;

    if (closure_call(&c, &x, &y) == CLOSURE_OK){
        printf("NOKILL closure result: %d\n", y);
    }

    closure_drop(&c);

    printf("closure_isnull(c) -> %s\n", closure_isnull(&c) ? "true" : "false");
    explain("Because NOKILL was set, we must free the env manually.");
    free(env);
}

int main(void){
    printf("========================================\n");
    printf(" jfunc showcase\n");
    printf(" raw closures in C using void * + env\n");
    printf("========================================\n");

    explain("A closure here is: function pointer + captured environment + lifecycle hooks.");

    run_adder_demo();
    run_multiplier_demo();
    run_counter_demo();
    run_borrowed_demo();
    run_clone_demo();
    run_no_clone_demo();
    run_nokill_demo();

    print_divider("done");
    explain("This demo exercised owned envs, borrowed envs, mutation, cloning, and cleanup.");

    return 0;
}
