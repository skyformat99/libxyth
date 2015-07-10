/**
 * @file   check_destroy_context.c
 * @author rodrigo
 * @date   22/03/2015
 * @brief  Tests for 'BGM_destroy_context'.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>
#include <bergamota.h>

START_TEST(simple_success)
{
    BGM_status status;
    struct BGM_context ctx;

    status = BGM_create_context(&ctx, NULL);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(ctx.magic_number, _BGM_CONTEXT_INIT_MAGIC_NUMBER);
    BGM_destroy_context(&ctx);
    ck_assert_int_ne(ctx.magic_number, _BGM_CONTEXT_INIT_MAGIC_NUMBER);
}
END_TEST

START_TEST(null_context)
{
    BGM_destroy_context(NULL);
}
END_TEST

START_TEST(invalid_context)
{
    struct BGM_context ctx;

    BGM_destroy_context(&ctx);
    ck_assert_int_ne(ctx.magic_number, _BGM_CONTEXT_INIT_MAGIC_NUMBER);
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
