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

#define XYT_OK                                                                 \
    "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                16 17 18\n 19 20 21\n 22 23 24\n 25 26 27\n \
                28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                40 41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                52 53 54\n 55 56 57\n 58 59 60\n 61 62 63\n"

#define XYT_NEG_X                                                              \
    "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                  16 17 18\n 19 20 21\n 22 23 24\n 25 26 27\n \
                  -28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                  40 41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                  52 53 54\n 55 56 57\n 58 59 60\n 61 62 63\n"

#define XYT_NEG_Y                                                              \
    "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                  16 17 18\n 19 20 21\n 22 23 24\n 25 26 27\n \
                  28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                  40 -41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                  52 53 54\n 55 56 57\n 58 59 60\n 61 62 63\n"

#define XYT_NEG_T                                                              \
    "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                  16 17 18\n 19 20 21\n 22 23 24\n 25 26 27\n \
                  28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                  40 41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                  52 53 -54\n 55 56 57\n 58 59 60\n 61 62 63\n"

#define XYT_T_2BIG                                                             \
    "1 2 3\n 4 5 6\n 7 8 9\n 10 11 12\n 13 14 15\n \
                   16 17 360\n 19 20 21\n 22 23 24\n 25 26 27\n \
                   28 29 30\n 31 32 33\n 34 35 36\n 37 38 39\n \
                   40 41 42\n 43 44 45\n 46 47 48\n 49 50 51\n \
                   52 53 54\n 55 56 57\n 58 59 60\n 61 62 63\n"

#define XYT_2MIN_5X5 "0 0 90\n 4 4 270\n"

START_TEST(basic_internals)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_2MIN_5X5, &tpl, 1);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
    ck_assert_int_eq(tpl.num_minutiae, 2);
    ck_assert_ptr_ne(tpl.minutiae, NULL);
    // min 0
    ck_assert_int_eq(tpl.minutiae[0].id, 0);
    ck_assert_int_eq(tpl.minutiae[0].angle, 90);
    ck_assert_int_eq(tpl.minutiae[0].x, 0);
    ck_assert_int_eq(tpl.minutiae[0].y, 0);
    ck_assert_int_eq(tpl.minutiae[0].num_neighbors, 1);
    ck_assert_int_eq(tpl.minutiae[0].neighbors[0].neighbor_id, 1);
    ck_assert_int_eq(tpl.minutiae[0].neighbors[0].relative_x, 4);
    ck_assert_int_eq(tpl.minutiae[0].neighbors[0].relative_y, -4);
    ck_assert_int_eq(tpl.minutiae[0].neighbors[0].relative_angle, 180);
    // min 1
    ck_assert_int_eq(tpl.minutiae[1].id, 1);
    ck_assert_int_eq(tpl.minutiae[1].angle, 270);
    ck_assert_int_eq(tpl.minutiae[1].x, 4);
    ck_assert_int_eq(tpl.minutiae[1].y, 4);
    ck_assert_int_eq(tpl.minutiae[1].num_neighbors, 1);
    ck_assert_int_eq(tpl.minutiae[1].neighbors[0].neighbor_id, 0);
    ck_assert_int_eq(tpl.minutiae[1].neighbors[0].relative_x, 4);
    ck_assert_int_eq(tpl.minutiae[1].neighbors[0].relative_y, -4);
    ck_assert_int_eq(tpl.minutiae[1].neighbors[0].relative_angle, 180);

    XYTH_destroy_template(&tpl);
}
END_TEST

START_TEST(null_xyt)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(NULL, &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_INVALID_PARAMETER);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(empty_xyt)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt("", &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_PARSER_ERROR);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(neg_x)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_NEG_X, &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_PARSER_ERROR);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(neg_y)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_NEG_Y, &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_PARSER_ERROR);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(neg_t)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_NEG_T, &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_PARSER_ERROR);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(t_too_big)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_T_2BIG, &tpl, 5);
    ck_assert_int_eq(status, XYTH_E_PARSER_ERROR);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(null_tpl)
{
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_OK, NULL, 5);
    ck_assert_int_eq(status, XYTH_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(too_many_neighbors)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_OK, &tpl, 21);
    ck_assert_int_eq(status, XYTH_E_TOO_FEW_MINUTIAE);
    ck_assert_int_ne(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(simple_success)
{
    struct XYTH_template tpl = {0};
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_OK, &tpl, 5);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(tpl.magic_num, _XYTH_TEMPLATE_INIT_MAGIC_NUMBER);
    ck_assert_int_eq(tpl.num_minutiae, 21);
    XYTH_destroy_template(&tpl);
}
END_TEST

TCase *template_from_xyt_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("TemplateFromXyt");

    tcase_add_test(tcase, null_xyt);
    tcase_add_test(tcase, empty_xyt);
    tcase_add_test(tcase, neg_x);
    tcase_add_test(tcase, neg_y);
    tcase_add_test(tcase, neg_t);
    tcase_add_test(tcase, t_too_big);
    tcase_add_test(tcase, null_tpl);
    tcase_add_test(tcase, too_many_neighbors);
    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, basic_internals);

    return tcase;
}
