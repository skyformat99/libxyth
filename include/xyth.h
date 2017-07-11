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

#ifndef XYTH_H
#define XYTH_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>
#include <context.h>

#define XYTH_RESERVED_TEMPLATE_ID ((unsigned int)-1)

extern unsigned int XYTH_version_major;
extern unsigned int XYTH_version_minor;

// Error codes
typedef enum {
    XYTH_SUCCESS = 0, // Success
    XYTH_ERROR = -1,  // Generic error
    // Specialized errors
    XYTH_E_NOT_IMPLEMENTED = -2,
    XYTH_E_PARSER_ERROR = -3,
    XYTH_E_NO_MEMORY = -4,
    XYTH_E_INVALID_PARAMETER = -5,
    XYTH_E_TOO_FEW_MINUTIAE = -6,
    XYTH_E_ALREADY_INITIALIZED = -7,
    XYTH_E_INVALID_CONFIGURATION = -8,
    XYTH_E_NOT_INITIALIZED = -9,
    XYTH_E_NOT_FOUND = -10,
    XYTH_E_INCOMPLETE_REMOVAL = -11,
    XYTH_E_VALUE_OUT_OF_RANGE = -12,
    XYTH_E_MINUTIAE_EXTRACTOR_ERROR = -13
} XYTH_status;

//
// Public functions/macros
//

#define XYTH_DB_CONFIG_INIT(cfg) _XYTH_DB_CONFIG_INIT(cfg)

#define XYTH_DB_CONFIG_SET_MAX_COORD(cfg, x, y) \
    _XYTH_DB_CONFIG_SET_MAX_COORD(cfg, x, y)

#define XYTH_DB_CONFIG_SET_DENSITY(cfg, ppg, dpg) \
    _XYTH_DB_CONFIG_SET_DENSITY(cfg, ppg, dpg)

XYTH_status
XYTH_create_context(struct XYTH_context *ctx, struct XYTH_database_config *db_cfg);

void XYTH_destroy_context(struct XYTH_context *ctx);

XYTH_status XYTH_add_template(struct XYTH_context *ctx,
                            struct XYTH_template *tpl,
                            unsigned int *tpl_id);

XYTH_status XYTH_remove_template(struct XYTH_context *ctx,
                               struct XYTH_template *tpl,
                               unsigned int tpl_id);

XYTH_status XYTH_identify(struct XYTH_context *ctx,
                        struct XYTH_template *tpl,
                        unsigned int *num_ids,
                        unsigned int *ids);

XYTH_status XYTH_template_from_xyt(char *xyt_buffer,
                                 struct XYTH_template *tpl,
                                 unsigned int num_neighbors);

XYTH_status XYTH_template_from_raw_image(unsigned char *buffer,
                                       unsigned int width,
                                       unsigned int height,
                                       unsigned int pixel_depth,
                                       unsigned int resolution_in_ppi,
                                       struct XYTH_template *tpl,
                                       unsigned int num_neighbors);

void XYTH_destroy_template(struct XYTH_template *tpl);

XYTH_status XYTH_set_match_tolerances(struct XYTH_context *ctx,
                                    unsigned int x_tol,
                                    unsigned int y_tol,
                                    unsigned int angle_tol);

XYTH_status XYTH_set_match_thresholds(struct XYTH_context *ctx,
                                    unsigned int minutia_threshold,
                                    unsigned int template_threshold,
                                    unsigned int failure_threshold);

XYTH_status
XYTH_get_template_counter(struct XYTH_context *ctx, unsigned int *tpl_counter);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XYTH_H
