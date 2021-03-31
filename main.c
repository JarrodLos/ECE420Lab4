/* File:        main.c
 * Authors:     Jarrod Los, Andrew Williams & Andrew Dearborn
 * Date:        March 31, 2021
 *
 * Input:       
 * Output:      
 * 
 * Description: 
 * 
 * References:  
 */


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001

int main(void) {
    int my_rank, comm_sz;
    struct node *nodehead;
    int nodecount;
    double *r, *r_pre, *contribution;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    int collected_nodecount;
    double *collected_r;
    double cst_addapted_threshold;
    double error;
    FILE *fp, *ip;

    /* Init, Get rank for each process & Find total # of processes used */
    // MPI_Init(NULL, NULL);
    // MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Load the data and simple verification
    if ((fp = fopen("data_output", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    }
    fscanf(fp, "%d\n%lf\n", &collected_nodecount, &error);
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 254;
    }
    fscanf(ip, "%d\n", &nodecount);
    if (nodecount != collected_nodecount){
        printf("Problem size does not match!\n");
        return 2;
    }
    fclose(ip);
    collected_r = malloc(collected_nodecount * sizeof(double));
    for ( i = 0; i < collected_nodecount; ++i)
        fscanf(fp, "%lf\n", &collected_r[i]);
    fclose(fp);

    // Adjust the threshold according to the problem size
    cst_addapted_threshold = THRESHOLD;
    
    if (node_init(&nodehead, 0, nodecount)) return 254;
    // initialize variables
    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    contribution = malloc(nodecount * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;
    // CORE CALCULATION
    // GET_TIME(start);
    do{
        ++iterationcount;
        vec_cp(r, r_pre, nodecount);
        // update the value
        for ( i = 0; i < nodecount; ++i){
            r[i] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                r[i] += contribution[nodehead[i].inlinks[j]];
            r[i] += damp_const;
        }
        // update and broadcast the contribution
        for ( i=0; i<nodecount; ++i){
            contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
        }
    }while(rel_error(r, r_pre, nodecount) >= EPSILON);
    // GET_TIME(end);
    // printf("Program converged at %d th iteration.\nElapsed time %f.\n", iterationcount, end-start);

    // post processing
    node_destroy(nodehead, nodecount);
    free(contribution);
    
    // Compare the result 
    error = rel_error(collected_r, r, nodecount);
    printf ("The relative error against the reference result is %e .\n", error);
    free(r); free(r_pre); free(collected_r);
    if ( error < cst_addapted_threshold){
        printf("Congratulations! Your result is correct!\n");
        return 0;
    }else{
        printf("Sorry. Your result is wrong.\n");
        return 1;
    }

    /* Shut down MPI */
    //MPI_Finalize();

    return 0;
}
