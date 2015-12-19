/**
 * @file   template.h
 * @author rodrigo
 * @date   13/10/2014
 * @brief  Template structure declaration.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <minutia.h>

#define _BGM_TEMPLATE_INIT_MAGIC_NUMBER 0x004D4742

/**
 * A fingerprint template which is a collection of minutiae
 */
struct BGM_template {
    unsigned int magic_num;
    unsigned int num_minutiae;
    struct _BGM_minutia *minutiae;
};

#define _BGM_IS_TEMPLATE_INITIALIZED(tpl) \
    ((tpl).magic_num == _BGM_TEMPLATE_INIT_MAGIC_NUMBER)

#endif // TEMPLATE_H
