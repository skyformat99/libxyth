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
