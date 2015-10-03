/**
 * @file   template-xyt.c
 * @author rodrigo
 * @date   11/05/2015
 * @brief  Convertion XYT-data => BGM_template.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include <bergamota.h>
#include <template.h>
#include <debug.h>

#include "template-common.h"

static BGM_status
_BGM_parse_xyt_value(char *str, unsigned int *uint_value)
{
    long temp_value;
    char *end_ptr;
    BGM_status status;

    temp_value = strtol(str, &end_ptr, 10);
    if (end_ptr != str) {
        if (temp_value >= 0) {
            *uint_value = (unsigned int)temp_value;
            status = BGM_SUCCESS;
        } else {
            status = BGM_E_PARSER_ERROR;
        }
    } else {
        status = BGM_E_PARSER_ERROR;
    }

    return status;
}

static BGM_status
_BGM_parse_xyt_line(char *line, unsigned int id, struct _BGM_minutia *minutia)
{
    char *field;
    char *field_ctx;
    BGM_status status;

    minutia->num_neighbors = 0;
    minutia->neighbors = NULL;

    // X
    field = strtok_r(line, " ", &field_ctx);
    if (field != NULL) {
        status = _BGM_parse_xyt_value(field, &minutia->x);
    } else {
        status = BGM_E_PARSER_ERROR;
    }
    // Y
    if (status == BGM_SUCCESS) {
        field = strtok_r(NULL, " ", &field_ctx);
        if (field != NULL) {
            status = _BGM_parse_xyt_value(field, &minutia->y);
        } else {
            status = BGM_E_PARSER_ERROR;
        }
    }
    // T
    if (status == BGM_SUCCESS) {
        field = strtok_r(NULL, " ", &field_ctx);
        if (field != NULL) {
            status = _BGM_parse_xyt_value(field, &minutia->angle);
            if (status == BGM_SUCCESS) {
                if (minutia->angle > 359) {
                    status = BGM_E_PARSER_ERROR;
                }
            }
        } else {
            status = BGM_E_PARSER_ERROR;
        }
    }
    // ID
    if (status == BGM_SUCCESS) {
        minutia->id = id;
    }

    return status;
}

static BGM_status
_BGM_parse_xyt_multiline(char *xyt_buffer, unsigned int max_num_minutiae, struct BGM_template *tpl)
{
    char *line;
    char *line_ctx;
    unsigned int minutia_counter = 0;
    BGM_status status;

    tpl->minutiae = malloc(max_num_minutiae * sizeof(*tpl->minutiae));
    if (tpl->minutiae != NULL) {
        status = BGM_SUCCESS;
        // Iterate over the lines
        line = strtok_r(xyt_buffer, "\n", &line_ctx);
        do {
            if (line != NULL) {
                PDEBUG("line: %s\n", line);
                status =
                    _BGM_parse_xyt_line(line, minutia_counter, &tpl->minutiae[minutia_counter]);
                if (status == BGM_SUCCESS) {
                    minutia_counter++;
                }
                line = strtok_r(NULL, "\n", &line_ctx);
            }
        } while (line != NULL && status == BGM_SUCCESS && minutia_counter < max_num_minutiae);
        // If, at least, one minutia was parsed successfully, and the last call to
        // '_BGM_minutia_from_xyt' returned success, everything is fine.
        if (minutia_counter > 0 && status == BGM_SUCCESS) {
            tpl->num_minutiae = minutia_counter;
        } else {
            // Otherwise, free 'minutiae' and set 'status' to apropriate error code
            free(tpl->minutiae);
            tpl->minutiae = NULL;
            tpl->num_minutiae = 0;
            status = BGM_E_PARSER_ERROR;
        }
    } else {
        status = BGM_E_NO_MEMORY;
    }

    return status;
}

/*
 * See documentation in 'include/template.h'
 */
BGM_status
BGM_template_from_xyt(char *xyt_buffer,
                      struct BGM_template *tpl,
                      unsigned int max_num_minutiae,
                      unsigned int num_neighbors)
{
    char *buffer_copy;
    int buffer_size;
    BGM_status status;

    if (xyt_buffer == NULL || tpl == NULL || num_neighbors == 0) {
        PRINT_IF_TRUE(num_neighbors == 0);
        PRINT_IF_NULL(xyt_buffer);
        PRINT_IF_NULL(tpl);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (!_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
        _BGM_reset_template(tpl);
        buffer_size = strlen(xyt_buffer) + 1;
        buffer_copy = malloc(buffer_size);
        if (buffer_copy != NULL) {
            // Copy 'xyt_buffer' because '_BGM_parse_xyt_multiline' changes the input buffer
            memcpy(buffer_copy, xyt_buffer, buffer_size);
            status = _BGM_parse_xyt_multiline(buffer_copy, max_num_minutiae, tpl);
            if (status == BGM_SUCCESS) {
                status = _BGM_intialize_template(tpl, num_neighbors);
            }
            // Free the memory allocated by malloc()
            free(buffer_copy);
        } else {
            status = BGM_E_NO_MEMORY;
        }
        if (status != BGM_SUCCESS) {
            BGM_destroy_template(tpl);
        }
    } else {
        status = BGM_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
