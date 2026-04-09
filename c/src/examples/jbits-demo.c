#include "../../include/jlib/jtype/jbits.h"
#include <stdbool.h>
/*
 ~ =========================================================================
 + Jbits Demo { cc -Wall -Wextra -std=c17 jbits-demo.c -o demos/jbits-demo }
 ~ =========================================================================
*/

int main(void) {
    printf("========================================\n");
    printf("1. BIT PACKING\n");
    printf("========================================\n");
    {
        uint8_t type = 5;   /* 101 */
        uint8_t level = 3;  /* 011 */
        uint8_t mode = 2;   /* 10  */

        uint8_t packed = pack_byte(type, level, mode);

        printf("packed byte = 0x%02X  ", packed);
        bits_print(packed);
        printf("\n");

        printf("unpack_type()  = %u\n", unpack_type(packed));
        printf("unpack_level() = %u\n", unpack_level(packed));
        printf("unpack_mode()  = %u\n", unpack_mode(packed));
    }

    printf("\n========================================\n");
    printf("2. FLAGS\n");
    printf("========================================\n");
    {
        uint8_t flags = 0;

        flag_set(&flags, FLAG_READ);
        flag_set(&flags, FLAG_WRITE);
        print_flags(flags);

        printf("\nToggle WRITE\n");
        flag_tgl(&flags, FLAG_WRITE);
        print_flags(flags);

        printf("\nSet EXEC and HIDDEN\n");
        flag_set(&flags, FLAG_EXEC);
        flag_set(&flags, FLAG_HIDDEN);
        print_flags(flags);

        printf("\nClear READ\n");
        flag_del(&flags, FLAG_READ);
        print_flags(flags);
    }

    printf("\n========================================\n");
    printf("3. CUSTOM PROTOCOL HEADER\n");
    printf("========================================\n");
    {
        uint16_t header = build_header(
            6,   /* version */
            17,  /* type */
            42,  /* length */
            1    /* urgent */
        );

        printf("header = 0x%04X  ", header);
        word_print(header);
        printf("\n");

        printf("version = %u\n", header_vrs(header));
        printf("type    = %u\n", header_type(header));
        printf("length  = %u\n", header_len(header));
        printf("urgent  = %u\n", header_urg(header));
    }

    printf("\n========================================\n");
    printf("4. ENDIANNESS\n");
    printf("========================================\n");
    {
        uint16_t x16 = 0x1234;
        uint32_t x32 = 0x12345678;

        printf("machine endianness: %s\n",
               endian_isbig() ? "little-endian" : "big-endian");

        printf("\nRaw bytes of uint16_t 0x%04X:\n", x16);
        byte_print(&x16, sizeof(x16));

        printf("\nRaw bytes of uint32_t 0x%08X:\n", x32);
        byte_print(&x32, sizeof(x32));

        printf("\nswap16(0x%04X) = 0x%04X\n", x16, swap16(x16));
        printf("swap32(0x%08X) = 0x%08X\n", x32, swap32(x32));
    }

    printf("\n========================================\n");
    printf("5. SHIFTING WINDOWS OF BITS\n");
    printf("========================================\n");
    {
        uint8_t value = 0xB6; /* 10110110 */
        show_window(value, 3);
    }

    printf("\n========================================\n");
    printf("6. MASKING\n");
    printf("========================================\n");
    {
        uint8_t value = 0b10110110;

        printf("VALUE:    ");
        bitmask_print(value);

        mask_t m = mask_range(2, 3);

        printf("MASK:     ");
        bitmask_print(m.val);

        uint8_t applied = mask_apply(value, m);
        printf("AND(&):   ");
        bitmask_print(applied);

        uint8_t cleared = mask_clear(value, m);
        printf("CLEARED:  ");
        bitmask_print(cleared);

        uint8_t toggled = mask_toggle(value, m);
        printf("TOGGLED:  ");
        bitmask_print(toggled);
    }

    printf("\n========================================\n");
    printf("7. FIELD EDITOR\n");
    printf("========================================\n");
    {
        byte_t b = { .val = 0b10110110 };
        byte_show("initial ", b.val);

        demo_set(b.val, 2, 3, 5);
        demo_set(b.val, 0, 2, 1);
        demo_set(b.val, 5, 3, 7);

        printf("\n--- manual extraction examples ---\n");
        printf("bits 2..4 = %u\n", field_get(b.val, 2, 3));
        printf("bits 0..1 = %u\n", field_get(b.val, 0, 2));
        printf("bits 5..7 = %u\n", field_get(b.val, 5, 3));
    }

    printf("\n========================================\n");
    printf("8. SCAN FOR BITS\n");
    printf("========================================\n");
    {
        uint8_t value = 0xB6;   /* 10110110 */
        uint8_t pattern = 0b101;
        int width = 3;
        bit_scan(value, pattern, width);
    }

    printf("\n========================================\n");
    printf("9. FIND BIT POSITION(S)\n");
    printf("========================================\n");
    {
        uint8_t value = 0xB6;   /* 10110110 */
        uint8_t pattern = 0b101;
        int positions[8];

        int count = bit_findpos(value, pattern, 3, positions, 8);

        printf("value = ");
        bits_print(value);
        printf("\n");

        printf("pattern 101 found %d time(s)\n", count);

        for (int i = 0; i < count && i < 8; ++i) {
            printf("  match at start=%d\n", positions[i]);
        }
    }

    printf("\n========================================\n");
    printf("10. ENCODE/DECODE/EXECUTE INSTRUCTION(S)\n");
    printf("========================================\n");
    {
        uint8_t regs[REG_COUNT] = {0};

        regs[1] = 0b11001100;
        regs[2] = 0b10101010;
        regs[3] = 5;
        regs[4] = 7;

        printf("initial state\n");
        dump_regs(regs);

        uint8_t instr1 = instr_encode(3, 1, 2); /* NAND R1, R2 */
        uint8_t instr2 = instr_encode(1, 3, 4); /* ADD  R3, R4 */
        uint8_t instr3 = instr_encode(2, 4, 3); /* SUB  R4, R3 */
        uint8_t instr4 = instr_encode(0, 5, 1); /* MOV  R5, R1 */

        printf("---- decode instr1 ----\n");
        instr_decode(instr1);
        instr_exec(instr1, regs);
        dump_regs(regs);

        printf("---- decode instr2 ----\n");
        instr_decode(instr2);
        instr_exec(instr2, regs);
        dump_regs(regs);

        printf("---- decode instr3 ----\n");
        instr_decode(instr3);
        instr_exec(instr3, regs);
        dump_regs(regs);

        printf("---- decode instr4 ----\n");
        instr_decode(instr4);
        instr_exec(instr4, regs);
        dump_regs(regs);
        // uint8_t instr1 = 0b10011101;  /* opcode=2, reg_a=3, reg_b=5 */
        // instr_decode(instr1);
        // printf("\n");

        // printf(" -> encode next instruction [1, 2 ,7]\n");
        // uint8_t instr2 = instr_encode(1, 2, 7);
        // instr_decode(instr2);
        // printf("\n");

        // uint8_t a = instr_encode(0, 1, 2);
        // uint8_t b = instr_encode(1, 2, 7);
        // uint8_t c = instr_encode(2, 3, 5);
        // uint8_t d = instr_encode(3, 6, 1);

        // instr_decode(a);
        // printf("\n");

        // instr_decode(b);
        // printf("\n");

        // instr_decode(c);
        // printf("\n");

        // instr_decode(d);
        // //printf("\n");
    }

    printf("\n========================================\n");
    printf("11. LOGIC GATES\n");
    printf("========================================\n");
    {
        uint8_t a = 0b11001100;
        uint8_t b = 0b10101010;

        demo_pair(a, b);

        a = 0b11110000;
        b = 0b00111100;

        demo_pair(a, b);
    }

    printf("\n========================================\n");
    printf("12. NAND MAGIC\n");
    printf("========================================\n");
    {
        uint8_t regs[REG_COUNT] = {0};

        regs[1] = 0b11001100;
        regs[2] = 0b10101010;
        regs[3] = 5;
        regs[4] = 7;

        // printf("========================================\n");
        // printf("TOY CPU WITH NAND OPCODE\n");
        // printf("========================================\n");

        dump_regs(regs);

        uint8_t instr1 = instr_encode(OP_NAND, 1, 2); /* NAND R1, R2 */
        uint8_t instr2 = instr_encode(OP_ADD,  3, 4); /* ADD  R3, R4  */
        uint8_t instr3 = instr_encode(OP_SUB,  4, 3); /* SUB  R4, R3  */
        uint8_t instr4 = instr_encode(OP_MOV,  5, 1); /* MOV  R5, R1  */

        printf("---- instruction 1 ----\n");
        instr_decode(instr1);
        instr_exec(instr1, regs);
        dump_regs(regs);

        printf("---- instruction 2 ----\n");
        instr_decode(instr2);
        instr_exec(instr2, regs);
        dump_regs(regs);

        printf("---- instruction 3 ----\n");
        instr_decode(instr3);
        instr_exec(instr3, regs);
        dump_regs(regs);

        printf("---- instruction 4 ----\n");
        instr_decode(instr4);
        instr_exec(instr4, regs);
        dump_regs(regs);

        demo_nand(0b11001100, 0b10101010);
        demo_nand(0b11110000, 0b00111100);
    }

    printf("\n========================================\n");
    printf("13. RUN PROGRAMS\n");
    printf("========================================\n");
    {
        /* ------------------------------------- */
        /* NOT demo: R1 = NOT(R1)                */
        /* ------------------------------------- */
        {
            uint8_t regs[REG_COUNT] = {0};
            uint8_t program[] = {
                instr_encode(OP_NAND, 1, 1)
            };

            regs[1] = 0b11001100;

            run_prg("PROGRAM: NOT R1", program, 1, regs);

            show_value("expected", (uint8_t)(~0b11001100));
            show_value("actual", regs[1]);
            printf("\n");
        }

        /* ------------------------------------- */
        /* AND demo: R1 = R1 & R2                */
        /* ------------------------------------- */
        {
            uint8_t regs[REG_COUNT] = {0};
            uint8_t program[] = {
                instr_encode(OP_NAND, 1, 2),
                instr_encode(OP_NAND, 1, 1)
            };

            regs[1] = 0b11001100;
            regs[2] = 0b10101010;

            run_prg("PROGRAM: AND R1, R2 (via NAND)", program, 2, regs);

            show_value("expected", (uint8_t)(0b11001100 & 0b10101010));
            show_value("actual", regs[1]);
            printf("\n");
        }

        /* ------------------------------------- */
        /* OR demo: R1 = R1 | R2                 */
        /* scratch: R3, R4                       */
        /* ------------------------------------- */
        {
            uint8_t regs[REG_COUNT] = {0};
            uint8_t program[] = {
                instr_encode(OP_MOV,  3, 1), /* R3 = A */
                instr_encode(OP_MOV,  4, 2), /* R4 = B */
                instr_encode(OP_NAND, 3, 3), /* R3 = ~A */
                instr_encode(OP_NAND, 4, 4), /* R4 = ~B */
                instr_encode(OP_NAND, 3, 4), /* R3 = A | B */
                instr_encode(OP_MOV,  1, 3)  /* R1 = result */
            };

            regs[1] = 0b11001100;
            regs[2] = 0b10101010;

            run_prg("PROGRAM: OR R1, R2 (via NAND)", program, 6, regs);

            show_value("expected", (uint8_t)(0b11001100 | 0b10101010));
            show_value("actual", regs[1]);
            printf("\n");
        }

        /* ------------------------------------- */
        /* XOR demo: R1 = R1 ^ R2                */
        /* scratch: R3, R4, R5                   */
        /* ------------------------------------- */
        {
            uint8_t regs[REG_COUNT] = {0};
            uint8_t program[] = {
                instr_encode(OP_MOV,  3, 1), /* R3 = A */
                instr_encode(OP_NAND, 3, 2), /* R3 = t1 = NAND(A, B) */

                instr_encode(OP_MOV,  4, 1), /* R4 = A */
                instr_encode(OP_NAND, 4, 3), /* R4 = t2 = NAND(A, t1) */

                instr_encode(OP_MOV,  5, 2), /* R5 = B */
                instr_encode(OP_NAND, 5, 3), /* R5 = t3 = NAND(B, t1) */

                instr_encode(OP_NAND, 4, 5), /* R4 = XOR(A, B) */
                instr_encode(OP_MOV,  1, 4)  /* R1 = result */
            };

            regs[1] = 0b11001100;
            regs[2] = 0b10101010;

            run_prg("PROGRAM: XOR R1, R2 (via NAND)", program, 8, regs);

            show_value("expected", (uint8_t)(0b11001100 ^ 0b10101010));
            show_value("actual", regs[1]);
            printf("\n");
        }
    }

    printf("\n========================================\n");
    printf("13. ASSEMBLE PROGRAMS\n");
    printf("========================================\n");
    {
        const char *source[] = {
            "MOV R3, R1",
            "NAND R3, R2",
            "MOV R4, R1",
            "NAND R4, R3",
            "MOV R5, R2",
            "NAND R5, R3",
            "NAND R4, R5",
            "MOV R1, R4"
        };

        uint8_t prg[PRG_MAX];
        uint8_t regs[REG_COUNT] = {0};
        regs[1] = 0b11001100;
        regs[2] = 0b10101010;

        int count = prg_assemble(source, (int)(sizeof(source) / sizeof(source[0])), prg, PRG_MAX);
        if (count < 0) return 1;

        printf("assembled program: \n");
        for (int i = 0; i < count; ++i) {
            printf("  %2d: ", i);
            bits_print(prg[i]);
            printf("  ");
            instr_decode(prg[i]);
            printf("\n");
        }
        printf("\n");

        printf("initial registers:\n");
        reg_dump(regs);
        const char* title = "assembly demo";
        prg_run(title, prg, count, regs);

        printf("final R1 = %u (0x%02X)\n", regs[1], regs[1]);
        printf("native XOR check = %u (0x%02X)\n", (uint8_t)(0b11001100 ^ 0b10101010), (uint8_t)(0b11001100 ^ 0b10101010));

        return 0;
    }
    printf("<-------------------------------------->\n");
    return 0;
}