/**
 * @file   config.h
 * @author rodrigo
 * @date   19/02/2015
 * @brief  Default values for bergamota's configuration.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef CONFIG_H
#define CONFIG_H

/*
 *Draft for bergamota configuration items*

 *Match config*
 MATCH_X_TOLERANCE_DFL.........When comparing neighbors
 MATCH_Y_TOLERANCE_DFL.........When comparing neighbors
 MATCH_T_TOLERANCE_DFL.........When comparing neighbors
 MATCH_MINUTIA_THRESHOLD_DFL...How many matching neighbors to consider two
 minutiae compatible
 MATCH_TEMPLATE_THRESHOLD_DFL..How many matching minutiae to consider two
 templates compatible
 MATCH_FAILURE_THRESHOLD_DFL...How many non-compatible minutiae to abort a match
 (0 - Do not abort)

 *DB config* -- Creation time only
 DB_MAX_X_COORD_DFL........Highest possible value for a minutia's X coordinate.
 DB_MAX_Y_COORD_DFL........Highest possible value for a minutia's Y coordinate.
 DB_PIXELS_PER_GROUP_DFL...How many x/y units will be consider in the same
 group.
 DB_DEGREES_PER_GROUP_DFL..How many degrees units wil be consider in the same
 group.

 *Template config*
 MAX_MINUTIAE_PER_TEMPLATE_DFL \__.-Implemented in template, only DFL defines
 are missing
 NEIGHBORS_PER_MINUTIAE_DFL    /

*/

// Template config.
#define MAX_MINUTIAE_PER_TEMPLATE_DFL 64
#define NEIGHBORS_PER_MINUTIAE_DFL 20

// Match config.
#define MATCH_X_TOLERANCE_DFL 5
#define MATCH_Y_TOLERANCE_DFL 5
#define MATCH_T_TOLERANCE_DFL 7
#define MATCH_MINUTIA_THRESHOLD_DFL 15
#define MATCH_TEMPLATE_THRESHOLD_DFL 1
#define MATCH_FAILURE_THRESHOLD_DFL 0

#endif // CONFIG_H
