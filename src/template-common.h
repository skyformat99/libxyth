/**
 * @file   template-common.h
 * @author rodrigo
 * @date   11/05/2015
 * @brief  Common template functions to be used internally.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEMPLATECOMMON_H
#define TEMPLATECOMMON_H

#include <bergamota.h>

void _BGM_reset_template(struct BGM_template *tpl);

BGM_status _BGM_intialize_template(struct BGM_template *tpl, unsigned int num_neighbors);

#endif // TEMPLATECOMMON_H
