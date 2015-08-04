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

//TODO: fix the image mask, a mechanism that removes border minutiae.

#include <bergamota.h>
#include <template.h>
#include <debug.h>

#include <mindtct/lfs.h>
#include <mindtct/bozorth.h>

#include "template-common.h"

//#include <fcntl.h>

//#define FILTER_BORDER 5 // Faixa, em pixels, ignorada pelo filtro
///*
// * Máscara indicando a área útil de uma linha da imagem
// */
//struct line_mask {
//    int begin; // Início da área útil
//    int end; // Fim da área útil
//    int empty; // Indica se a linha está vazia (invalida os campos acima)
//};

/* Copied from NBIS package's bz_sort.c */
int sort_quality_decreasing(const void *a, const void *b)
{
    struct minutiae_struct *af;
    struct minutiae_struct *bf;

    af = (struct minutiae_struct *) a;
    bf = (struct minutiae_struct *) b;

    if ( af->col[3] > bf->col[3] )
        return -1;
    if ( af->col[3] < bf->col[3] )
        return 1;
    return 0;
}

/* Copied from NBIS package's bz_sort.c */
int sort_x_y(const void *a, const void *b)
{
    struct minutiae_struct *af;
    struct minutiae_struct *bf;

    af = (struct minutiae_struct *) a;
    bf = (struct minutiae_struct *) b;

    if ( af->col[0] < bf->col[0] )
        return -1;
    if ( af->col[0] > bf->col[0] )
        return 1;

    if ( af->col[1] < bf->col[1] )
        return -1;
    if ( af->col[1] > bf->col[1] )
        return 1;

    return 0;
}

static BGM_status _BGM_mindtct_get_minutiae(
                                             unsigned char *buffer,
                                             unsigned int width,
                                             unsigned int height,
                                             unsigned int pixel_depth,
                                             unsigned int resolution_in_ppi,
                                             struct BGM_template *tpl,
                                             unsigned int max_num_minutiae
                                           )
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

    // Get minutiae using mindtct's get_minutiae()
    ret = get_minutiae(&minutiae, &quality_map, &direction_map,
        &low_contrast_map, &low_flow_map, &high_curve_map,
        &map_w, &map_h, &bdata, &bw, &bh, &bd,
        buffer, width, height, pixel_depth, pixels_per_mm, &lfsparms_V2);

    // A bit paranoid, but this is a external library.
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
        }
        // Sort by quality.
        qsort((void *) &minutiae_nist_form, (size_t) minutiae->num,
              sizeof (struct minutiae_struct), sort_quality_decreasing);

        // If found too many minutiae, then discard the ones with less quality.
        if ((unsigned int)minutiae->num > max_num_minutiae) {
            minutiae->num = max_num_minutiae;
        }
        // Sort by X, then by Y
        qsort((void *) &minutiae_nist_form, (size_t) minutiae->num,
              sizeof (struct minutiae_struct), sort_x_y);

        // Copy data to a bergamota struct
        for (int i = 0; i < minutiae->num; i++) {
            tpl->minutiae[i].id = i;
            tpl->minutiae[i].x = minutiae_nist_form[i].col[0];
            tpl->minutiae[i].y = minutiae_nist_form[i].col[1];
            tpl->minutiae[i].angle = minutiae_nist_form[i].col[2];
        }
        tpl->num_minutiae = minutiae->num;

        free_minutiae(minutiae);

        status = BGM_SUCCESS;
    } else {
        status = BGM_E_MINUTIAE_EXTRACTOR_ERROR;
    }

    PRINT_IF_ERROR(status);
    return status;
}

///*
// * Registra onde começa e termina cada linha da imagem.
// */
//int read_img_mask(struct RawImageData *img, struct line_mask* img_mask)
//{
//    char *line;
//    int x;

//    memset(img_mask, 0, sizeof(struct line_mask) * img->height);

//    line = (char*)&img[0];

//    for (int y = 0; y < img->height; y++) {
//        int line_index = (img->height - 1) - y;
//        for (x = 0; line[x] == (char)0xff && x < img->width; x++);
//        if (x == img->width) {
//            img_mask[line_index].empty = 1;
//            line += img->width;
//            continue;
//        }
//        img_mask[line_index].begin = x;
//        for (x = img->width - 1; line[x] == (char)0xff; x--);
//        img_mask[line_index].end = x;
//        line += img->width;
//    }

//    return 0;
//}

///*
// * Retira do template tpl_in as minúcias que ficam na borda da imagem, e salva o novo template em tpl_out.
// * As minúcias detectadas na borda da imagem, normalmente, são inúteis para a identificação. Elas surgem da
// * detecção de terminações das rugas, terminações que só aparecem porque chegou ao fim do dedo.
// */
//int filter_min(struct Template *tpl_in, int width, int height, struct line_mask *img_mask, struct Template *tpl_out)
//{
//    int first, last;
//    int count;

