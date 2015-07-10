/**
 * @file   common.c
 * @author rodrigo
 * @date   26/03/2015
 * @brief  Common functions.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <context.h>
#include <debug.h>
#include "config.h"

unsigned int BGM_version_major = BGM_VERSION_MAJOR;
unsigned int BGM_version_minor = BGM_VERSION_MINOR;

#define IS_IN_RANGE(value, min_val, max_val) \
    ((int)(value) >= (int)(min_val) && (int)(value) <= (int)(max_val))

void _BGM_calc_num_groups(
        struct BGM_context *ctx,
        unsigned int       *x_groups,
        unsigned int       *y_groups,
        unsigned int       *t_groups)
{
    unsigned int x_range_values, y_range_values, t_range_values;

    x_range_values = (2 * ctx->db_cfg.max_x) + 1;
    *x_groups = x_range_values / ctx->db_cfg.pixels_per_group;
    *x_groups += x_range_values % ctx->db_cfg.pixels_per_group > 0 ? 1 : 0;

    y_range_values = (2 * ctx->db_cfg.max_y) + 1;
    *y_groups = y_range_values / ctx->db_cfg.pixels_per_group;
    *y_groups += y_range_values % ctx->db_cfg.pixels_per_group > 0 ? 1 : 0;

    t_range_values = (DB_MAX_T_ANGLE_DFL + 1);
    *t_groups = t_range_values / ctx->db_cfg.degrees_per_group;
    *t_groups += t_range_values % ctx->db_cfg.degrees_per_group > 0 ? 1 : 0;
}


BGM_status _BGM_calc_group_index (struct BGM_context *ctx,
                                  int                 x,
                                  int                 y,
                                  unsigned int        t,
                                  unsigned int       *group_index)
{
    BGM_status   status;
    unsigned int x_groups, y_groups, t_groups;
    unsigned int x_block_size, y_block_size;
    int x_comp, y_comp, t_comp;


    if (IS_IN_RANGE(x, -ctx->db_cfg.max_x, ctx->db_cfg.max_x) &&
        IS_IN_RANGE(y, -ctx->db_cfg.max_y, ctx->db_cfg.max_y) &&
        IS_IN_RANGE(t, 0, 359)) {
        _BGM_calc_num_groups(ctx, &x_groups, &y_groups, &t_groups);

        x_block_size = y_groups * t_groups;
        y_block_size = t_groups;

        x_comp = x_block_size * ((ctx->db_cfg.max_x + x) / ctx->db_cfg.pixels_per_group);
        y_comp = y_block_size * ((ctx->db_cfg.max_y + y) / ctx->db_cfg.pixels_per_group);
        t_comp = t / ctx->db_cfg.degrees_per_group;

        *group_index = x_comp + y_comp + t_comp;
        status = BGM_SUCCESS;
    } else {
        PDEBUG("x: %d, min_x: %d, max_x: %d\n", x, -ctx->db_cfg.max_x, ctx->db_cfg.max_x);
        PDEBUG("y: %d, min_y: %d, max_y: %d\n", y, -ctx->db_cfg.max_y, ctx->db_cfg.max_y);
        PRINT_IF_TRUE(!IS_IN_RANGE(x, -ctx->db_cfg.max_x, ctx->db_cfg.max_x));
        PRINT_IF_TRUE(!IS_IN_RANGE(y, -ctx->db_cfg.max_y, ctx->db_cfg.max_y));
        PRINT_IF_TRUE(!IS_IN_RANGE(t, 0, 359));
        status = BGM_E_VALUE_OUT_OF_RANGE;
    }

    PRINT_IF_ERROR(status);
    return status;
}
