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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <template.h>

#define _XYTH_CONTEXT_INIT_MAGIC_NUMBER 0x004D4742

//
// Match configuration
//

struct _XYTH_match_config {
    unsigned int minutia_threshold; // No. of matching neighbors required to
                                    // consider two minutiae
                                    // compatible

    unsigned int template_threshold; // No. of matching minutiae required to
                                     // consider two templates
                                     // compatible

    // FIXME: The actual failure threshold is 0, regardless of the value
    // configured here.
    unsigned int failure_threshold; // No. of non-compatible minutiae required
                                    // to abort a match
                                    // (0 - Do not abort)

    unsigned int x_tolerance; // Coordinates tolerance...
    unsigned int y_tolerance; // ...when comparing neighbors.
    unsigned int t_tolerance; // Angle tolerance when comparing neighbors.
};

//
// Database configuration
//

// Default values
#define DB_MAX_X_COORD_DFL 353
#define DB_MAX_Y_COORD_DFL 390
#define DB_MAX_T_ANGLE_DFL 359
#define DB_PIXELS_PER_GROUP_DFL 1
#define DB_DEGREES_PER_GROUP_DFL 2
#define DB_ALLOC_STEP_DFL 32

// Structure used to hold/transmit configuration
struct XYTH_database_config {
    unsigned int max_x;
    unsigned int max_y;
    unsigned int pixels_per_group;
    unsigned int degrees_per_group;
    unsigned int alloc_step; // in members, not bytes
};

// Macros for basic structure manipulation
#define _XYTH_DB_CONFIG_SET_MAX_COORD(cfg, x, y) \
    do {                                        \
        cfg.max_x = x;                          \
        cfg.max_y = y;                          \
    } while (0)

#define _XYTH_DB_CONFIG_SET_DENSITY(cfg, ppg, dpg) \
    do {                                          \
        cfg.pixels_per_group = ppg;               \
        cfg.degrees_per_group = dpg;              \
    } while (0)

#define _XYTH_DB_CONFIG_INIT(cfg)                          \
    do {                                                  \
        cfg.max_x = DB_MAX_X_COORD_DFL;                   \
        cfg.max_y = DB_MAX_Y_COORD_DFL;                   \
        cfg.pixels_per_group = DB_PIXELS_PER_GROUP_DFL;   \
        cfg.degrees_per_group = DB_DEGREES_PER_GROUP_DFL; \
        cfg.alloc_step = DB_ALLOC_STEP_DFL;               \
    } while (0)

//
// Database
//
struct _XYTH_database {
    unsigned int templates_counter;
    unsigned int next_template_id;
    unsigned int **data;
    unsigned int *alloc_counter; // in members, not bytes
    unsigned int num_groups;
};

//
// Context
//
struct XYTH_context {
    unsigned int magic_number;
    struct _XYTH_match_config match_cfg;
    struct XYTH_database_config db_cfg;
    struct _XYTH_database db;
};

#define _XYTH_IS_CONTEXT_INITIALIZED(ctx) \
    ((ctx).magic_number == _XYTH_CONTEXT_INIT_MAGIC_NUMBER)

#endif // CONTEXT_H
