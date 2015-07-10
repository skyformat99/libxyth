/**
 * @file   check_remove_template.c
 * @author rodrigo
 * @date   23/03/2015
 * @brief  Tests for 'BGM_remove_template'.
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

struct BGM_template tpl;
struct BGM_context ctx;

void remove_template_setup()
{
    BGM_status status;

    status = BGM_template_from_xyt(XYT_OK, &tpl, 64, 10);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_create_context(&ctx, NULL);
    ck_assert_int_eq(status, BGM_SUCCESS);
}

void remove_template_teardown()
{
    BGM_destroy_template(&tpl);
    BGM_destroy_context(&ctx);
}

START_TEST(simple_success)
{
    BGM_status status;
    unsigned int old_tpl_counter;
    unsigned int id;

    status = BGM_add_template(&ctx, &tpl, &id);
    ck_assert_int_eq(status, BGM_SUCCESS);

    old_tpl_counter = ctx.db.templates_counter;

    status = BGM_remove_template(&ctx, &tpl, id);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(ctx.db.templates_counter, old_tpl_counter - 1);
}
END_TEST

START_TEST(null_context)
{
    BGM_status status;

    status = BGM_remove_template(NULL, &tpl, 0);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_context)
{
    BGM_status status;
    struct BGM_context invalid_ctx;

    status = BGM_remove_template(&invalid_ctx, &tpl, 0);
    ck_assert_int_eq(status, BGM_E_NOT_INITIALIZED);
}
END_TEST

START_TEST(null_template)
{
    BGM_status status;

    status = BGM_remove_template(&ctx, NULL, 0);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_template)
{
    BGM_status status;
    struct BGM_template invalid_tpl;

    status = BGM_remove_template(&ctx, &invalid_tpl, 0);
    ck_assert_int_eq(status, BGM_E_NOT_INITIALIZED);
}
END_TEST

TCase *remove_template_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("RemoveTemplate");

    tcase_add_unchecked_fixture(tcase, remove_template_setup, remove_template_teardown);

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, null_context);
    tcase_add_test(tcase, invalid_context);
    tcase_add_test(tcase, null_template);
    tcase_add_test(tcase, invalid_template);

    return tcase;
}
