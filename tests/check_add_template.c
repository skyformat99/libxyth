/**
 * @file   check_add_template.c
 * @author rodrigo
 * @date   23/03/2015
 * @brief  Tests for 'BGM_add_template'.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>
#include <bergamota.h>

#define XYT_OK "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                16 17 18\n 19 20 21\n 22 23 24\n 25 26 27\n \
                28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                40 41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                52 53 54\n 55 56 57\n 58 59 60\n 61 62 63\n"


/*
    // Neighbors of this template

    tpl.minutiae[0].neighbors[0].neighbor_id == 1
    tpl.minutiae[0].neighbors[0].relative_x == 4
    tpl.minutiae[0].neighbors[0].relative_y == -4
    tpl.minutiae[0].neighbors[0].relative_angle == 180

    tpl.minutiae[1].neighbors[0].neighbor_id == 0
    tpl.minutiae[1].neighbors[0].relative_x == 4
    tpl.minutiae[1].neighbors[0].relative_y == -4
    tpl.minutiae[1].neighbors[0].relative_angle == 180

*/
#define XYT_2MIN_5X5 "0 0 90\n 4 4 270\n"

struct BGM_template tpl = {0};
struct BGM_context ctx = {0};

void add_template_setup()
{
    BGM_status status;
    struct BGM_database_config cfg;

    BGM_DB_CONFIG_INIT(cfg);
    BGM_DB_CONFIG_SET_MAX_COORD(cfg, 4, 4);
    BGM_DB_CONFIG_SET_DENSITY(cfg, 1, 90);

    status = BGM_template_from_xyt(XYT_2MIN_5X5, &tpl, 4, 1);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_create_context(&ctx, &cfg);
    ck_assert_int_eq(status, BGM_SUCCESS);
}

void add_template_teardown()
{
    BGM_destroy_template(&tpl);
    BGM_destroy_context(&ctx);
}

START_TEST(simple_success)
{
    BGM_status status;
    unsigned int old_next_tpl_id;
    unsigned int old_tpl_counter;
    unsigned int tpl_counter;
    unsigned int id = 1000; // Any number...

    old_next_tpl_id = ctx.db.next_template_id;
    BGM_get_template_counter(&ctx, &old_tpl_counter);

    status = BGM_add_template(&ctx, &tpl, &id);

    BGM_get_template_counter(&ctx, &tpl_counter);

    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(id, old_next_tpl_id);
    ck_assert_int_eq(tpl_counter, old_tpl_counter + 1);
    ck_assert_int_eq(ctx.db.next_template_id, old_next_tpl_id + 1);
    ck_assert_int_eq(ctx.db.alloc_counter[290], 32);
     // Index calculated using 'index_calc.py'
    ck_assert_int_eq(ctx.db.data[290][0], 0);
    ck_assert_int_eq(ctx.db.data[290][1], 1);
    ck_assert_int_eq(ctx.db.data[290][2], (unsigned int)-1);
    // Make sure that the other groups don't have memory allocated.
    for (int i = 0; i < ctx.db.num_groups; i++) {
        if (i == 290) continue;
        ck_assert_int_eq(ctx.db.data[i], NULL);
        ck_assert_int_eq(ctx.db.alloc_counter[i], 0);
    }
}
END_TEST

START_TEST(null_context)
{
    BGM_status status;
    unsigned int id;

    status = BGM_add_template(NULL, &tpl, &id);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_context)
{
    BGM_status status;
    struct BGM_context invalid_ctx = {0};
    unsigned int id;

    status = BGM_add_template(&invalid_ctx, &tpl, &id);
    ck_assert_int_eq(status, BGM_E_NOT_INITIALIZED);
}
END_TEST

START_TEST(null_template)
{
    BGM_status status;
    unsigned int id;

    status = BGM_add_template(&ctx, NULL, &id);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_template)
{
    BGM_status status;
    struct BGM_template invalid_tpl = {0};
    unsigned int id;

    status = BGM_add_template(&ctx, &invalid_tpl, &id);
    ck_assert_int_eq(status, BGM_E_NOT_INITIALIZED);
}
END_TEST

START_TEST(null_id)
{
    BGM_status status;
    struct BGM_template invalid_tpl = {0};

    status = BGM_add_template(&ctx, &invalid_tpl, NULL);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
}
END_TEST

TCase *add_template_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("AddTemplate");

    tcase_add_unchecked_fixture(tcase, add_template_setup, add_template_teardown);

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, null_context);
    tcase_add_test(tcase, invalid_context);
    tcase_add_test(tcase, null_template);
    tcase_add_test(tcase, invalid_template);
    tcase_add_test(tcase, null_id);

    return tcase;
}


