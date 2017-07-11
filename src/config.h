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

#ifndef CONFIG_H
#define CONFIG_H

/*
 *DB config* -- Creation time only
 DB_MAX_X_COORD_DFL........Highest possible value for a minutia's X coordinate.
 DB_MAX_Y_COORD_DFL........Highest possible value for a minutia's Y coordinate.
 DB_PIXELS_PER_GROUP_DFL...How many x/y units will be consider in the same
 group.
 DB_DEGREES_PER_GROUP_DFL..How many degrees units wil be consider in the same
 group.
*/

// Template config.
#define MAX_MINUTIAE_PER_TEMPLATE 64
#define NEIGHBORS_PER_MINUTIAE_DFL 20

// Match config.
#define MATCH_X_TOLERANCE_DFL 5
#define MATCH_Y_TOLERANCE_DFL 5
#define MATCH_T_TOLERANCE_DFL 7

// Number of matching neighbors needed to consider two minutiae compatible
#define MATCH_MINUTIA_THRESHOLD_DFL 15
// Number of matching minutiae needed to consider two fingerprints compatible
#define MATCH_TEMPLATE_THRESHOLD_DFL 1
// Number of non-compatible minutiae needed to abort an identification.
// 0 - Don't abort
#define MATCH_FAILURE_THRESHOLD_DFL 0

#endif // CONFIG_H
