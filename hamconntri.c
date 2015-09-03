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
#include "boolean.h"

bitset currentPath;
int pathSequence[MAXN];
int pathPosition[MAXN+1];
int pathLength;
bitset connected[MAXN+1];

void derivePathDepth1_fromStart(GRAPH graph, ADJACENCY adj, bitset *neighbours,
        int xi, int xi1, int end){
    int i;
    
    int xi1Pos = pathPosition[xi1];
    
    for(i = 0; i < adj[xi1]; i++){
        int yi = graph[xi1][i];
        if(yi!=xi){
            int direction = (pathPosition[yi] < xi1Pos) ? 1 : -1;
            int yi1 = pathSequence[pathPosition[yi]+direction];
            ADD(connected[yi1], end);
            ADD(connected[end], yi1);
        }
    }
    
    for(i = 0; i < adj[end]; i++){
        int yi = graph[end][i];
        if(yi!=pathSequence[0]){
            //the other case is already handled by ham cycle
            int direction = (pathPosition[yi] < xi1Pos) ? -1 : 1;
            int yi1 = pathSequence[pathPosition[yi]+direction];
            ADD(connected[yi1], xi1);
            ADD(connected[xi1], yi1);
        }
    }
}

void derivePathDepth1_fromEnd(GRAPH graph, ADJACENCY adj, bitset *neighbours,
        int xi, int xi1, int start){
    int i;
    
    int xi1Pos = pathPosition[xi1]; //position of x_{i+1}
    
    for(i = 0; i < adj[xi1]; i++){
        int yi = graph[xi1][i];
        if(yi!=xi){
            //direction on original path of y_{i-1} with respect to y_i
            int direction = (pathPosition[yi] < xi1Pos) ? 1 : -1;
            int yi1 = pathSequence[pathPosition[yi]+direction]; //y_{i-1}
            ADD(connected[yi1], start);
            ADD(connected[start], yi1);
        }
    }
    
    for(i = 0; i < adj[start]; i++){
        int yi = graph[start][i];
        if(yi!=pathSequence[pathLength-1]){
            //the other case is already handled by ham cycle
            
            //direction on original path of y_{i+1} with respect to y_i
            int direction = (pathPosition[yi] < xi1Pos) ? -1 : 1;
            int yi1 = pathSequence[pathPosition[yi]+direction]; //y_{i+1}
            ADD(connected[yi1], xi1);
            ADD(connected[xi1], yi1);
        }
    }
}

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
    
    //for each neighbour of the start vertex, we can find a new hamiltonian path
    for(i = 0; i < adj[start]; i++){
        //determine the position of the vertex x_i
        int pos_i = pathPosition[graph[start][i]];
        if(pos_i>1){
            int xi1 = pathSequence[pos_i-1]; //the vertex x_{i-1}
            //there is a hamiltonian path from xi1 to end
            ADD(connected[xi1], end);
            ADD(connected[end], xi1);
            derivePathDepth1_fromStart(graph, adj, neighbours, graph[start][i], xi1, end);
        }
    }
    
    //for each neighbour of the end vertex, we can find a new hamiltonian path
    for(i = 0; i < adj[end]; i++){
        //determine the position of the vertex x_i
        int pos_i = pathPosition[graph[end][i]];
        if(pos_i<pathLength-2){
            int xi1 = pathSequence[pos_i+1]; //the vertex x_{i+1}
            //there is a hamiltonian path from xi1 to start
            ADD(connected[xi1], start);
            ADD(connected[start], xi1);
            derivePathDepth1_fromEnd(graph, adj, neighbours, graph[end][i], xi1, start);
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
            pathPosition[targetVertex] = pathLength;
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
            pathPosition[graph[last][i]] = pathLength;
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
    pathPosition[startVertex] = 0;
    for(i = 0; i < adj[startVertex]; i++){
        if(graph[startVertex][i] != targetVertex){
            ADD(currentPath, graph[startVertex][i]);
            pathSequence[1] = graph[startVertex][i];
            pathPosition[graph[startVertex][i]] = 1;
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
    
    //next we sort the vertices according to there degree
    //we just use a radix sort for now
    //the maximum degree is at most order - 1, and the minimum
    //degree is at least 3.
    int sorted[MAXN];
    int sortedPosition = 0;
    for(i = order - 1; i >= 3; i--){
        for(j = 1; j <= order; j++){
            if(adj[j]==i){
                sorted[sortedPosition++] = j;
            }
        }
    }
    
    for(i=0; i < order-1; i++){
        for(j=i+1; j< order; j++){
            int v1 = sorted[i];
            int v2 = sorted[j];
            if(!CONTAINS(connected[v1], v2)){
                //clear old path
                currentPath = EMPTY_SET;
                
                if(!startPath(graph, adj, neighbours, v1, v2, order)){
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
