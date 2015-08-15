/**
 * @file   check_identify.c
 * @author rodrigo
 * @date   29/03/2015
 * @brief  Tests for 'BGM_identify'.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>
#include <bergamota.h>

#define XYT_OK1 "1  2 45\n  4  5 45\n  7  8 45\n 10 11 45\n 13 14 45\n \
                16 17 45\n 19 20 45\n 22 23 45\n 25 26 45\n \
                28 29 45\n 31 32 45\n 34 35 45\n 37 38 45\n \
                40 41 45\n 43 44 45\n 46 47 45\n 49 50 45\n \
                52 53 45\n 55 56 45\n 58 59 45\n 61 62 45\n"

#define XYT_OK2 "1  2 90\n  4  5 90\n  7  8 90\n 10 11 90\n 13 14 90\n \
                16 17 90\n 19 20 90\n 22 23 90\n 25 26 90\n \
                28 29 90\n 31 32 90\n 34 35 90\n 37 38 90\n \
                40 41 90\n 43 44 90\n 46 47 90\n 49 50 90\n \
                52 53 90\n 55 56 90\n 58 59 90\n 61 62 90\n"

struct BGM_template tpl1 = {0};
struct BGM_template tpl2 = {0};
struct BGM_context ctx2 = {0};
unsigned int tpl_id1;
unsigned int tpl_id2;

void identify_setup()
{
    BGM_status status;

    status = BGM_template_from_xyt(XYT_OK1, &tpl1, 64, 20);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_template_from_xyt(XYT_OK2, &tpl2, 64, 20);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_create_context(&ctx2, NULL);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_set_match_thresholds(&ctx2, 10, 1, 0);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_set_match_tolerances(&ctx2, 5, 5, 7);
    ck_assert_int_eq(status, BGM_SUCCESS);

    status = BGM_add_template(&ctx2, &tpl1, &tpl_id1);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(tpl_id1, 0);

}

void identify_teardown()
{
    BGM_destroy_template(&tpl1);
    BGM_destroy_template(&tpl2);
    BGM_destroy_context(&ctx2);
}

START_TEST(simple_success)
{
    BGM_status status;
    unsigned int id = BGM_RESERVED_TEMPLATE_ID;
    bool found;

    status = BGM_identify(&ctx2, &tpl1, &found, &id);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(id, tpl_id1);
    ck_assert(found);
}
END_TEST

START_TEST(tpl_not_found)
{
    BGM_status status;
    bool found;
    unsigned int id = 3;

    status = BGM_identify(&ctx2, &tpl2, &found, &id);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(id, BGM_RESERVED_TEMPLATE_ID);
    ck_assert(!found);
}
END_TEST

START_TEST(success_2_templates)
{
    BGM_status status;
    bool found;
    unsigned int id = 3;

    status = BGM_add_template(&ctx2, &tpl2, &tpl_id2);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(tpl_id2, 1);

    status = BGM_identify(&ctx2, &tpl2, &found, &id);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(id, tpl_id2);
    ck_assert(found);

    status = BGM_identify(&ctx2, &tpl1, &found, &id);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(id, tpl_id1);
    ck_assert(found);
}
END_TEST

TCase *identify_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("Identify");

    tcase_add_unchecked_fixture(tcase, identify_setup, identify_teardown);

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, tpl_not_found);
    tcase_add_test(tcase, success_2_templates);
//    tcase_add_test(tcase, null_template);
//    tcase_add_test(tcase, invalid_template);
//    tcase_add_test(tcase, null_id);

    return tcase;
}




