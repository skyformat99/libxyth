/**
 * @file   add.c
 * @author rodrigo
 * @date   07/04/2015
 * @brief  Functions to Add/Remove templates to/from context.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <string.h>
#include <stdlib.h>

#include <bergamota.h>
#include <context.h>
#include <template.h>
#include <debug.h>

#include "config.h"
#include "common.h"

static BGM_status
_BGM_add_positions(struct BGM_context *ctx, unsigned int group_index)
{
    BGM_status status;

    if (ctx->db.alloc_counter[group_index] == 0) {
        ctx->db.data[group_index] =
            malloc(ctx->db_cfg.alloc_step * sizeof(unsigned int));
        if (ctx->db.data[group_index] != NULL) {
            ctx->db.alloc_counter[group_index] = ctx->db_cfg.alloc_step;
            memset(ctx->db.data[group_index],
                   -1,
                   ctx->db_cfg.alloc_step * sizeof(unsigned int));
            status = BGM_SUCCESS;
        } else {
            status = BGM_E_NO_MEMORY;
        }
    } else {
        unsigned int *old_ptr;
        unsigned int old_alloc_counter;

        old_ptr = ctx->db.data[group_index];
        old_alloc_counter = ctx->db.alloc_counter[group_index];

        ctx->db.data[group_index] =
            malloc((ctx->db.alloc_counter[group_index] + ctx->db_cfg.alloc_step)
                   * sizeof(unsigned int));
        if (ctx->db.data[group_index] != NULL) {
            ctx->db.alloc_counter[group_index] += ctx->db_cfg.alloc_step;
            memcpy(ctx->db.data[group_index],
                   old_ptr,
                   old_alloc_counter * sizeof(unsigned int));
            free(old_ptr);
            memset(&ctx->db.data[group_index][old_alloc_counter],
                   -1,
                   ctx->db_cfg.alloc_step * sizeof(unsigned int));
            status = BGM_SUCCESS;
        } else {
            status = BGM_E_NO_MEMORY;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static BGM_status
_BGM_add_neighbor(struct BGM_context *ctx,
                  struct _BGM_neighbor *nei,
                  unsigned int tpl_id,
                  unsigned int min_id)
{
    BGM_status status;
    unsigned int group_index;

    status = _BGM_calc_group_index(ctx,
                                   nei->relative_x,
                                   nei->relative_y,
                                   nei->relative_angle,
                                   &group_index);
    if (status == BGM_SUCCESS) {
        if (ctx->db.alloc_counter[group_index] == 0) {
            status = _BGM_add_positions(ctx, group_index);
        }

        if (status == BGM_SUCCESS) {
            unsigned int position;
            // Find the first free position
            // TODO: It would be better to start at the group's top, going
            // downwards to find the
            // first
            //       free position.
            for (position = 0; position < ctx->db.alloc_counter[group_index]
                 && ctx->db.data[group_index][position] != (unsigned int)-1;
                 position++)
                ;

            status = BGM_SUCCESS;
            if (position >= ctx->db.alloc_counter[group_index]) {
                status = _BGM_add_positions(ctx, group_index);
            }

            if (status == BGM_SUCCESS) {
                ctx->db.data[group_index][position] =
                    (tpl_id * MAX_MINUTIAE_PER_TEMPLATE_DFL) + min_id;
            }
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static BGM_status
_BGM_remove_neighbor(struct BGM_context *ctx,
                     struct _BGM_neighbor *nei,
                     unsigned int tpl_id,
                     unsigned int min_id)
{
    BGM_status status;
    unsigned int group_index;

    status = _BGM_calc_group_index(ctx,
                                   nei->relative_x,
                                   nei->relative_y,
                                   nei->relative_angle,
                                   &group_index);
    if (status == BGM_SUCCESS) {
        if (ctx->db.alloc_counter[group_index] != 0) {
            unsigned int position;
            for (position = 0; position < ctx->db.alloc_counter[group_index]
                 && ctx->db.data[group_index][position]
                     != (tpl_id << 6) + min_id;
                 position++)
                ;

            if (position < ctx->db.alloc_counter[group_index]) {
                memmove(&ctx->db.data[group_index][position],
                        &ctx->db.data[group_index][position + 1],
                        (ctx->db.alloc_counter[group_index] - position + 1));
            } else {
                status = BGM_E_NOT_FOUND;
            }
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static BGM_status
_BGM_add_minutia(struct BGM_context *ctx,
                 struct _BGM_minutia *min,
                 unsigned int tpl_id)
{
    BGM_status status = BGM_SUCCESS;

    for (unsigned int i = 0; i < min->num_neighbors; i++) {
        status = _BGM_add_neighbor(ctx, &min->neighbors[i], tpl_id, min->id);
        if (status != BGM_SUCCESS) {
            for (unsigned int j = 0; j < i; j++) {
                BGM_status debug_status;
                debug_status = _BGM_remove_neighbor(ctx,
                                                    &min->neighbors[j],
                                                    tpl_id,
                                                    min->id);
                PRINT_IF_ERROR(debug_status);
            }
            break;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static BGM_status
_BGM_remove_minutia(struct BGM_context *ctx,
                    struct _BGM_minutia *min,
                    unsigned int tpl_id)
{
    BGM_status status = BGM_SUCCESS;
    int error_count = 0;

    for (unsigned int i = 0; i < min->num_neighbors; i++) {
        status = _BGM_remove_neighbor(ctx, &min->neighbors[i], tpl_id, min->id);
        if (status != BGM_SUCCESS) {
            error_count++;
        }
    }

    return (error_count == 0) ? BGM_SUCCESS : BGM_E_NOT_FOUND;
}

static BGM_status
_BGM_add_template(struct BGM_context *ctx,
                  struct BGM_template *tpl,
                  unsigned int *tpl_id)
{
    BGM_status status = BGM_E_TOO_FEW_MINUTIAE;

    for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
        status =
            _BGM_add_minutia(ctx, &tpl->minutiae[i], ctx->db.next_template_id);
        if (status != BGM_SUCCESS) {
            for (unsigned int j = 0; j < i; j++) {
                BGM_status debug_status;
                debug_status = _BGM_remove_minutia(ctx,
                                                   &tpl->minutiae[j],
                                                   ctx->db.next_template_id);
                PRINT_IF_ERROR(debug_status);
            }
            break;
        }
    }

    if (status == BGM_SUCCESS) {
        *tpl_id = ctx->db.next_template_id;
        ctx->db.next_template_id++;
        ctx->db.templates_counter++;
    }

    PRINT_IF_ERROR(status);
    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BGM_status
BGM_add_template(struct BGM_context *ctx,
                 struct BGM_template *tpl,
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
            status = _BGM_add_template(ctx, tpl, tpl_id);
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

BGM_status
_BGM_remove_template(struct BGM_context *ctx,
                     struct BGM_template *tpl,
                     unsigned int tpl_id)
{
    BGM_status status;
    unsigned int removed_minutiae = 0;

    for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
        status = _BGM_remove_minutia(ctx, &tpl->minutiae[i], tpl_id);
        if (status == BGM_SUCCESS) {
            removed_minutiae++;
        }
    }

    if (removed_minutiae == tpl->num_minutiae) {
        if (removed_minutiae > 0) {
            ctx->db.templates_counter--;
            status = BGM_SUCCESS;
        } else {
            status = BGM_E_TOO_FEW_MINUTIAE;
        }
    } else {
        if (removed_minutiae > 0) {
            status = BGM_E_INCOMPLETE_REMOVAL;
        } else {
            status = BGM_E_NOT_FOUND;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

BGM_status
BGM_remove_template(struct BGM_context *ctx,
                    struct BGM_template *tpl,
                    unsigned int tpl_id)
{
    BGM_status status;

    if (ctx == NULL || tpl == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl);
        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_BGM_IS_CONTEXT_INITIALIZED(*ctx)) {
        if (_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
            status = _BGM_remove_template(ctx, tpl, tpl_id);
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
