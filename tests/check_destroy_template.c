/**
 * @file   check_destroy_template.c
 * @author rodrigo
 * @date   21/03/2015
 * @brief  Tests for 'BGM_destroy_template'.
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

START_TEST(null_tpl)
{
    BGM_destroy_template(NULL);
}
END_TEST

START_TEST(invalid_tpl)
{
    struct BGM_template tpl = {0};

    BGM_destroy_template(&tpl);
    ck_assert_int_ne(tpl.magic_num, _BGM_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(simple_success)
{
    struct BGM_template tpl = {0};
    BGM_status status;

    status = BGM_template_from_xyt(XYT_OK, &tpl, 64, 5);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(tpl.magic_num, _BGM_TEMPLATE_INIT_MAGIC_NUMBER);
    ck_assert_int_eq(tpl.num_minutiae, 21);
    BGM_destroy_template(&tpl);
    ck_assert_int_ne(tpl.magic_num, _BGM_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

TCase *destroy_template_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("DestroyTemplate");

    tcase_add_test(tcase, null_tpl);
    tcase_add_test(tcase, invalid_tpl);
    tcase_add_test(tcase, simple_success);

    return tcase;
}
