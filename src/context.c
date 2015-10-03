/**
 * @file   match_context.c
 * @author rodrigo
 * @date   04/02/2015
 * @brief  Functions to Create/Destroy context.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>

#include <bergamota.h>
#include <context.h>
#include <template.h>
#include <config.h>
#include <debug.h>

#include "common.h"

static void
_BGM_set_dfl_match_config(struct _BGM_match_config *cfg)
{
    cfg->failure_threshold = MATCH_FAILURE_THRESHOLD_DFL;
    cfg->minutia_threshold = MATCH_MINUTIA_THRESHOLD_DFL;
    cfg->template_threshold = MATCH_TEMPLATE_THRESHOLD_DFL;
    cfg->t_tolerance = MATCH_T_TOLERANCE_DFL;
    cfg->x_tolerance = MATCH_X_TOLERANCE_DFL;
    cfg->y_tolerance = MATCH_Y_TOLERANCE_DFL;
}

static void
_BGM_set_dfl_database_config(struct BGM_database_config *db_cfg)
{
    db_cfg->degrees_per_group = DB_DEGREES_PER_GROUP_DFL;
    db_cfg->max_x = DB_MAX_X_COORD_DFL;
    db_cfg->max_y = DB_MAX_Y_COORD_DFL;
    db_cfg->pixels_per_group = DB_PIXELS_PER_GROUP_DFL;
    db_cfg->alloc_step = DB_ALLOC_STEP_DFL;
}

static BGM_status
_BGM_set_custom_database_config(struct BGM_database_config *in, struct BGM_database_config *out)
{
    BGM_status status;

    if (in->degrees_per_group < 360 && in->max_x > 0 && in->max_y > 0 && in->pixels_per_group > 0
        && in->alloc_step > 0) {
        out->degrees_per_group = in->degrees_per_group;
        out->max_x = in->max_x;
        out->max_y = in->max_y;
        out->pixels_per_group = in->pixels_per_group;
        out->alloc_step = in->alloc_step;

        status = BGM_SUCCESS;
    } else {
        status = BGM_E_INVALID_CONFIGURATION;
    }

    return status;
}

static BGM_status
_BGM_create_database(struct BGM_context *ctx)
{
    BGM_status status;
    unsigned int num_groups;
    unsigned int x_groups, y_groups, t_groups;

    ctx->db.next_template_id = 0;
    // TODO: Create a macro or function to get templates_counter.
    ctx->db.templates_counter = 0;

    if (ctx->db_cfg.degrees_per_group != 0 && ctx->db_cfg.pixels_per_group != 0) {
        _BGM_calc_num_groups(ctx, &x_groups, &y_groups, &t_groups);
        num_groups = x_groups * y_groups * t_groups;
        // Each group will hold a pointer
        ctx->db.data = calloc(num_groups, sizeof(int *));
        if (ctx->db.data != NULL) {
            // For each group, we have counter to control memory allocations
            ctx->db.alloc_counter = calloc(num_groups, sizeof(unsigned int));
            if (ctx->db.alloc_counter != NULL) {
                ctx->db.num_groups = num_groups;
                PDEBUG("num_groups: %d\n", num_groups);
                status = BGM_SUCCESS;
            } else {
                free(ctx->db.data);
                status = BGM_E_NO_MEMORY;
            }
        }
    } else {
        PRINT_IF_TRUE(ctx->db_cfg.degrees_per_group == 0);
        PRINT_IF_TRUE(ctx->db_cfg.pixels_per_group == 0);
        status = BGM_E_INVALID_CONFIGURATION;
    }

    PRINT_IF_ERROR(status);
    return status;
}

static void
_BGM_destroy_database(struct BGM_context *ctx)
{
    if (ctx->db.alloc_counter != NULL) {
        if (ctx->db.data != NULL) {
            PDEBUG("ctx->db_cfg.num_groups: %d\n", ctx->db.num_groups);
            for (unsigned int i = 0; i < ctx->db.num_groups; i++) {
                if (ctx->db.alloc_counter[i] > 0) {
                    free(ctx->db.data[i]);
                }
            }
            free(ctx->db.data);
            ctx->db.data = NULL;
        }
        free(ctx->db.alloc_counter);
        ctx->db.alloc_counter = NULL;
    } else {
        PRINT_IF_NULL(ctx->db.alloc_counter);
    }
}

BGM_status
BGM_set_match_tolerances(struct BGM_context *ctx,
                         unsigned int x_tol,
                         unsigned int y_tol,
                         unsigned int angle_tol)
{
    BGM_status status;

    if (ctx == NULL) {
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_NULL(ctx);
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        ctx->match_cfg.x_tolerance = x_tol;
        ctx->match_cfg.y_tolerance = y_tol;
        ctx->match_cfg.t_tolerance = angle_tol;
        status = BGM_SUCCESS;
    } else {
        status = BGM_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}

BGM_status
BGM_set_match_thresholds(struct BGM_context *ctx,
                         unsigned int minutia_threshold,
                         unsigned int template_threshold,
                         unsigned int failure_threshold)
{
    BGM_status status;

    if (ctx == NULL) {
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_NULL(ctx);
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        ctx->match_cfg.minutia_threshold = minutia_threshold;
        ctx->match_cfg.template_threshold = template_threshold;
        ctx->match_cfg.failure_threshold = failure_threshold;
        status = BGM_SUCCESS;
    } else {
        status = BGM_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}

BGM_status
BGM_create_context(struct BGM_context *ctx, struct BGM_database_config *db_cfg)
{
    BGM_status status;

    if (ctx == NULL) {
        PRINT_IF_NULL(ctx);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (!_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        _BGM_set_dfl_match_config(&ctx->match_cfg);

        if (db_cfg != NULL) {
            status = _BGM_set_custom_database_config(db_cfg, &ctx->db_cfg);
        } else {
            _BGM_set_dfl_database_config(&ctx->db_cfg);
            status = BGM_SUCCESS;
        }
        if (status == BGM_SUCCESS) {
            status = _BGM_create_database(ctx);
            if (status == BGM_SUCCESS) {
                ctx->magic_number = _BGM_CONTEXT_INIT_MAGIC_NUMBER;
            }
        }
    } else {
        status = BGM_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}

void
BGM_destroy_context(struct BGM_context *ctx)
{
    if (ctx != NULL) {
        if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
            _BGM_destroy_database(ctx);
            ctx->magic_number = 0;
        } else {
            PRINT_IF_TRUE(ctx->magic_number != _BGM_CONTEXT_INIT_MAGIC_NUMBER);
        }
    } else {
        PRINT_IF_NULL(ctx);
    }
}

BGM_status
BGM_get_template_counter(struct BGM_context *ctx, unsigned int *tpl_counter)
{
    BGM_status status;

    if (ctx == NULL || tpl_counter == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl_counter);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        *tpl_counter = ctx->db.templates_counter;
        status = BGM_SUCCESS;
    } else {
        status = BGM_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
