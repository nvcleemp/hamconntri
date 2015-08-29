/*
 * Main developer: Nico Van Cleemput
 *
 * Copyright (C) 2015 Nico Van Cleemput.
 * Licensed under the GNU AFFERO GPL, read the file LICENSE for details.
 */


#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "bitset.h"
#include "lw_plane_graph.h"

typedef int boolean;

#define FALSE 0
#define TRUE 1

boolean isHamiltonianConnected(GRAPH graph, ADJACENCY adj, bitset* neighbours){
    return FALSE;
}

 //====================== USAGE =======================

void help(char *name) {
    fprintf(stderr, "The program %s checks whether a plane triangulation is hamiltonian-\nconnected.\n\n", name);
    fprintf(stderr, "Usage\n=====\n");
    fprintf(stderr, " %s [options]\n\n", name);
    fprintf(stderr, "Valid options\n=============\n");
    fprintf(stderr, "    -h, --help\n");
    fprintf(stderr, "       Print this help and return.\n");
}

void usage(char *name) {
    fprintf(stderr, "Usage: %s [options]\n", name);
    fprintf(stderr, "For more information type: %s -h \n\n", name);
}

int main(int argc, char *argv[]) {

    /*=========== commandline parsing ===========*/

    int c;
    char *name = argv[0];
    static struct option long_options[] = {
         {"help", no_argument, NULL, 'h'}
    };
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                help(name);
                return EXIT_SUCCESS;
            case '?':
                usage(name);
                return EXIT_FAILURE;
            default:
                fprintf(stderr, "Illegal option %c.\n", c);
                usage(name);
                return EXIT_FAILURE;
        }
    }
    
    /*=========== process graphs ===========*/
    GRAPH graph;
    ADJACENCY adj;
    bitset *neighbours = (bitset *)malloc(sizeof(bitset)*MAXN);
    unsigned long long numberOfGraphs = 0ULL;
    unsigned long long numberOfHamiltonianConnected = 0ULL;
    unsigned long long numberOfNotHamiltonianConnected = 0ULL;
    unsigned short code[MAXCODELENGTH];
    int length;
    while (readPlanarCode(code, &length, stdin)) {
        decodePlanarCode(code, graph, adj, neighbours);
        if(isHamiltonianConnected(graph, adj, neighbours)){
            numberOfHamiltonianConnected++;
        } else {
            numberOfNotHamiltonianConnected++;
        }
        numberOfGraphs++;
    }
    
    fprintf(stderr, "Read %llu graph%s.\n", numberOfGraphs, 
                numberOfGraphs==1 ? "" : "s");
    
    fprintf(stderr, "   %llu graph%s %s hamiltonian-connected.\n",
                numberOfHamiltonianConnected, 
                numberOfHamiltonianConnected==1 ? "" : "s", 
                numberOfHamiltonianConnected==1 ? "was" : "were");
    fprintf(stderr, "   %llu graph%s %s not hamiltonian-connected.\n",
                numberOfNotHamiltonianConnected, 
                numberOfNotHamiltonianConnected==1 ? "" : "s", 
                numberOfNotHamiltonianConnected==1 ? "was" : "were");
    

    return EXIT_SUCCESS;
}
