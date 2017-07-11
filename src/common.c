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

#include "config.h"
#include <context.h>
#include <debug.h>

unsigned int XYTH_version_major = XYTH_VERSION_MAJOR;
unsigned int XYTH_version_minor = XYTH_VERSION_MINOR;

#define IS_IN_RANGE(value, min_val, max_val)                                   \
    ((int)(value) >= (int)(min_val) && (int)(value) <= (int)(max_val))

void _XYTH_calc_num_groups(struct XYTH_context *ctx, unsigned int *x_groups,
                           unsigned int *y_groups, unsigned int *t_groups)
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

XYTH_status _XYTH_calc_group_index(struct XYTH_context *ctx, int x, int y,
                                   unsigned int t, unsigned int *group_index)
{
    XYTH_status status;
    unsigned int x_groups, y_groups, t_groups;
    unsigned int x_block_size, y_block_size;
    int x_comp, y_comp, t_comp;

    if (IS_IN_RANGE(x, -ctx->db_cfg.max_x, ctx->db_cfg.max_x) &&
        IS_IN_RANGE(y, -ctx->db_cfg.max_y, ctx->db_cfg.max_y) &&
        IS_IN_RANGE(t, 0, 359)) {
        _XYTH_calc_num_groups(ctx, &x_groups, &y_groups, &t_groups);

        x_block_size = y_groups * t_groups;
        y_block_size = t_groups;

        x_comp = x_block_size *
                 ((ctx->db_cfg.max_x + x) / ctx->db_cfg.pixels_per_group);
        y_comp = y_block_size *
                 ((ctx->db_cfg.max_y + y) / ctx->db_cfg.pixels_per_group);
        t_comp = t / ctx->db_cfg.degrees_per_group;

        *group_index = x_comp + y_comp + t_comp;
        status = XYTH_SUCCESS;
    } else {
        PDEBUG("x: %d, min_x: %d, max_x: %d\n", x, -ctx->db_cfg.max_x,
               ctx->db_cfg.max_x);
        PDEBUG("y: %d, min_y: %d, max_y: %d\n", y, -ctx->db_cfg.max_y,
               ctx->db_cfg.max_y);
        PRINT_IF_TRUE(!IS_IN_RANGE(x, -ctx->db_cfg.max_x, ctx->db_cfg.max_x));
        PRINT_IF_TRUE(!IS_IN_RANGE(y, -ctx->db_cfg.max_y, ctx->db_cfg.max_y));
        PRINT_IF_TRUE(!IS_IN_RANGE(t, 0, 359));
        status = XYTH_E_VALUE_OUT_OF_RANGE;
    }

    PRINT_IF_ERROR(status);
    return status;
}
