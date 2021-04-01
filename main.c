/* File:        main.c
 * Authors:     Jarrod Los, Andrew Williams & Andrew Dearborn
 * Date:        March 31, 2021
 *
 * Input:       
 * Output:      Creates file "data_output"
 * 
 * Description: Implementation of PageRank via MPI that can run multiple processes on a single 
 *              process or on a cluster of computers.
 * 
 * References:  The algorithm in the lab manual, as well as code in serial tester and lecture examples
 *              for initialization, creation and message passing between processes was used to supplement
 *              our implementation of PageRank.
 */

#define LAB4_EXTEND
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Lab4_IO.h"
#include "timer.h"
#include <string.h>

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001

int main(int argc, char *argv[]) {
    
    int my_rank, comm_sz;
    struct node *nodehead;
    int nodecount, local_StartIndex, local_EndIndex;
    int partition, padded_length;
    double *r, *r_pre, *contribution, *local_r, *local_contribution;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    FILE *ip;
    double start, end;

    /* Init, Get rank for each process & Find total # of processes used */
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Open the meta data file created from datatrim.c & pull out the node count */
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 254;
    } 
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);
    
    /* Update this to the original node_init() & fix header file as well */
    if (node_init(&nodehead, 0, nodecount)) return 254;
    
    /* Calculate partition size for each process */
    partition = (int) nodecount/comm_sz;
    if (partition * comm_sz != nodecount)
        partition += 1;
    padded_length = partition * comm_sz;

    /* Initialize variables local to process creating the shared buffers for message passing */
    r = malloc(padded_length * sizeof(double));
    r_pre = malloc(padded_length * sizeof(double));
    local_r = malloc(partition * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    
    contribution = malloc(padded_length * sizeof(double));
    local_contribution = malloc(partition * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;
    
    /* The core of the calculation & the main focus of our time & performance measurements */
    GET_TIME(start);
    do{
        ++iterationcount;
        vec_cp(r, r_pre, nodecount);
        
        /* Partition work for each process */
        local_StartIndex = my_rank * partition;
        local_EndIndex   = (local_StartIndex + partition) - 1;
        
        /* Update the value for our local r */
        for ( i = local_StartIndex; (i < nodecount) && (i <= local_EndIndex); ++i) {
            local_r[i - local_StartIndex] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                local_r[i - local_StartIndex] += contribution[nodehead[i].inlinks[j]];
            local_r[i - local_StartIndex] += damp_const;
        }

        /* Update the value for our local contribution */
        for ( i=local_StartIndex; (i < nodecount) && (i <= local_EndIndex); ++i){
            local_contribution[i - local_StartIndex] = local_r[i - local_StartIndex] / nodehead[i].num_out_links * DAMPING_FACTOR;
        }

        /* 'Gather' all data from each process & share to all */
        MPI_Allgather(local_r, partition, MPI_DOUBLE, r, partition, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgather(local_contribution, partition, MPI_DOUBLE, contribution, partition, MPI_DOUBLE, MPI_COMM_WORLD);
    }
    while(rel_error(r, r_pre, nodecount) >= EPSILON);
    GET_TIME(end);
    
    /* Shut down MPI */
    MPI_Finalize();

    /* Have the master output the desired data */
    if (my_rank == 0) {
        printf("Program converged at %d th iteration.\nElapsed time %f.\n", iterationcount, end-start);

        /* Save the data to file for Lab 4 */
        double end_time = end - start;
        Lab4_saveoutput(r, nodecount, end_time);
    }

    /* Free our initial variables */
    node_destroy(nodehead, nodecount);
    free(contribution);
    free(local_contribution);
    free(r);
    free(r_pre);
    free(local_r);

    return 0;
}
