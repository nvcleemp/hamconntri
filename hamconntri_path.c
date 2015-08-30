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


bitset currentPath;
bitset connected[MAXN+1];

/**
  * 
  */
boolean continuePath(GRAPH graph, ADJACENCY adj, bitset *neighbours, int last, int targetVertex, int remaining, int unvisitedNeighboursOfTarget) {
    int i;
    
    if(remaining==1){
        return CONTAINS(neighbours[targetVertex], last);
    }
    
    if(!unvisitedNeighboursOfTarget){
        //the target vertex is no longer reachable
        return FALSE;
    }
    
    for(i = 0; i < adj[last]; i++){
        if(graph[last][i] != targetVertex && !CONTAINS(currentPath, graph[last][i])){
            ADD(currentPath, graph[last][i]);
            if(continuePath(graph, adj, neighbours, graph[last][i],
                    targetVertex, remaining - 1,
                    (CONTAINS(neighbours[targetVertex], graph[last][i]) ?
                        unvisitedNeighboursOfTarget - 1 :
                        unvisitedNeighboursOfTarget))){
                return TRUE;
            }
            REMOVE(currentPath, graph[last][i]);
        }
    }
    
    return FALSE;
}

boolean startPath(GRAPH graph, ADJACENCY adj, bitset *neighbours, int startVertex, int targetVertex, int order){
    int i;
    
    int unvisitedNeighboursOfTarget = adj[targetVertex];
    if(CONTAINS(neighbours[targetVertex], startVertex)){
        unvisitedNeighboursOfTarget--;
    }
    
    ADD(currentPath, startVertex);
    for(i = 0; i < adj[startVertex]; i++){
        if(graph[startVertex][i] != targetVertex){
            ADD(currentPath, graph[startVertex][i]);
            
            //search for path containing the edge (v, graph[v][i])
            if(continuePath(graph, adj, neighbours, graph[startVertex][i],
                    targetVertex, order - 2,
                    (CONTAINS(neighbours[targetVertex], graph[startVertex][i]) ?
                        unvisitedNeighboursOfTarget - 1 :
                        unvisitedNeighboursOfTarget))){
                return TRUE;
            }
            
            REMOVE(currentPath, graph[startVertex][i]);
        }
    }
    
    return FALSE;
}

boolean isHamiltonianConnected(GRAPH graph, ADJACENCY adj, bitset* neighbours){
    int i, j;
    
    //first we clear the information about which vertices are connected by a
    //hamiltonian path
    for(i = 1; i<=MAXN; i++){
        connected[i] = neighbours[i];
    }
    
    int order = graph[0][0];
    
    for(i=1; i<= order-1; i++){
        for(j=i+1; j<= order; j++){
            if(!CONTAINS(connected[i], j)){
                //clear old path
                currentPath = EMPTY_SET;
                
                if(!startPath(graph, adj, neighbours, i, j, order)){
                    return FALSE;
                }
            }
        }
    }
    
    return TRUE;
}

 //====================== USAGE =======================

void help(char *name) {
    fprintf(stderr, "The program %s checks whether a plane triangulation is hamiltonian-\nconnected. This program only works for triangulations with a path as decomposition tree.\n\n", name);
    fprintf(stderr, "Usage\n=====\n");
    fprintf(stderr, " %s [options]\n\n", name);
    fprintf(stderr, "Valid options\n=============\n");
    fprintf(stderr, "    -f, --filter\n");
    fprintf(stderr, "       Filter graphs that are hamiltonian-connected.\n");
    fprintf(stderr, "    -i, --invert\n");
    fprintf(stderr, "       Invert the filter.\n");
    fprintf(stderr, "    -h, --help\n");
    fprintf(stderr, "       Print this help and return.\n");
}

void usage(char *name) {
    fprintf(stderr, "Usage: %s [options]\n", name);
    fprintf(stderr, "For more information type: %s -h \n\n", name);
}

int main(int argc, char *argv[]) {

    /*=========== commandline parsing ===========*/
    boolean invert = FALSE;
    boolean filter = FALSE;
    int c;
    char *name = argv[0];
    static struct option long_options[] = {
        {"invert", no_argument, NULL, 'i'},
        {"filter", no_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'}
    };
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "hif", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                invert = TRUE;
                break;
            case 'f':
                filter = TRUE;
                break;
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
    if(filter){
        writePlanarCodeHeader(stdout);
    }
    while (readPlanarCode(code, &length, stdin)) {
        decodePlanarCode(code, graph, adj, neighbours);
        if(isHamiltonianConnected(graph, adj, neighbours)){
            numberOfHamiltonianConnected++;
            if(filter && !invert){
                writeCode(stdout, code, length);
            }
        } else {
            numberOfNotHamiltonianConnected++;
            if(filter && invert){
                writeCode(stdout, code, length);
            }
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
