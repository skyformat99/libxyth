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

#include <debug.h>
#include <template.h>
#include <xyth.h>

#include <mindtct/bozorth.h>
#include <mindtct/lfs.h>

#include "config.h"
#include "template-common.h"

#define MINUTIA_MIN_RELIABILITY 45

struct _XYTH_image_line_boundaries {
    unsigned int first_nonwhite_pixel;
    unsigned int last_nonwhite_pixel;
    bool all_white;
};

/* Copied from NBIS package's bz_sort.c */
static int
_XYTH_sort_quality_decreasing(const void *a, const void *b)
{
    struct minutiae_struct *af;
    struct minutiae_struct *bf;

    af = (struct minutiae_struct *)a;
    bf = (struct minutiae_struct *)b;

    if (af->col[3] > bf->col[3])
        return -1;
    if (af->col[3] < bf->col[3])
        return 1;
    return 0;
}

/* Copied from NBIS package's bz_sort.c */
static int
_XYTH_sort_x_y(const void *a, const void *b)
{
    struct minutiae_struct *af;
    struct minutiae_struct *bf;

    af = (struct minutiae_struct *)a;
    bf = (struct minutiae_struct *)b;

    if (af->col[0] < bf->col[0])
        return -1;
    if (af->col[0] > bf->col[0])
        return 1;

    if (af->col[1] < bf->col[1])
        return -1;
    if (af->col[1] > bf->col[1])
        return 1;

    return 0;
}

static XYTH_status
_XYTH_mindtct_get_minutiae(unsigned char *buffer,
                           unsigned int width,
                           unsigned int height,
                           unsigned int pixel_depth,
                           unsigned int resolution_in_ppi,
                           struct XYTH_template *tpl)
{
    double pixels_per_mm = (resolution_in_ppi / 25.4);
    int bw, bh, bd;
    unsigned char *bdata;
    int *direction_map, *low_contrast_map, *low_flow_map;
    int *high_curve_map, *quality_map;
    int map_w, map_h;
    MINUTIAE *minutiae;
    int ox, oy, ot;
    int ret;
    struct minutiae_struct minutiae_nist_form[2000];
    XYTH_status status;
    unsigned int min_index;

    // Get minutiae using mindtct's get_minutiae()
    ret = get_minutiae(&minutiae,
                       &quality_map,
                       &direction_map,
                       &low_contrast_map,
                       &low_flow_map,
                       &high_curve_map,
                       &map_w,
                       &map_h,
                       &bdata,
                       &bw,
                       &bh,
                       &bd,
                       buffer,
                       width,
                       height,
                       pixel_depth,
                       pixels_per_mm,
                       &lfsparms_V2);

    // A bit paranoid, but this is an external library.
    if (ret == 0 && minutiae != NULL && minutiae->num > 0) {
        free(quality_map);
        free(direction_map);
        free(low_contrast_map);
        free(low_flow_map);
        free(high_curve_map);
        free(bdata);
        // Convert minutiae to NIST form (system origin and angle differences)
        for (int i = 0; i < minutiae->num; i++) {
            lfs2nist_minutia_XYT(&ox,
                                 &oy,
                                 &ot,
                                 minutiae->list[i],
                                 width,
                                 height);
            minutiae_nist_form[i].col[0] = ox;
            minutiae_nist_form[i].col[1] = oy;
            minutiae_nist_form[i].col[2] = ot;
            minutiae_nist_form[i].col[3] =
                (int)(minutiae->list[i]->reliability * 100);
        }
        // Sort by quality.
        qsort((void *)&minutiae_nist_form,
              (size_t)minutiae->num,
              sizeof(struct minutiae_struct),
              _XYTH_sort_quality_decreasing);

        // If found too many minutiae, then discard the ones with less quality.
        if ((unsigned int)minutiae->num > MAX_MINUTIAE_PER_TEMPLATE) {
            minutiae->num = MAX_MINUTIAE_PER_TEMPLATE;
        }
        // Sort by X, then by Y
        qsort((void *)&minutiae_nist_form,
              (size_t)minutiae->num,
              sizeof(struct minutiae_struct),
              _XYTH_sort_x_y);
        // Copy data to a xyth's struct
        min_index = 0;
        for (int i = 0; i < minutiae->num; i++) {
            tpl->minutiae[min_index].neighbors = NULL;
            tpl->minutiae[min_index].num_neighbors = 0;

            tpl->minutiae[min_index].id = min_index;
            tpl->minutiae[min_index].x = minutiae_nist_form[i].col[0];
            tpl->minutiae[min_index].y = minutiae_nist_form[i].col[1];
            tpl->minutiae[min_index].angle = minutiae_nist_form[i].col[2];

            if (minutiae_nist_form[i].col[3] >= MINUTIA_MIN_RELIABILITY) {
                min_index++;
            }
        }
        tpl->num_minutiae = min_index;
        status = XYTH_SUCCESS;
        free_minutiae(minutiae);
    } else {
        status = XYTH_E_MINUTIAE_EXTRACTOR_ERROR;
    }

    PRINT_IF_ERROR(status);
    return status;
}

static XYTH_status
_XYTH_template_from_raw_image(unsigned char *buffer,
                              unsigned int width,
                              unsigned int height,
                              unsigned int pixel_depth,
                              unsigned int resolution_in_ppi,
                              struct XYTH_template *tpl,
                              unsigned int num_neighbors)
{
    XYTH_status status;

    tpl->minutiae = malloc(MAX_MINUTIAE_PER_TEMPLATE * sizeof(*tpl->minutiae));
    if (tpl->minutiae != NULL) {
        status = _XYTH_mindtct_get_minutiae(buffer,
                                            width,
                                            height,
                                            pixel_depth,
                                            resolution_in_ppi,
                                            tpl);
        if (status == XYTH_SUCCESS) {
            status = _XYTH_intialize_template(tpl, num_neighbors);
        }
    } else {
        status = XYTH_E_NO_MEMORY;
    }

    PRINT_IF_ERROR(status);
    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

XYTH_status
XYTH_template_from_raw_image(unsigned char *buffer,
                             unsigned int width,
                             unsigned int height,
                             unsigned int pixel_depth,
                             unsigned int resolution_in_ppi,
                             struct XYTH_template *tpl,
                             unsigned int num_neighbors)
{
    int status;

    // Param check
    if (buffer == NULL || width == 0 || height == 0 || pixel_depth == 0
        || resolution_in_ppi == 0
        || tpl == NULL
        || num_neighbors == 0) {
        PRINT_IF_NULL(buffer);
        PRINT_IF_TRUE(width == 0);
        PRINT_IF_TRUE(height == 0);
        PRINT_IF_TRUE(pixel_depth == 0);
        PRINT_IF_TRUE(resolution_in_ppi == 0);
        PRINT_IF_NULL(tpl);
        PRINT_IF_TRUE(num_neighbors == 0);

        status = XYTH_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (!_XYTH_IS_TEMPLATE_INITIALIZED(*tpl)) {
        _XYTH_reset_template(tpl);
        status = _XYTH_template_from_raw_image(buffer,
                                               width,
                                               height,
                                               pixel_depth,
                                               resolution_in_ppi,
                                               tpl,
                                               num_neighbors);
        if (status != XYTH_SUCCESS) {
            XYTH_destroy_template(tpl);
        }
    } else {
        status = XYTH_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
