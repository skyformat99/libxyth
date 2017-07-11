// Copyright 2011-2017 Rodrigo Dias Correa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <check.h>
#include <xyth.h>

#define XYT_OK1                                                                \
    "1  2 45\n  4  5 45\n  7  8 45\n 10 11 45\n 13 14 45\n \
                16 17 45\n 19 20 45\n 22 23 45\n 25 26 45\n \
                28 29 45\n 31 32 45\n 34 35 45\n 37 38 45\n \
                40 41 45\n 43 44 45\n 46 47 45\n 49 50 45\n \
                52 53 45\n 55 56 45\n 58 59 45\n 61 62 45\n"

#define XYT_OK2                                                                \
    "1  2 90\n  4  5 90\n  7  8 90\n 10 11 90\n 13 14 90\n \
                16 17 90\n 19 20 90\n 22 23 90\n 25 26 90\n \
                28 29 90\n 31 32 90\n 34 35 90\n 37 38 90\n \
                40 41 90\n 43 44 90\n 46 47 90\n 49 50 90\n \
                52 53 90\n 55 56 90\n 58 59 90\n 61 62 90\n"

struct XYTH_template tpl1 = {0};
struct XYTH_template tpl2 = {0};
struct XYTH_context ctx2 = {0};
unsigned int tpl_id1;
unsigned int tpl_id2;

void identify_setup()
{
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_OK1, &tpl1, 20);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_template_from_xyt(XYT_OK2, &tpl2, 20);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_create_context(&ctx2, NULL);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_set_match_thresholds(&ctx2, 10, 1, 0);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_set_match_tolerances(&ctx2, 5, 5, 7);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_add_template(&ctx2, &tpl1, &tpl_id1);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(tpl_id1, 0);
}

void identify_teardown()
{
    XYTH_destroy_template(&tpl1);
    XYTH_destroy_template(&tpl2);
    XYTH_destroy_context(&ctx2);
}

START_TEST(simple_success)
{
    XYTH_status status;
    unsigned int matches[1];
    unsigned int matches_length = 1;

    status = XYTH_identify(&ctx2, &tpl1, &matches_length, matches);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(matches_length, 1);
    ck_assert_int_eq(matches[0], tpl_id1);
}
END_TEST

START_TEST(tpl_not_found)
{
    XYTH_status status;
    unsigned int matches[1];
    unsigned int matches_length = 1;

    status = XYTH_identify(&ctx2, &tpl2, &matches_length, matches);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(matches_length, 0);
}
END_TEST

START_TEST(success_2_templates)
{
    XYTH_status status;
    unsigned int matches[1];
    unsigned int matches_length = 1;

    status = XYTH_add_template(&ctx2, &tpl2, &tpl_id2);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(tpl_id2, 1);

    status = XYTH_identify(&ctx2, &tpl2, &matches_length, matches);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(matches_length, 1);
    ck_assert_int_eq(matches[0], tpl_id2);

    matches_length = 1;
    status = XYTH_identify(&ctx2, &tpl1, &matches_length, matches);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(matches_length, 1);
    ck_assert_int_eq(matches[0], tpl_id1);
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
