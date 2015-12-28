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

#define _BGM_MAX_MATCHES 100

struct _BGM_global_score {
    // minutiae
    unsigned int *minutiae_scores;
    unsigned int num_minutiae_scores;
    // templates
    unsigned int *template_scores;
    unsigned int num_template_scores;
    // result
    unsigned int num_matches;
    unsigned int matches[_BGM_MAX_MATCHES];
};

//
// Given a linear value and its tolerance, calculates a range of values.
// - 'range_begin' and 'range_end' are included in the range.
//
static void
_BGM_calculate_linear_range(int value,
                            unsigned int tolerance,
                            int min_possible_value,
                            int max_possible_value,
                            int *range_begin,
                            int *range_end)
{
    *range_begin = value - tolerance;
    if (*range_begin < min_possible_value) {
        *range_begin = min_possible_value;
    }

    *range_end = value + tolerance;
    if (*range_end > max_possible_value) {
        *range_end = max_possible_value;
    }
}

//
// Given an angular value and its tolerance, calculates a range of values.
// - 'range_begin' and 'range_end' are part of the range.
// - 'range_end' may be greater than 360.
//
static void
_BGM_define_angular_range(unsigned int value,
                          unsigned int tolerance,
                          unsigned int *range_begin,
                          unsigned int *range_end)
{
    int temp_begin;

    temp_begin = value - tolerance;
    if (temp_begin < 0) {
        temp_begin += 360;
    }

    *range_begin = temp_begin;

    // Don't care whether the angle is greater than 360.
    *range_end = value + tolerance;
    if (*range_end < *range_begin) {
        *range_end += 360;
    }
}

static void
_BGM_reset_minutiae_scores(struct _BGM_global_score *score)
{
    memset(score->minutiae_scores,
           0,
           score->num_minutiae_scores * sizeof(unsigned int));
}

static void
_BGM_reset_template_scores(struct _BGM_global_score *score)
{
    memset(score->template_scores,
           0,
           score->num_template_scores * sizeof(unsigned int));

    score->num_matches = 0;
}

//
// Creates and initializes a score structure.
//
static BGM_status
_BGM_create_score(struct BGM_context *context, struct _BGM_global_score *score)
{
    BGM_status status;
    score->num_minutiae_scores =
        context->db.next_template_id * MAX_MINUTIAE_PER_TEMPLATE;

    score->num_template_scores = context->db.next_template_id;

    score->minutiae_scores =
        malloc(score->num_minutiae_scores * sizeof(unsigned int));

    if (score->minutiae_scores != NULL) {
        _BGM_reset_minutiae_scores(score);
        score->template_scores =
            malloc(score->num_template_scores * sizeof(unsigned int));

        if (score->template_scores != NULL) {
            _BGM_reset_template_scores(score);
            status = BGM_SUCCESS;
        } else {
            status = BGM_E_NO_MEMORY;
        }

        if (status != BGM_SUCCESS) {
            free(score->minutiae_scores);
        }
    } else {
        status = BGM_E_NO_MEMORY;
    }

    return status;
}

//
// Destroys a score structure.
//
static void
_BGM_destroy_score(struct _BGM_global_score *score)
{
    if (score->minutiae_scores != NULL) {
        free(score->minutiae_scores);
    }

    if (score->template_scores != NULL) {
        free(score->template_scores);
    }
}

//
// Given a neighbor, calculates the ranges for X, Y, and angle.
//
static void
_BGM_calc_compatibility_window(struct BGM_context *context,
                               struct _BGM_neighbor *neighbor,
                               int *x_begin,
                               int *x_end,
                               int *y_begin,
                               int *y_end,
                               unsigned int *angle_begin,
                               unsigned int *angle_end)
{
    _BGM_calculate_linear_range(neighbor->relative_x,
                                context->match_cfg.x_tolerance,
                                -(context->db_cfg.max_x),
                                context->db_cfg.max_x,
                                x_begin,
                                x_end);

    _BGM_calculate_linear_range(neighbor->relative_y,
                                context->match_cfg.y_tolerance,
                                -(context->db_cfg.max_y),
                                context->db_cfg.max_y,
                                y_begin,
                                y_end);

    _BGM_define_angular_range(neighbor->relative_angle,
                              context->match_cfg.t_tolerance,
                              angle_begin,
                              angle_end);
}

