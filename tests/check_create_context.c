/**
 * @file   check_create_context.c
 * @author rodrigo
 * @date   21/03/2015
 * @brief  Tests for 'BGM_create_context'.
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
    struct BGM_context ctx = {0};

    status = BGM_create_context(&ctx, NULL);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(ctx.magic_number, _BGM_CONTEXT_INIT_MAGIC_NUMBER);

    BGM_destroy_context(&ctx);
}
END_TEST

START_TEST(null_context)
{
    BGM_status status;
    struct BGM_context ctx = {0};

    status = BGM_create_context(NULL, NULL);
    ck_assert_int_eq(status, BGM_E_INVALID_PARAMETER);
    ck_assert_int_ne(ctx.magic_number, _BGM_CONTEXT_INIT_MAGIC_NUMBER);
}
END_TEST

TCase *create_context_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("CreateContext");

    tcase_add_test(tcase, simple_success);
    tcase_add_test(tcase, null_context);

    return tcase;
}
