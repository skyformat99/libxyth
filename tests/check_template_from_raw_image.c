/**
 * @file   check_template_from_raw_image.c
 * @author rodrigo
 * @date   14/05/2015
 * @brief  Tests for 'BGM_template_from_raw_image'.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>
#include <bergamota.h>

#include "test-raw-image1.h"

//#include <stdio.h> // To print a template

START_TEST(simple_success)
{
    struct BGM_template tpl = {0};
    BGM_status status;

    status = BGM_template_from_raw_image(test_raw_image1, test_raw_image1_width,
                                         test_raw_image1_height, test_raw_image1_pixel_depth,
                                         test_raw_image1_ppi, &tpl, 100, 10);
    ck_assert_int_eq(status, BGM_SUCCESS);
    ck_assert_int_eq(tpl.magic_num, _BGM_TEMPLATE_INIT_MAGIC_NUMBER);
    ck_assert_int_eq(tpl.num_minutiae, 49);

    // To print the template
//    for (int i = 0; i < tpl.num_minutiae; i++) {
//        printf("%d %d %d\n", tpl.minutiae[i].x, tpl.minutiae[i].y, tpl.minutiae[i].angle);
//    }

    BGM_destroy_template(&tpl);
}
END_TEST

TCase *template_from_raw_image_tcase(void)
{
    TCase *tcase;

    tcase = tcase_create("TemplateFromRawImage");

//    tcase_add_test(tcase, null_xyt);
//    tcase_add_test(tcase, empty_xyt);
//    tcase_add_test(tcase, neg_x);
//    tcase_add_test(tcase, neg_y);
//    tcase_add_test(tcase, neg_t);
//    tcase_add_test(tcase, t_too_big);
//    tcase_add_test(tcase, null_tpl);
//    tcase_add_test(tcase, exceed_max_min);
//    tcase_add_test(tcase, too_many_neighbors);
    tcase_add_test(tcase, simple_success);
//    tcase_add_test(tcase, basic_internals);

    return tcase;
}