//
// Computes one point (+1) in the score for each minutia referenced by the group
// associated with 'group_index'.
//
static void
_BGM_update_minutia_score(struct BGM_context *context,
                          struct _BGM_global_score *score,
                          unsigned int group_index)
{
    unsigned int *group;
    unsigned int group_length;
    unsigned int position = 0;
    unsigned int global_minutia_index;

    group_length = context->db.alloc_counter[group_index];
    if (group_length > 0) {
        group = context->db.data[group_index];
        while (group[position] != (unsigned int)-1 && position < group_length) {
            // The value in group[position] is a combination of
            // template/minutia, and it can be used directly as an index in
            // 'minutiae_scores'.
            global_minutia_index = group[position];
            score->minutiae_scores[global_minutia_index]++;
            position++;
        }
    }
}

//
// Finds minutiae that are compatible with 'neighbor', then updates the score
// structure accordingly.
//
static void
_BGM_find_matching_minutiae(struct BGM_context *context,
                            struct _BGM_neighbor *neighbor,
                            struct _BGM_global_score *score)
{
    int x_begin;
    int x_end;
    int y_begin;
    int y_end;
    unsigned int angle_begin;
    unsigned int angle_end;
    BGM_status status;

    _BGM_calc_compatibility_window(context,
                                   neighbor,
                                   &x_begin,
                                   &x_end,
                                   &y_begin,
                                   &y_end,
                                   &angle_begin,
                                   &angle_end);

    for (int x = x_begin; x <= x_end; x += context->db_cfg.pixels_per_group) {
        for (int y = y_begin; y <= y_end;
             y += context->db_cfg.pixels_per_group) {
            for (unsigned int t = angle_begin; t <= angle_end;
                 t += context->db_cfg.degrees_per_group) {
                unsigned int group_index;
                status =
                    _BGM_calc_group_index(context, x, y, t % 360, &group_index);
                if (status == BGM_SUCCESS) {
                    _BGM_update_minutia_score(context, score, group_index);
                }
            }
        }
    }
}

// Given a score structure, which contains minutiae's scores, creates a list of
// candidates.
static void
_BGM_calculate_templates_score(struct BGM_context *context,
                               struct _BGM_global_score *score)
{
    for (unsigned int i = 0; i < score->num_minutiae_scores; i++) {
        if (score->minutiae_scores[i] >= context->match_cfg.minutia_threshold) {
            unsigned int template_index = i / MAX_MINUTIAE_PER_TEMPLATE;
            score->template_scores[template_index]++;
        }
    }
}

static void
_BGM_sort_matches_list(struct _BGM_global_score *score)
{
    bool swapped;
    unsigned int n = score->num_matches;

    do {
        swapped = false;
        for (unsigned int i = 1; i < n; i++) {
            if (score->template_scores[score->matches[i]]
                > score->template_scores[score->matches[i - 1]]) {
                unsigned int tmp = score->matches[i - 1];
                score->matches[i - 1] = score->matches[i];
                score->matches[i] = tmp;
                swapped = true;
            }
        }
        n--;
    } while (swapped);
}

static void
_BGM_compile_matches_list(struct BGM_context *context,
                          struct _BGM_global_score *score)
{
    for (unsigned int i = 0; i < score->num_template_scores; i++) {
        if (score->template_scores[i]
            >= context->match_cfg.template_threshold) {
            score->matches[score->num_matches++] = i;
        }
    }

    _BGM_sort_matches_list(score);
}

static BGM_status
_BGM_identify(struct BGM_context *ctx,
              struct BGM_template *tpl,
              unsigned int *num_matches,
              unsigned int *matches)
{
    BGM_status status;
    struct _BGM_global_score score;

    status = _BGM_create_score(ctx, &score);

    if (status == BGM_SUCCESS) {
        for (unsigned int min_index = 0; min_index < tpl->num_minutiae;
             min_index++) {
            for (unsigned int nei_index = 0;
                 nei_index < tpl->minutiae[min_index].num_neighbors;
                 nei_index++) {
                _BGM_find_matching_minutiae(ctx,
                                            &tpl->minutiae[min_index]
                                                 .neighbors[nei_index],
                                            &score);
            }
            _BGM_calculate_templates_score(ctx, &score);
            _BGM_reset_minutiae_scores(&score);
        }
        _BGM_compile_matches_list(ctx, &score);

        if (score.num_matches < *num_matches) {
            *num_matches = score.num_matches;
        }
        memcpy(matches, score.matches, *num_matches * sizeof(matches[0]));
        _BGM_destroy_score(&score);
    }

    PRINT_IF_ERROR(status);
    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BGM_status
BGM_identify(struct BGM_context *ctx,
             struct BGM_template *tpl,
             unsigned int *num_ids,
             unsigned int *ids)
{
    BGM_status status;

    if (ctx == NULL || tpl == NULL || num_ids == NULL || ids == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl);
        PRINT_IF_NULL(num_ids);
        PRINT_IF_NULL(ids);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        if (_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
            status = _BGM_identify(ctx, tpl, num_ids, ids);
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
