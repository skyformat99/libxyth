/**
 * @file   check_context.c
 * @author rodrigo
 * @date   21/03/2015
 * @brief  Test suite to exercise functions from context.c.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>

// From check_create_context.c
TCase *create_context_tcase(void);

// From check_destroy_context.c
TCase *destroy_context_tcase(void);

// From check_add_template.c
TCase *add_template_tcase(void);

// From check_remove_template.c
TCase *remove_template_tcase(void);

// From check_identify.c
TCase *identify_tcase(void);

Suite *
context_suite(void)
{
    Suite *suite;
    TCase *id_tcase;

    suite = suite_create("Context");

    suite_add_tcase(suite, create_context_tcase());
    suite_add_tcase(suite, destroy_context_tcase());
    suite_add_tcase(suite, add_template_tcase());
    suite_add_tcase(suite, remove_template_tcase());

    id_tcase = identify_tcase();
    tcase_set_timeout(id_tcase, 120);
    suite_add_tcase(suite, id_tcase);

    return suite;
}
