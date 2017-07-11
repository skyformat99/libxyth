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

START_TEST(simple_success)
{
    XYTH_status status;
    struct XYTH_context ctx = {0};

    status = XYTH_create_context(&ctx, NULL);
    ck_assert_int_eq(status, XYTH_SUCCESS);
    ck_assert_int_eq(ctx.magic_number, _XYTH_CONTEXT_INIT_MAGIC_NUMBER);
    XYTH_destroy_context(&ctx);
    ck_assert_int_ne(ctx.magic_number, _XYTH_CONTEXT_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(null_context) { XYTH_destroy_context(NULL); }
END_TEST

START_TEST(invalid_context)
{
    struct XYTH_context ctx = {0};

    XYTH_destroy_context(&ctx);
    ck_assert_int_ne(ctx.magic_number, _XYTH_CONTEXT_INIT_MAGIC_NUMBER);
}
END_TEST

TCase *destroy_context_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("DestroyContext");

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, null_context);
    tcase_add_test(tcase, invalid_context);

    return tcase;
}
