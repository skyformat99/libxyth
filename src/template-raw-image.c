/**
 * @file   template-raw-image.c
 * @author rodrigo
 * @date   11/05/2015
 * @brief  Convertion raw gray-scale image => BGM_template.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <bergamota.h>
#include <template.h>
#include <debug.h>

#include <mindtct/lfs.h>
#include <mindtct/bozorth.h>

#include "template-common.h"

#define MINUTIA_MIN_RELIABILITY 45

struct _BGM_image_line_boundaries {
    unsigned int first_nonwhite_pixel;
    unsigned int last_nonwhite_pixel;
    bool all_white;
};

/* Copied from NBIS package's bz_sort.c */
int
sort_quality_decreasing(const void *a, const void *b)
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
int
sort_x_y(const void *a, const void *b)
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

static BGM_status
_BGM_mindtct_get_minutiae(unsigned char *buffer,
                          unsigned int width,
                          unsigned int height,
                          unsigned int pixel_depth,
                          unsigned int resolution_in_ppi,
                          struct BGM_template *tpl,
                          unsigned int max_num_minutiae)
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
    BGM_status status;
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
            lfs2nist_minutia_XYT(&ox, &oy, &ot, minutiae->list[i], width, height);
            minutiae_nist_form[i].col[0] = ox;
            minutiae_nist_form[i].col[1] = oy;
            minutiae_nist_form[i].col[2] = ot;
            minutiae_nist_form[i].col[3] = (int)(minutiae->list[i]->reliability * 100);
        }
        // Sort by quality.
        qsort((void *)&minutiae_nist_form,
              (size_t)minutiae->num,
              sizeof(struct minutiae_struct),
              sort_quality_decreasing);

        // If found too many minutiae, then discard the ones with less quality.
        if ((unsigned int)minutiae->num > max_num_minutiae) {
            minutiae->num = max_num_minutiae;
        }
        // Sort by X, then by Y
        qsort((void *)&minutiae_nist_form,
              (size_t)minutiae->num,
              sizeof(struct minutiae_struct),
              sort_x_y);
        // Copy data to a bergamota struct
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
        status = BGM_SUCCESS;
        free_minutiae(minutiae);
    } else {
        status = BGM_E_MINUTIAE_EXTRACTOR_ERROR;
    }

    PRINT_IF_ERROR(status);
    return status;
}

static BGM_status
_BGM_template_from_raw_image(unsigned char *buffer,
                             unsigned int width,
                             unsigned int height,
                             unsigned int pixel_depth,
                             unsigned int resolution_in_ppi,
                             struct BGM_template *tpl,
                             unsigned int max_num_minutiae,
                             unsigned int num_neighbors)
{
    BGM_status status;

    tpl->minutiae = malloc(max_num_minutiae * sizeof(*tpl->minutiae));
    if (tpl->minutiae != NULL) {
        status = _BGM_mindtct_get_minutiae(buffer,
                                           width,
                                           height,
                                           pixel_depth,
                                           resolution_in_ppi,
                                           tpl,
                                           max_num_minutiae);
        if (status == BGM_SUCCESS) {
            status = _BGM_intialize_template(tpl, num_neighbors);
        }
    } else {
        status = BGM_E_NO_MEMORY;
    }

    PRINT_IF_ERROR(status);
    return status;
}

BGM_status
BGM_template_from_raw_image(unsigned char *buffer,
                            unsigned int width,
                            unsigned int height,
                            unsigned int pixel_depth,
                            unsigned int resolution_in_ppi,
                            struct BGM_template *tpl,
                            unsigned int max_num_minutiae,
                            unsigned int num_neighbors)
{
    int status;

    // Param check
    if (buffer == NULL || width == 0 || height == 0 || pixel_depth == 0 || resolution_in_ppi == 0
        || tpl == NULL
        || max_num_minutiae == 0
        || num_neighbors == 0) {
        PRINT_IF_NULL(buffer);
        PRINT_IF_TRUE(width == 0);
        PRINT_IF_TRUE(height == 0);
        PRINT_IF_TRUE(pixel_depth == 0);
        PRINT_IF_TRUE(resolution_in_ppi == 0);
        PRINT_IF_NULL(tpl);
        PRINT_IF_TRUE(max_num_minutiae == 0);
        PRINT_IF_TRUE(num_neighbors == 0);

        status = BGM_E_INVALID_PARAMETER;
        PRINT_IF_ERROR(status);
        return status;
    }

    if (!_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
        _BGM_reset_template(tpl);
        status = _BGM_template_from_raw_image(buffer,
                                              width,
                                              height,
                                              pixel_depth,
                                              resolution_in_ppi,
                                              tpl,
                                              max_num_minutiae,
                                              num_neighbors);
        if (status != BGM_SUCCESS) {
            BGM_destroy_template(tpl);
        }
    } else {
        status = BGM_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}
