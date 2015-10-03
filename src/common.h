/**
 * @file   common.h
 * @author rodrigo
 * @date   26/03/2015
 * @brief  Common functions.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef COMMON_H
#define COMMON_H

#include <context.h>

int _BGM_calc_group_index(struct BGM_context *ctx,
                          int x,
                          int y,
                          unsigned int t,
                          unsigned int *group_index);

void _BGM_calc_num_groups(struct BGM_context *ctx,
                          unsigned int *x_groups,
                          unsigned int *y_groups,
                          unsigned int *t_groups);

#endif // COMMON_H
