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

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include <debug.h>
#include <template.h>
#include <xyth.h>

#include "config.h"
#include "template-common.h"

static XYTH_status _XYTH_parse_xyt_value(char *str, unsigned int *uint_value)
{
    long temp_value;
    char *end_ptr;
    XYTH_status status;

    temp_value = strtol(str, &end_ptr, 10);
    if (end_ptr != str) {
        if (temp_value >= 0) {
            *uint_value = (unsigned int)temp_value;
            status = XYTH_SUCCESS;
        } else {
            status = XYTH_E_PARSER_ERROR;
        }
    } else {
        status = XYTH_E_PARSER_ERROR;
    }

    return status;
}

static XYTH_status _XYTH_parse_xyt_line(char *line, unsigned int id,
                                        struct _XYTH_minutia *minutia)
{
    char *field;
    char *field_ctx;
    XYTH_status status;

    minutia->num_neighbors = 0;
    minutia->neighbors = NULL;

    // X
    field = strtok_r(line, " ", &field_ctx);
    if (field != NULL) {
        status = _XYTH_parse_xyt_value(field, &minutia->x);
    } else {
        status = XYTH_E_PARSER_ERROR;
    }
    // Y
    if (status == XYTH_SUCCESS) {
        field = strtok_r(NULL, " ", &field_ctx);
        if (field != NULL) {
            status = _XYTH_parse_xyt_value(field, &minutia->y);
        } else {
            status = XYTH_E_PARSER_ERROR;
        }
    }
    // T
    if (status == XYTH_SUCCESS) {
        field = strtok_r(NULL, " ", &field_ctx);
        if (field != NULL) {
            status = _XYTH_parse_xyt_value(field, &minutia->angle);
            if (status == XYTH_SUCCESS) {
                if (minutia->angle > 359) {
                    status = XYTH_E_PARSER_ERROR;
                }
            }
        } else {
            status = XYTH_E_PARSER_ERROR;
        }
    }
    // ID
    if (status == XYTH_SUCCESS) {
        minutia->id = id;
    }

    return status;
}

static XYTH_status _XYTH_parse_xyt_multiline(char *xyt_buffer,
                                             struct XYTH_template *tpl)
{
    char *line;
    char *line_ctx;
    unsigned int minutia_counter = 0;
    XYTH_status status;

    tpl->minutiae = malloc(MAX_MINUTIAE_PER_TEMPLATE * sizeof(*tpl->minutiae));
    if (tpl->minutiae != NULL) {
        status = XYTH_SUCCESS;
        // Iterate over the lines
        line = strtok_r(xyt_buffer, "\n", &line_ctx);
        do {
            if (line != NULL) {
                PDEBUG("line: %s\n", line);
                status = _XYTH_parse_xyt_line(line, minutia_counter,
                                              &tpl->minutiae[minutia_counter]);
                if (status == XYTH_SUCCESS) {
                    minutia_counter++;
                }
                line = strtok_r(NULL, "\n", &line_ctx);
            }
        } while (line != NULL && status == XYTH_SUCCESS &&
                 minutia_counter < MAX_MINUTIAE_PER_TEMPLATE);
        // If, at least, one minutia was parsed successfully, and the last call
        // to '_XYTH_minutia_from_xyt' returned success, everything is fine.
        if (minutia_counter > 0 && status == XYTH_SUCCESS) {
            tpl->num_minutiae = minutia_counter;
        } else {
            // Otherwise, free 'minutiae' and set 'status' to apropriate error
            // code
            free(tpl->minutiae);
            tpl->minutiae = NULL;
            tpl->num_minutiae = 0;
            status = XYTH_E_PARSER_ERROR;
        }
    } else {
        status = XYTH_E_NO_MEMORY;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

XYTH_status XYTH_template_from_xyt(char *xyt_buffer, struct XYTH_template *tpl,
                                   unsigned int num_neighbors)
{
    char *buffer_copy;
    int buffer_size;
    XYTH_status status;

    if (xyt_buffer == NULL || tpl == NULL || num_neighbors == 0) {
        PRINT_IF_TRUE(num_neighbors == 0);
        PRINT_IF_NULL(xyt_buffer);
        PRINT_IF_NULL(tpl);
        status = XYTH_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (!_XYTH_IS_TEMPLATE_INITIALIZED(*tpl)) {
        _XYTH_reset_template(tpl);
        buffer_size = strlen(xyt_buffer) + 1;
        buffer_copy = malloc(buffer_size);
        if (buffer_copy != NULL) {
            // Copy 'xyt_buffer' because '_XYTH_parse_xyt_multiline' changes the
            // input buffer
            memcpy(buffer_copy, xyt_buffer, buffer_size);
            status = _XYTH_parse_xyt_multiline(buffer_copy, tpl);
            if (status == XYTH_SUCCESS) {
                status = _XYTH_intialize_template(tpl, num_neighbors);
            }
            // Free the memory allocated by malloc()
            free(buffer_copy);
        } else {
            status = XYTH_E_NO_MEMORY;
        }
        if (status != XYTH_SUCCESS) {
            XYTH_destroy_template(tpl);
        }
    } else {
        status = XYTH_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
