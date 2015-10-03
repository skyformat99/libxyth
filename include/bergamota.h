/**
 * @file   bergamota.h
 * @author rodrigo
 * @date   14/10/2014
 * @brief  Main bergamota header
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef BERGAMOTA_H
#define BERGAMOTA_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>
#include <context.h>

#define BGM_RESERVED_TEMPLATE_ID ((unsigned int)-1)

extern unsigned int BGM_version_major;
extern unsigned int BGM_version_minor;

// Error codes
typedef enum {
    BGM_SUCCESS = 0, // Success
    BGM_ERROR = -1,  // Generic error
    // Specialized errors
    BGM_E_NOT_IMPLEMENTED = -2,
    BGM_E_PARSER_ERROR = -3,
    BGM_E_NO_MEMORY = -4,
    BGM_E_INVALID_PARAMETER = -5,
    BGM_E_TOO_FEW_MINUTIAE = -6,
    BGM_E_ALREADY_INITIALIZED = -7,
    BGM_E_INVALID_CONFIGURATION = -8,
    BGM_E_NOT_INITIALIZED = -9,
    BGM_E_NOT_FOUND = -10,
    BGM_E_INCOMPLETE_REMOVAL = -11,
    BGM_E_VALUE_OUT_OF_RANGE = -12,
    BGM_E_MINUTIAE_EXTRACTOR_ERROR = -13
} BGM_status;

//
// Public functions/macros
//

#define BGM_DB_CONFIG_INIT(cfg) _BGM_DB_CONFIG_INIT(cfg)

#define BGM_DB_CONFIG_SET_MAX_COORD(cfg, x, y) _BGM_DB_CONFIG_SET_MAX_COORD(cfg, x, y)

#define BGM_DB_CONFIG_SET_DENSITY(cfg, ppg, dpg) _BGM_DB_CONFIG_SET_DENSITY(cfg, ppg, dpg)

BGM_status BGM_create_context(struct BGM_context *ctx, struct BGM_database_config *db_cfg);

void BGM_destroy_context(struct BGM_context *ctx);

BGM_status
BGM_add_template(struct BGM_context *ctx, struct BGM_template *tpl, unsigned int *tpl_id);

BGM_status
BGM_remove_template(struct BGM_context *ctx, struct BGM_template *tpl, unsigned int tpl_id);

BGM_status
BGM_identify(struct BGM_context *ctx, struct BGM_template *tpl, bool *found, unsigned int *tpl_id);

BGM_status BGM_template_from_xyt(char *xyt_buffer,
                                 struct BGM_template *tpl,
                                 unsigned int max_num_minutiae,
                                 unsigned int num_neighbors);

BGM_status BGM_template_from_raw_image(unsigned char *buffer,
                                       unsigned int width,
                                       unsigned int height,
                                       unsigned int pixel_depth,
                                       unsigned int resolution_in_ppi,
                                       struct BGM_template *tpl,
                                       unsigned int max_num_minutiae,
                                       unsigned int num_neighbors);

void BGM_destroy_template(struct BGM_template *tpl);

BGM_status BGM_set_match_tolerances(struct BGM_context *ctx,
                                    unsigned int x_tol,
                                    unsigned int y_tol,
                                    unsigned int angle_tol);

BGM_status BGM_set_match_thresholds(struct BGM_context *ctx,
                                    unsigned int minutia_threshold,
                                    unsigned int template_threshold,
                                    unsigned int failure_threshold);

BGM_status BGM_get_template_counter(struct BGM_context *ctx, unsigned int *tpl_counter);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BERGAMOTA_H
