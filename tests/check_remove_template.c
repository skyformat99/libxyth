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

struct XYTH_template tpl3 = {0};
struct XYTH_context ctx1 = {0};

void remove_template_setup()
{
    XYTH_status status;

    status = XYTH_template_from_xyt(XYT_OK, &tpl3, 10);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    status = XYTH_create_context(&ctx1, NULL);
    ck_assert_int_eq(status, XYTH_SUCCESS);
}

void remove_template_teardown()
{
    XYTH_destroy_template(&tpl3);
    XYTH_destroy_context(&ctx1);
}

START_TEST(simple_success)
{
    XYTH_status status;
    unsigned int old_tpl_counter;
    unsigned int tpl_counter;
    unsigned int id;

    status = XYTH_add_template(&ctx1, &tpl3, &id);
    ck_assert_int_eq(status, XYTH_SUCCESS);

    XYTH_get_template_counter(&ctx1, &old_tpl_counter);

    status = XYTH_remove_template(&ctx1, &tpl3, id);

    XYTH_get_template_counter(&ctx1, &tpl_counter);

    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(tpl_counter, old_tpl_counter - 1);
}
END_TEST

START_TEST(null_context)
{
    XYTH_status status;

    status = XYTH_remove_template(NULL, &tpl3, 0);
    ck_assert_int_eq(status, XYTH_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_context)
{
    XYTH_status status;
    struct XYTH_context invalid_ctx = {0};

    status = XYTH_remove_template(&invalid_ctx, &tpl3, 0);
    ck_assert_int_eq(status, XYTH_E_NOT_INITIALIZED);
}
END_TEST

START_TEST(null_template)
{
    XYTH_status status;

    status = XYTH_remove_template(&ctx1, NULL, 0);
    ck_assert_int_eq(status, XYTH_E_INVALID_PARAMETER);
}
END_TEST

START_TEST(invalid_template)
{
    XYTH_status status;
    struct XYTH_template invalid_tpl = {0};

    status = XYTH_remove_template(&ctx1, &invalid_tpl, 0);
    ck_assert_int_eq(status, XYTH_E_NOT_INITIALIZED);
}
END_TEST

TCase *remove_template_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("RemoveTemplate");

    tcase_add_unchecked_fixture(tcase, remove_template_setup,
                                remove_template_teardown);

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, null_context);
    tcase_add_test(tcase, invalid_context);
    tcase_add_test(tcase, null_template);
    tcase_add_test(tcase, invalid_template);

    return tcase;
}
