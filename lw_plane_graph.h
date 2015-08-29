/*
 * Main developer: Nico Van Cleemput
 *
 * Copyright (C) 2015 Nico Van Cleemput.
 * Licensed under the GNU AFFERO GPL, read the file LICENSE for details.
 */

/* Data structures and methods to work with lightweight plane graphs.
 * These are lightweight because no edges are stored, just the adjacency
 * information, and the order around each vertex.
 */

#include <stdio.h>
#include "bitset.h"

#ifndef LW_PLANE_GRAPH_H
#define	LW_PLANE_GRAPH_H

// -1 because we start labeling vertices from 1
#define MAXN (SET_CAPACITY - 1)

typedef int boolean;

#define TRUE 1
#define FALSE 0

typedef int GRAPH[MAXN + 1][MAXN];
typedef int ADJACENCY[MAXN + 1];

void decodePlanarCode(unsigned short* code, GRAPH graph, ADJACENCY adj, bitset* neighbours);

int readPlanarCode(unsigned short code[], int *length, FILE *file);


#endif	/* LW_PLANE_GRAPH_H */
