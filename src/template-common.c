/**
 * @file   template-common.c
 * @author rodrigo
 * @date   13/10/2014
 * @brief  Common functions to create BGM_template.
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#define _GNU_SOURCE
#include <bergamota.h>
#include <template.h>
#include <debug.h>

#include <math.h>
#include <stdlib.h>

/**
 * The connection between two minutiae, represented as a vector.
 * Using euclidean vector terminology
 */
struct _BGM_connection {
    unsigned int magnitude;
    unsigned int direction;
    struct _BGM_minutia *first_minutia;
    struct _BGM_minutia *second_minutia;
};

/**
 * Given its scalar components (x,y), this function returns the direction of a
 * vector.
 *
 * @param[in] dx    X component.
 * @param[in] dy    Y component.
 *
 * @return The angle in degrees (0 .. 359)
 */
static unsigned int
_BGM_xy_to_angle(int x, int y)
{
    double direction;

    if (x == 0) {
        direction = 90;
    } else if (y == 0) {
        direction = 0;
    } else {
        // Calculate and transform from radians to degrees
        direction = atan2(y, x) * (180.0 / M_PI);
        if (direction < 0) {
            // Transform into a positive angle
            direction = direction + 360;
        }
    }

    return (unsigned int)direction;
}

/**
 * Given its magnitude an direction, this function calculates the scalar
 * components (x,y) of a
 * vector.
 *
 * @param[in]  magnitude    Vector's magnitude ("lenght")
 * @param[in]  direction    Vector's direction (the angle)
 * @param[out] x            Calculated X component.
 * @param[out] y            Calculated Y component
 */
static void
_BGM_angle_to_xy(unsigned int magnitude, unsigned int direction, int *x, int *y)
{
    double rad = direction * (M_PI / 180);

    *x = round(cos(rad) * magnitude);
    *y = round(sin(rad) * magnitude);
}

/**
 * Function to be used with 'qsort', sorting connections by its magnitude in
 * ascending order.
 *
 * @param[in] ptr1	Connection 1.
 * @param[in] ptr2	Connection 2.
 *
 * @retval  0 (Connection1 == Connection2).
 * @retval -1 (Connection1 < Connection2).
 * @retval  1 (Connection1 > Connection2).
 */
static int
_BGM_sort_connections_by_magnitude(const void *ptr1, const void *ptr2)
{
    struct _BGM_connection *connection1;
    struct _BGM_connection *connection2;

    connection1 = (struct _BGM_connection *)ptr1;
    connection2 = (struct _BGM_connection *)ptr2;

    if (connection1->magnitude < connection2->magnitude) {
        return -1;
    } else if (connection1->magnitude > connection2->magnitude) {
        return 1;
    } else {
        return 0;
    }
}

static void
_BGM_connection_from_minutiae(struct _BGM_minutia *first_minutia,
                              struct _BGM_minutia *second_minutia,
                              struct _BGM_connection *connection)
{
    int dx;
    int dy;

    dx = second_minutia->x - first_minutia->x;
    dy = second_minutia->y - first_minutia->y;

    connection->magnitude =
        sqrt(pow(dx, 2) + pow(dy, 2)); // Pythagorean theorem
    connection->direction = _BGM_xy_to_angle(dx, dy);
    connection->first_minutia = first_minutia;
    connection->second_minutia = second_minutia;
}

/**
 * Given a template, this function creates connections between every two
 * minutiae.
 *
 * @param[in]  tpl              Template to be "connected".
 * @param[out] connections      Connection created from template's minutiae.
 * @param[out] num_connections  Number of connections created.
 *
 * @retval  BGM_SUCCESS     Connections created successfully.
 */
static void
_BGM_connections_from_template(struct BGM_template *tpl,
                               struct _BGM_connection *connections,
                               unsigned int *num_connections)
{
    *num_connections = 0;

    // Create connections between every minutiae in template
    for (unsigned int i = 0; i < (tpl->num_minutiae - 1); i++) {
        for (unsigned int j = (i + 1); j < tpl->num_minutiae; j++) {
            // Create connection using data from the two minutiae
            _BGM_connection_from_minutiae(&tpl->minutiae[i],
                                          &tpl->minutiae[j],
                                          &connections[*num_connections]);
            (*num_connections)++;
        }
    }

    // Sort connections by magnitude
    qsort((void *)connections,
          *num_connections,
          sizeof(struct _BGM_connection),
          _BGM_sort_connections_by_magnitude);
}

static inline void
_BGM_destroy_minutia(struct _BGM_minutia *minutia)
{
    if (minutia->neighbors != NULL) {
        free(minutia->neighbors);
        minutia->neighbors = NULL;
        minutia->num_neighbors = 0;
    }
}

