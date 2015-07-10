/**
 * @file   minutia.h
 * @author rodrigo
 * @date   13/10/2014
 * @brief  Minutia structure declaration
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#ifndef MINUTIA_H
#define MINUTIA_H

#include <neighbor.h>

/**
 * A minutia, i.e. ridge ending or bifurcation.
 */
struct _BGM_minutia {
	unsigned int id;
    unsigned int x;
    unsigned int y;
    unsigned int angle;
    unsigned int num_neighbors;
    struct _BGM_neighbor *neighbors;
};

#endif // MINUTIA_H
