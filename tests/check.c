/**
 * @file   check.c
 * @author rodrigo
 * @date   19/03/2015
 * @brief  Main unit test file, where all test suites are called.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <check.h>
#include <stdlib.h>

// From check_template.c
Suite *template_suite(void);

// From check_context.c
Suite *context_suite(void);

int
main(void)
{
    int number_failed;
    SRunner *sr;

    sr = srunner_create(template_suite());
    srunner_add_suite(sr, context_suite());

    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
