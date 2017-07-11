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

#include <stdlib.h>
#include <string.h>

#include <context.h>
#include <debug.h>
#include <template.h>
#include <xyth.h>

#include "common.h"
#include "config.h"

static XYTH_status
_XYTH_add_positions(struct XYTH_context *ctx, unsigned int group_index)
{
    XYTH_status status;

    if (ctx->db.alloc_counter[group_index] == 0) {
        ctx->db.data[group_index] =
            malloc(ctx->db_cfg.alloc_step * sizeof(unsigned int));
        if (ctx->db.data[group_index] != NULL) {
            ctx->db.alloc_counter[group_index] = ctx->db_cfg.alloc_step;
            memset(ctx->db.data[group_index],
                   -1,
                   ctx->db_cfg.alloc_step * sizeof(unsigned int));
            status = XYTH_SUCCESS;
        } else {
            status = XYTH_E_NO_MEMORY;
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
            status = XYTH_SUCCESS;
        } else {
            status = XYTH_E_NO_MEMORY;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static XYTH_status
_XYTH_add_neighbor(struct XYTH_context *ctx,
                   struct _XYTH_neighbor *nei,
                   unsigned int tpl_id,
                   unsigned int min_id)
{
    XYTH_status status;
    unsigned int group_index;

    status = _XYTH_calc_group_index(ctx,
                                    nei->relative_x,
                                    nei->relative_y,
                                    nei->relative_angle,
                                    &group_index);
    if (status == XYTH_SUCCESS) {
        if (ctx->db.alloc_counter[group_index] == 0) {
            status = _XYTH_add_positions(ctx, group_index);
        }

        if (status == XYTH_SUCCESS) {
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

            status = XYTH_SUCCESS;
            if (position >= ctx->db.alloc_counter[group_index]) {
                status = _XYTH_add_positions(ctx, group_index);
            }

            if (status == XYTH_SUCCESS) {
                ctx->db.data[group_index][position] =
                    (tpl_id * MAX_MINUTIAE_PER_TEMPLATE) + min_id;
            }
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static XYTH_status
_XYTH_remove_neighbor(struct XYTH_context *ctx,
                      struct _XYTH_neighbor *nei,
                      unsigned int tpl_id,
                      unsigned int min_id)
{
    XYTH_status status;
    unsigned int group_index;

    status = _XYTH_calc_group_index(ctx,
                                    nei->relative_x,
                                    nei->relative_y,
                                    nei->relative_angle,
                                    &group_index);
    if (status == XYTH_SUCCESS) {
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
                status = XYTH_E_NOT_FOUND;
            }
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

static XYTH_status
_XYTH_add_minutia(struct XYTH_context *ctx,
                  struct _XYTH_minutia *min,
                  unsigned int tpl_id)
{
    XYTH_status status = XYTH_SUCCESS;

    for (unsigned int i = 0; i < min->num_neighbors; i++) {
        status = _XYTH_add_neighbor(ctx, &min->neighbors[i], tpl_id, min->id);
        if (status != XYTH_SUCCESS) {
            for (unsigned int j = 0; j < i; j++) {
                XYTH_status debug_status;
                debug_status = _XYTH_remove_neighbor(ctx,
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

static XYTH_status
_XYTH_remove_minutia(struct XYTH_context *ctx,
                     struct _XYTH_minutia *min,
                     unsigned int tpl_id)
{
    XYTH_status status = XYTH_SUCCESS;
    int error_count = 0;

    for (unsigned int i = 0; i < min->num_neighbors; i++) {
        status =
            _XYTH_remove_neighbor(ctx, &min->neighbors[i], tpl_id, min->id);
        if (status != XYTH_SUCCESS) {
            error_count++;
        }
    }

    return (error_count == 0) ? XYTH_SUCCESS : XYTH_E_NOT_FOUND;
}

static XYTH_status
_XYTH_add_template(struct XYTH_context *ctx,
                   struct XYTH_template *tpl,
                   unsigned int *tpl_id)
{
    XYTH_status status = XYTH_E_TOO_FEW_MINUTIAE;

    for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
        status =
            _XYTH_add_minutia(ctx, &tpl->minutiae[i], ctx->db.next_template_id);
        if (status != XYTH_SUCCESS) {
            for (unsigned int j = 0; j < i; j++) {
                XYTH_status debug_status;
                debug_status = _XYTH_remove_minutia(ctx,
                                                    &tpl->minutiae[j],
                                                    ctx->db.next_template_id);
                PRINT_IF_ERROR(debug_status);
            }
            break;
        }
    }

    if (status == XYTH_SUCCESS) {
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

XYTH_status
XYTH_add_template(struct XYTH_context *ctx,
                  struct XYTH_template *tpl,
                  unsigned int *tpl_id)
{
    XYTH_status status;

    if (ctx == NULL || tpl == NULL || tpl_id == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl);
        PRINT_IF_NULL(tpl_id);
        status = XYTH_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_XYTH_IS_CONTEXT_INITIALIZED(*ctx)) {
        if (_XYTH_IS_TEMPLATE_INITIALIZED(*tpl)) {
            status = _XYTH_add_template(ctx, tpl, tpl_id);
        } else {
            PERROR("template not initialized\n");
            status = XYTH_E_NOT_INITIALIZED;
        }
    } else {
        PERROR("context not initialized\n");
        status = XYTH_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}

XYTH_status
_XYTH_remove_template(struct XYTH_context *ctx,
                      struct XYTH_template *tpl,
                      unsigned int tpl_id)
{
    XYTH_status status;
    unsigned int removed_minutiae = 0;

    for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
        status = _XYTH_remove_minutia(ctx, &tpl->minutiae[i], tpl_id);
        if (status == XYTH_SUCCESS) {
            removed_minutiae++;
        }
    }

    if (removed_minutiae == tpl->num_minutiae) {
        if (removed_minutiae > 0) {
            ctx->db.templates_counter--;
            status = XYTH_SUCCESS;
        } else {
            status = XYTH_E_TOO_FEW_MINUTIAE;
        }
    } else {
        if (removed_minutiae > 0) {
            status = XYTH_E_INCOMPLETE_REMOVAL;
        } else {
            status = XYTH_E_NOT_FOUND;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

XYTH_status
XYTH_remove_template(struct XYTH_context *ctx,
                     struct XYTH_template *tpl,
                     unsigned int tpl_id)
{
    XYTH_status status;

    if (ctx == NULL || tpl == NULL) {
        PRINT_IF_NULL(ctx);
        PRINT_IF_NULL(tpl);
        status = XYTH_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (_XYTH_IS_CONTEXT_INITIALIZED(*ctx)) {
        if (_XYTH_IS_TEMPLATE_INITIALIZED(*tpl)) {
            status = _XYTH_remove_template(ctx, tpl, tpl_id);
        } else {
            PERROR("template not initialized\n");
            status = XYTH_E_NOT_INITIALIZED;
        }
    } else {
        PERROR("context not initialized\n");
        status = XYTH_E_NOT_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
