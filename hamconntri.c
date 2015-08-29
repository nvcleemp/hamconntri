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
int pathSequence[MAXN];
int pathLength;
bitset connected[MAXN+1];

void foundPath(GRAPH graph, ADJACENCY adj, bitset *neighbours){
    int i;
    int start = pathSequence[0];
    int end = pathSequence[pathLength-1];
    
    //start and end are connected by a hamiltonian path
    ADD(connected[start], end);
    ADD(connected[end], start);
    
    if(CONTAINS(neighbours[start], end)){
        //we found a hamiltonian cycle
        //all adjacent vertices on the cycle are hamiltonian-connected
        for(i = 1; i < pathLength; i++){
            int v1 = pathSequence[i-1];
            int v2 = pathSequence[i];
            ADD(connected[v1], v2);
            ADD(connected[v2], v1);
        }
    }
}

/**
  * 
  */
boolean continuePath(GRAPH graph, ADJACENCY adj, bitset *neighbours, int last, int targetVertex, int remaining, int unvisitedNeighboursOfTarget) {
    int i;
    
    if(remaining==1){
        if(CONTAINS(neighbours[targetVertex], last)){
            pathSequence[pathLength] = targetVertex;
            pathLength++;
            foundPath(graph, adj, neighbours);
            return TRUE;
        } else {
            return FALSE;
        }
    }
    
    if(!unvisitedNeighboursOfTarget){
        //the target vertex is no longer reachable
        return FALSE;
    }
    
    for(i = 0; i < adj[last]; i++){
        if(graph[last][i] != targetVertex && !CONTAINS(currentPath, graph[last][i])){
            ADD(currentPath, graph[last][i]);
            pathSequence[pathLength] = graph[last][i];
            pathLength++;
            if(continuePath(graph, adj, neighbours, graph[last][i],
                    targetVertex, remaining - 1,
                    (CONTAINS(neighbours[targetVertex], graph[last][i]) ?
                        unvisitedNeighboursOfTarget - 1 :
                        unvisitedNeighboursOfTarget))){
                return TRUE;
            }
            pathLength--;
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
    pathSequence[0] = startVertex;
    for(i = 0; i < adj[startVertex]; i++){
        if(graph[startVertex][i] != targetVertex){
            ADD(currentPath, graph[startVertex][i]);
            pathSequence[1] = graph[startVertex][i];
            pathLength = 2;
            
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
        connected[i] = EMPTY_SET;
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
