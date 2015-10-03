/**
 * @file   check_template.c
 * @author rodrigo
 * @date   16/03/2015
 * @brief  Test suite to exercise functions from template.c.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>

// From check_template_from_xyt.c
TCase *template_from_xyt_tcase(void);

// From check_destroy_template.c
TCase *destroy_template_tcase(void);

// From check_template_from_raw_image.c
TCase *template_from_raw_image_tcase(void);

Suite *
template_suite(void)
{
    Suite *suite;

    suite = suite_create("\n Template");

    suite_add_tcase(suite, template_from_xyt_tcase());
    suite_add_tcase(suite, destroy_template_tcase());
    suite_add_tcase(suite, template_from_raw_image_tcase());

    return suite;
}