//    /* Vai até a primeira linha não vazia da imagem */
//    for (first = 0; first < height - 1 && img_mask[first].empty; first++);
//    /* Se todas as linhas úteis estiverem vazias... */
//    if (first > (height - 1 - FILTER_BORDER)) {
//        printf("Erro no filtro 1.\n");
//        return -1;
//    }
//    /* Com o início já determinado, pula a borda */
//    first += FILTER_BORDER;
//    /* Acha a última linha não vazia da imagem */
//    for (last = height - 1; last > 0 && img_mask[last].empty; last--);
//    /* Se todas as linhas úteis estiverem vazias... [PARANOIA] */
//    if (last < FILTER_BORDER) {
//        printf("Erro no filtro 2.\n");
//        return -1;
//    }
//    /* Com o fim já determinado, pula a borda */
//    last -= FILTER_BORDER;
//    count = 0;
//    /* Para cada minúcia... */
//    for (int i = 0; i < tpl_in->mins_size; i++) {
//        int x = tpl_in->mins[i].x;
//        int y = tpl_in->mins[i].y;
//        /* Se estiver dentro do intervalo no eixo Y (geral) */
//        if (y > first && y < last) {
//            /* Se estiver dentro do intervalo no eixo X (por linha), minúcia boa */
//            if (x > img_mask[y].begin + FILTER_BORDER && x < img_mask[y].end - FILTER_BORDER) {
//                tpl_out->mins[count].x = x;
//                tpl_out->mins[count].y = y;
//                tpl_out->mins[count].theta = tpl_in->mins[i].theta;
//                tpl_out->mins[count].id = count;
//                count++;
//            }
//        }
//    }
//    tpl_out->mins_size = count;

//    return 0;
//}

///**
// * Cria um template a partir de uma imagem raw.
// * @param [in] img Imagem de onde serão extraídas as minúcias
// * @param [out] tpl Ponteiro para onde será armazenado o novo template.
// * @return Retorna 0 em caso de sucesso, caso contrário, um valor negativo.
// */
//int create_tpl(struct RawImageData *img, struct Template *tpl)
//{
//    int ret;
//    struct line_mask img_mask[img->height];
//    struct Template local_tpl;
//    /* Extração das minúcias */
//    ret = xtract_min(img, &local_tpl);
//    if (ret < 0) {
//        printf("%s: xtract_min falhou\n", __FUNCTION__);
//        return -1;
//    }
//    /* Lê onde começa e onde termina a imagem em cada linha */
//    ret = read_img_mask(img, img_mask);
//    if (ret < 0) {
//        printf("%s: read_img_mask falhou\n", __FUNCTION__);
//        return -1;
//    }
//    /* Retira do template preliminar as minúcias detectadas na borda da imagem.
//     * Armazena em tpl o template já filtrado */
//    ret = filter_min(&local_tpl, img->width, img->height, img_mask, tpl);
//    if (ret < 0) {
//        printf("%s: filter_min falhou\n", __FUNCTION__);
//        return -1;
//    }

//    return 0;
//}

static BGM_status _BGM_template_from_raw_image(
                                                unsigned char *buffer,
                                                unsigned int width,
                                                unsigned int height,
                                                unsigned int pixel_depth,
                                                unsigned int resolution_in_ppi,
                                                struct BGM_template *tpl,
                                                unsigned int max_num_minutiae,
                                                unsigned int num_neighbors
                                              )
{
    BGM_status status;

    tpl->minutiae = malloc(max_num_minutiae * sizeof(*tpl->minutiae));
    if (tpl->minutiae != NULL) {
        status = _BGM_mindtct_get_minutiae(buffer, width, height, pixel_depth,
                                           resolution_in_ppi, tpl, max_num_minutiae);
        if (status == BGM_SUCCESS) {
            status = _BGM_intialize_template(tpl, num_neighbors);
        }
    } else {
        status = BGM_E_NO_MEMORY;
    }

    PRINT_IF_ERROR(status);
    return status;
}

BGM_status BGM_template_from_raw_image(
                                        unsigned char *buffer,
                                        unsigned int width,
                                        unsigned int height,
                                        unsigned int pixel_depth,
                                        unsigned int resolution_in_ppi,
                                        struct BGM_template *tpl,
                                        unsigned int max_num_minutiae,
                                        unsigned int num_neighbors
                                      )
{
    int status;

    // Param check
    if (buffer == NULL || width == 0 || height == 0 || pixel_depth == 0 ||
        resolution_in_ppi == 0 || tpl == NULL || max_num_minutiae == 0 ||
        num_neighbors == 0)
    {
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
        status = _BGM_template_from_raw_image(buffer, width, height, pixel_depth,
                                              resolution_in_ppi, tpl,
                                              max_num_minutiae, num_neighbors);
        if (status != BGM_SUCCESS) {
            BGM_destroy_template(tpl);
        }
    } else {
        status = BGM_E_ALREADY_INITIALIZED;
    }

    PRINT_IF_ERROR(status);
    return status;
}


