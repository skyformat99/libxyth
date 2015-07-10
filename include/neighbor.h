/**
 * @file   neighbor.h
 * @author rodrigo
 * @date   13/10/2014
 * @brief  Neighbor structure declaration.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#ifndef NEIGHBOR_H
#define NEIGHBOR_H

/**
 * The relative location of a minutia's neighbor, which is another minutia.
 */
struct _BGM_neighbor {
	int relative_x;
	int relative_y;
	unsigned int relative_angle;
	unsigned int neighbor_id;
};

#endif // NEIGHBOR_H
