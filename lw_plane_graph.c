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

#include <stdlib.h>
#include <string.h>

#include "lw_plane_graph.h"

void decodePlanarCode(unsigned short* code, GRAPH graph, ADJACENCY adj, bitset* neighbours) {
    int i, j, codePosition;
    int edgeCounter = 0;
    
    if(code[0] > MAXN){
        fprintf(stderr, "Too many vertices -- exiting!\n");
        exit(EXIT_FAILURE);
    }

    graph[0][0] = code[0];
    codePosition = 1;

    for (i = 1; i <= code[0]; i++) {
        neighbours[i] = EMPTY_SET;
        for (j = 0; code[codePosition]; j++, codePosition++) {
            graph[i][j] = code[codePosition];
            ADD(neighbours[i], code[codePosition]);
            adj[i]++;
            edgeCounter++;
        }
        adj[i] = j;

        codePosition++; /* read the closing 0 */
    }
    
    if(edgeCounter != 6*code[0] - 12){
        //verify that the graph is a triangulation
        fprintf(stderr, "Not a triangulation -- exiting!\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * 
 * @param code
 * @param length
 * @param file
 * @return returns 1 if a code was read and 0 otherwise. Exits in case of error.
 */
int readPlanarCode(unsigned short code[], int *length, FILE *file) {
    static int first = 1;
    unsigned char c;
    char testheader[20];
    int bufferSize, zeroCounter;
    
    int readCount;


    if (first) {
        first = 0;

        if (fread(&testheader, sizeof (unsigned char), 13, file) != 13) {
            fprintf(stderr, "can't read header ((1)file too small)-- exiting\n");
            exit(1);
        }
        testheader[13] = 0;
        if (strcmp(testheader, ">>planar_code") == 0) {

        } else {
            fprintf(stderr, "No planarcode header detected -- exiting!\n");
            exit(1);
        }
        //read reminder of header (either empty or le/be specification)
        if (fread(&c, sizeof (unsigned char), 1, file) == 0) {
            return FALSE;
        }
        while (c!='<'){
            if (fread(&c, sizeof (unsigned char), 1, file) == 0) {
                return FALSE;
            }
        }
        //read one more character
        if (fread(&c, sizeof (unsigned char), 1, file) == 0) {
            return FALSE;
        }
    }

    /* possibly removing interior headers -- only done for planarcode */
    if (fread(&c, sizeof (unsigned char), 1, file) == 0) {
        //nothing left in file
        return (0);
    }

    if (c == '>') {
        // could be a header, or maybe just a 62 (which is also possible for unsigned char
        code[0] = c;
        bufferSize = 1;
        zeroCounter = 0;
        code[1] = (unsigned short) getc(file);
        if (code[1] == 0) zeroCounter++;
        code[2] = (unsigned short) getc(file);
        if (code[2] == 0) zeroCounter++;
        bufferSize = 3;
        // 3 characters were read and stored in buffer
        if ((code[1] == '>') && (code[2] == 'p')) /*we are sure that we're dealing with a header*/ {
            while ((c = getc(file)) != '<');
            /* read 2 more characters: */
            c = getc(file);
            if (c != '<') {
                fprintf(stderr, "Problems with header -- single '<'\n");
                exit(1);
            }
            if (!fread(&c, sizeof (unsigned char), 1, file)) {
                //nothing left in file
                return (0);
            }
            bufferSize = 1;
            zeroCounter = 0;
        }
    } else {
        //no header present
        bufferSize = 1;
        zeroCounter = 0;
    }

    if (c != 0) /* unsigned chars would be sufficient */ {
        code[0] = c;
        if (code[0] > MAXN) {
            fprintf(stderr, "Constant N too small %d > %lu \n", code[0], MAXN);
            exit(1);
        }
        while (zeroCounter < code[0]) {
            code[bufferSize] = (unsigned short) getc(file);
            if (code[bufferSize] == 0) zeroCounter++;
            bufferSize++;
        }
    } else {
        readCount = fread(code, sizeof (unsigned short), 1, file);
        if(!readCount){
            fprintf(stderr, "Unexpected EOF.\n");
            exit(1);
        }
        if (code[0] > MAXN) {
            fprintf(stderr, "Constant N too small %d > %lu \n", code[0], MAXN);
            exit(1);
        }
        bufferSize = 1;
        zeroCounter = 0;
        while (zeroCounter < code[0]) {
            readCount = fread(code + bufferSize, sizeof (unsigned short), 1, file);
            if(!readCount){
                fprintf(stderr, "Unexpected EOF.\n");
                exit(1);
            }
            if (code[bufferSize] == 0) zeroCounter++;
            bufferSize++;
        }
    }

    *length = bufferSize;
    return (1);


}

void writePlanarCodeHeader(FILE *f){
    fprintf(f, ">>planar_code<<");
}

void writeCode(FILE *f, unsigned short code[], int length){
    int i;
    for(i = 0; i < length; i++){
        putc(code[i], f);
    }
}