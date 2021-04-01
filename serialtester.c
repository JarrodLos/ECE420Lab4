/*
    Test the result stored in "data_output" against a serial implementation.

    -----
    Compiling:
    Include "Lab4_IO.c" to compile. Set the macro "LAB4_EXTEND" defined in the "Lab4_IO.c" file to include the extended functions
    $ gcc serialtester.c Lab4_IO.c -o serialtester -lm 

    -----
    Return values:
    0      result is correct
    1      result is wrong
    2      problem size does not match
    253    no "data_output" file
    254    no "data_input_meta" or "data_input_link" file
*/
#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

#define THRESHOLD 0.0001

int main (int argc, char* argv[]){
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

    // Load the data and simple verification
    if ((fp = fopen("data_output", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    } fscanf(fp, "%d\n%lf\n", &collected_nodecount, &error);

    /* For use with a custom data input */
    int testFile; 
    if (argc == 1) {
        testFile = 0;
        printf("Test File #0");
        if ((ip = fopen("data_input_meta","r")) == NULL) {
            printf("Error opening the data_input_meta file.\n");
            return 254;
        }
    } else {
        if (strcmp(argv[1], "1") == 0) {
            testFile = 1;
            printf("Test File #1");
            if ((ip = fopen("1112_nodes_input_meta","r")) == NULL) {
                printf("Error opening the 1112_nodes_input_meta file.\n");
                return 254;
            }
        } else if (strcmp(argv[1], "2") == 0) {
            testFile = 2;
            printf("Test File #2");
            if ((ip = fopen("5424_nodes_input_meta","r")) == NULL) {
                printf("Error opening the 5424_nodes_input_meta file.\n");
                return 254;
            }
        } else if (strcmp(argv[1], "3") == 0) {
            testFile = 3;
            printf("Test File #3");
            if ((ip = fopen("10000_nodes_input_meta","r")) == NULL) {
                printf("Error opening the 5424_nodes_input_meta file.\n");
                return 254;
            }
        } else {
            printf("Error reading custom input name.\n");
            return 254;
        }
    }
    // if ((ip = fopen("data_input_meta","r")) == NULL) {
    //     printf("Error opening the data_input_meta file.\n");
    //     return 254;
    // }
    
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
    
    /* Edited to support custom inputs */
    //if (node_init(&nodehead, 0, nodecount)) return 254;
    if (node_init(&nodehead, 0, nodecount, testFile)) return 254;

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
}