static void
_BGM_create_minutia_neighbors(struct _BGM_minutia *minutia,
                              struct _BGM_connection *connections,
                              unsigned int num_connections,
                              unsigned int max_neighbors)
{
    unsigned int neighbor_count = 0;
    unsigned int temp_angle;

    // For each connection
    for (unsigned int i = 0;
         (i < num_connections) && (neighbor_count < max_neighbors);
         i++) {
        struct _BGM_neighbor *cur_neighbor;
        cur_neighbor = &minutia->neighbors[neighbor_count];
        // Check whether the minutia is the first minutia of current connection
        if (minutia->id == connections[i].first_minutia->id) {
            cur_neighbor->neighbor_id = connections[i].second_minutia->id;
            cur_neighbor->relative_angle = connections[i].second_minutia->angle;
            temp_angle = connections[i].direction;
            // Check whether the minutia is the second minutia of current
            // connection
        } else if (minutia->id == connections[i].second_minutia->id) {
            cur_neighbor->neighbor_id = connections[i].first_minutia->id;
            cur_neighbor->relative_angle = connections[i].first_minutia->angle;
            temp_angle = (connections[i].direction + 180) % 360;
        } else {
            // The minutia is not present in current connection, go to next
            // connection
            continue;
        }

        if (temp_angle > minutia->angle) {
            temp_angle = temp_angle - minutia->angle;
        } else if (temp_angle < minutia->angle) {
            temp_angle = 360 - minutia->angle + temp_angle;
        } else {
            temp_angle = 0;
        }

        cur_neighbor->relative_angle =
            cur_neighbor->relative_angle > minutia->angle
            ? cur_neighbor->relative_angle - minutia->angle
            : minutia->angle - cur_neighbor->relative_angle;
        // Calculate relative_x and relative_y from connection's magnitude and
        // temp_angle
        _BGM_angle_to_xy(connections[i].magnitude,
                         temp_angle,
                         &cur_neighbor->relative_x,
                         &cur_neighbor->relative_y);

        neighbor_count++;
    }

    minutia->num_neighbors = neighbor_count;
}

void
_BGM_reset_template(struct BGM_template *tpl)
{
    tpl->magic_num = _BGM_TEMPLATE_INIT_MAGIC_NUMBER;
    tpl->minutiae = NULL;
    tpl->num_minutiae = 0;
}

/**
 * Given a template and all its minutiae connections, this function identifies
 * the neighbors of each
 * minutia and stores this result in the minutia structure.
 *
 * @param[in,out] tpl               Template that owns the minutiae.
 * @param[in]     connections       Connections between every two minutiae in
 * template.
 * @param[in]     num_connections   Number of connections in 'connections'.
 *
 * @return BGM_SUCCESS  Only success for now (could it be a void function?).
 */
static BGM_status
_BGM_create_template_neighbors(struct BGM_template *tpl,
                               struct _BGM_connection *connections,
                               unsigned int num_connections,
                               unsigned int num_neighbors)
{
    BGM_status status = BGM_SUCCESS;

    // For each minutia in template
    for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
        struct _BGM_minutia *cur_minutia = &tpl->minutiae[i];
        // Allocate memory for minutia's neighbors
        cur_minutia->neighbors =
            malloc(num_neighbors * sizeof(*cur_minutia->neighbors));
        if (cur_minutia->neighbors != NULL) {
            // Populate minutia's neighbors using information from connections
            _BGM_create_minutia_neighbors(cur_minutia,
                                          connections,
                                          num_connections,
                                          num_neighbors);
        } else {
            // If memory allocation fails for a minutia's neighbors, free the
            // memory previously
            // allocated for other minutiae
            status = BGM_E_NO_MEMORY;
            for (unsigned int j = 0; j < i; j++) {
                _BGM_destroy_minutia(&tpl->minutiae[j]);
            }
            break;
        }
    }

    PRINT_IF_ERROR(status);
    return status;
}

/**
 * Given a template, this function computes the neighbors for each minutiae.
 *
 * @param[in,out] tpl Input template, which will have the minutiae updated.
 *
 * @retval BGM_SUCCESS  Minutiae's neghborhood updated successfully.
 * @retval BGM_E_GENERIC    An error ocurred.
 */
BGM_status
_BGM_intialize_template(struct BGM_template *tpl, unsigned int num_neighbors)
{
    BGM_status status;
    struct _BGM_connection *connections;
    unsigned int num_connections;

    // There is a minimun number of minutiae, depending on the required number
    // of neighbors.
    if (tpl->num_minutiae > num_neighbors) {
        connections = malloc(tpl->num_minutiae * tpl->num_minutiae
                             * sizeof(*connections));
        if (connections != NULL) {
            // Create connections between every pair of minutiae
            _BGM_connections_from_template(tpl, connections, &num_connections);
            // Identify the neighborhood of each minutia
            status = _BGM_create_template_neighbors(tpl,
                                                    connections,
                                                    num_connections,
                                                    num_neighbors);
            if (status == BGM_SUCCESS) {
                // TODO: Review it. This assignment is also done in
                // '_BGM_reset_template'
                tpl->magic_num = _BGM_TEMPLATE_INIT_MAGIC_NUMBER;
            }
            //
            free(connections);
        } else {
            status = BGM_E_NO_MEMORY;
        }
    } else {
        status = BGM_E_TOO_FEW_MINUTIAE;
    }

    PRINT_IF_ERROR(status);
    return status;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  P U B L I C  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
BGM_destroy_template(struct BGM_template *tpl)
{
    if (tpl != NULL) {
        if (_BGM_IS_TEMPLATE_INITIALIZED(*tpl)) {
            if (tpl->minutiae != NULL) {
                for (unsigned int i = 0; i < tpl->num_minutiae; i++) {
                    _BGM_destroy_minutia(&tpl->minutiae[i]);
                }
                free(tpl->minutiae);
            }
            tpl->magic_num = 0;
        }
    }
}
