/**
 * @file   identify.c
 * @author rodrigo
 * @date   07/04/2015
 * @brief  Implementation of BGM_identify().
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <bergamota.h>
#include <context.h>
#include <debug.h>

#include "config.h"
#include "common.h"

struct _BGM_global_score {
    unsigned int num_templates;
    unsigned int *minutiae_scores;
    unsigned int winner_id;
};

static void
_BGM_define_coord_range(int min,
                        int max,
                        int val,
                        unsigned int tolerance,
                        int *start,
                        int *end)
{
    *start = val - tolerance;
    if (*start < min) {
        *start = min;
    }

    *end = val + tolerance;
    if (*end > max) {
        *end = max;
    }
}

static void
_BGM_define_angle_range(unsigned int val,
                        unsigned int tolerance,
                        unsigned int *begin,
                        unsigned int *end)
{
    int temp_begin;

    temp_begin = val - tolerance;
    if (temp_begin < 0) {
        temp_begin += 360;
    }

    *begin = temp_begin;

    // Don't care if the angle is greater than 360, fix it later
    *end = val + tolerance;
    if (*end < *begin) {
        *end += 360;
    }
}

static void
_BGM_reset_score(struct _BGM_global_score *score)
{
    memset(score->minutiae_scores,
           0,
           score->num_templates * MAX_MINUTIAE_PER_TEMPLATE_DFL
               * sizeof(unsigned int));
}

static BGM_status
_BGM_create_score(struct BGM_context *ctx, struct _BGM_global_score *score)
{
    BGM_status status;
    unsigned int alloc_size;

    // TODO: Stop using MAX_MINUTIAE_PER_TEMPLATE_DFL directly.
    alloc_size = ctx->db.next_template_id * MAX_MINUTIAE_PER_TEMPLATE_DFL
        * sizeof(unsigned int);

    score->minutiae_scores = malloc(alloc_size);
    if (score->minutiae_scores != NULL) {
        score->num_templates = ctx->db.next_template_id;
        score->winner_id = BGM_RESERVED_TEMPLATE_ID;
        _BGM_reset_score(score);
        status = BGM_SUCCESS;
    } else {
        status = BGM_E_NO_MEMORY;
    }

    return status;
}

static void
_BGM_destroy_score(struct _BGM_global_score *score)
{
    free(score->minutiae_scores);
}

static void
_BGM_calc_compatibility_window(struct BGM_context *ctx,
                               struct _BGM_neighbor *nei,
                               int *start_x,
                               int *end_x,
                               int *start_y,
                               int *end_y,
                               unsigned int *start_t,
                               unsigned int *end_t)
{
    _BGM_define_coord_range(-(ctx->db_cfg.max_x),
                            ctx->db_cfg.max_x,
                            nei->relative_x,
                            ctx->match_cfg.x_tolerance,
                            start_x,
                            end_x);

    _BGM_define_coord_range(-(ctx->db_cfg.max_y),
                            ctx->db_cfg.max_y,
                            nei->relative_y,
                            ctx->match_cfg.y_tolerance,
                            start_y,
                            end_y);

    _BGM_define_angle_range(nei->relative_angle,
                            ctx->match_cfg.t_tolerance,
                            start_t,
                            end_t);
}

static void
_BGM_update_score_from_group(struct BGM_context *ctx,
                             struct _BGM_global_score *sc,
                             unsigned int group_index)
{
    unsigned int *group;
    unsigned int group_size;
    unsigned int position = 0;
    unsigned int score_index;

    group_size = ctx->db.alloc_counter[group_index];
    if (group_size > 0) {
        group = ctx->db.data[group_index];
        while (group[position] != (unsigned int)-1 && position < group_size) {
            score_index = group[position];
            sc->minutiae_scores[score_index]++;
            position++;
        }
    }
}

static void
_BGM_update_score_with_matching_neighbors(struct BGM_context *ctx,
                                          struct _BGM_neighbor *nei,
                                          struct _BGM_global_score *score)
{
    int start_x;
    int end_x;
    int start_y;
    int end_y;
    unsigned int start_t;
    unsigned int end_t;
    BGM_status status;

    _BGM_calc_compatibility_window(ctx,
                                   nei,
                                   &start_x,
                                   &end_x,
                                   &start_y,
                                   &end_y,
                                   &start_t,
                                   &end_t);

    for (int x = start_x; x <= end_x; x += ctx->db_cfg.pixels_per_group) {
        for (int y = start_y; y <= end_y; y += ctx->db_cfg.pixels_per_group) {
            for (unsigned int t = start_t; t <= end_t;
                 t += ctx->db_cfg.degrees_per_group) {
                unsigned int group_index;
                status =
                    _BGM_calc_group_index(ctx, x, y, t % 360, &group_index);
                if (status == BGM_SUCCESS) {
                    _BGM_update_score_from_group(ctx, score, group_index);
                }
            }
        }
    }
}

static void
_BGM_get_matching_id(struct BGM_context *ctx,
                     struct _BGM_global_score *score,
                     unsigned int *tpl_id)
{
    unsigned int *template_scores;
    unsigned int alloc_size;
    unsigned int temp_id;

    alloc_size = score->num_templates * sizeof(*template_scores);

    template_scores = malloc(alloc_size);
    if (template_scores != NULL) {
        memset(template_scores, 0, alloc_size);
        for (unsigned int i = 0;
             i < score->num_templates * MAX_MINUTIAE_PER_TEMPLATE_DFL;
             i++) {
            if (score->minutiae_scores[i] != 0) {
                if (score->minutiae_scores[i]
                    >= ctx->match_cfg.minutia_threshold) {
                    template_scores[i / MAX_MINUTIAE_PER_TEMPLATE_DFL]++;
                }
            }
        }
        temp_id = BGM_RESERVED_TEMPLATE_ID;
        for (unsigned int i = 0; i < score->num_templates; i++) {
            if (template_scores[i] != 0) {
                if (temp_id == BGM_RESERVED_TEMPLATE_ID) {
                    temp_id = i;
                } else {
                    if (template_scores[i] > template_scores[temp_id]) {
                        temp_id = i;
                    }
                }
            }
        }
        *tpl_id = temp_id;
        free(template_scores);
    } else {
        *tpl_id = BGM_RESERVED_TEMPLATE_ID;
    }
}

static BGM_status
_BGM_identify(struct BGM_context *ctx,
              struct BGM_template *tpl,
              bool *found,
              unsigned int *tpl_id)
{
    BGM_status status;
    struct _BGM_global_score score;

    *tpl_id = BGM_RESERVED_TEMPLATE_ID;
    *found = false;

    status = _BGM_create_score(ctx, &score);

    if (status == BGM_SUCCESS) {
        for (unsigned int min_index = 0; min_index < tpl->num_minutiae;
             min_index++) {
            for (unsigned int nei_index = 0;
                 nei_index < tpl->minutiae[min_index].num_neighbors;
                 nei_index++) {
                _BGM_update_score_with_matching_neighbors(
                    ctx,
                    &tpl->minutiae[min_index].neighbors[nei_index],
                    &score);
            }
            _BGM_get_matching_id(ctx, &score, tpl_id);
            _BGM_reset_score(&score);
            if (*tpl_id != BGM_RESERVED_TEMPLATE_ID) {
                *found = true;
                goto MATCH_FOUND;
            }
        }
    }

MATCH_FOUND:

    _BGM_destroy_score(&score);

    PRINT_IF_ERROR(status);
    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BGM_status
BGM_identify(struct BGM_context *ctx,
             struct BGM_template *tpl,
             bool *found,
             unsigned int *tpl_id)
{
    BGM_status status;

    if (ctx == NULL || tpl == NULL || tpl_id == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl);
        PRINT_IF_NULL(tpl_id);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        if (_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
            status = _BGM_identify(ctx, tpl, found, tpl_id);
        } else {
            PERROR("template not initialized\n");
            status = BGM_E_NOT_INITIALIZED;
        }
    } else {
        PERROR("context not initialized\n");
        status = BGM_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
