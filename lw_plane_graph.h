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
#include "boolean.h"

#ifndef LW_PLANE_GRAPH_H
#define	LW_PLANE_GRAPH_H

// -1 because we start labeling vertices from 1
#define MAXN (SET_CAPACITY - 1)
#define MAXE (6*MAXN-12)    /* the maximum number of oriented edges */
#define MAXCODELENGTH (MAXN+MAXE+3)

typedef int GRAPH[MAXN + 1][MAXN];
typedef int ADJACENCY[MAXN + 1];

void decodePlanarCode(unsigned short* code, GRAPH graph, ADJACENCY adj, bitset* neighbours);

int readPlanarCode(unsigned short code[], int *length, FILE *file);

void writePlanarCodeHeader(FILE *f);

void writeCode(FILE *f, unsigned short code[], int length);

#endif	/* LW_PLANE_GRAPH_H */

