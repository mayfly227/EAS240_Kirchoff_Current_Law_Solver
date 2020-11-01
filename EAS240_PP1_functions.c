#include "EAS240_PP1_functions.h"
#include <stdlib.h>
#include <stdio.h>

void SolveCircuit(const Branch *circuit, double *voltages, int numBranches, int numNodes){

    //Using dynamic memory allocation for a new arry for matrix
    double **matrix = (double **)malloc(sizeof(double *) * numNodes);
    for (int i = 0; i < numNodes + 1; i++){
        matrix[i] = (double *)malloc(sizeof(double) * (numNodes + 1));
    }
    for (int i = 0; i < numNodes; ++i){
        //get souce startNode
        int sourceNode = i + 1;
        //Setting sourceValue is zero
        double sourceValue = 0;
        //get source value from Branch
        for (int k = 0; k < numBranches; ++k){
            if ((circuit + k)->type == 1 && (circuit + k)->startNode == sourceNode){
                sourceValue = (circuit + k)->value;
            }
        }
        //The last columns for matrix
        matrix[sourceNode - 1][numNodes] = sourceValue;
        //Three situations: 1.Resistors startnode is the same as the sourceNode 2.the resistors endnode is the same as sourceNode 3.the current endnode is the same as the source Node
        for (int j = 0; j < numBranches; ++j){
            //1.Resistors startnode is the same as the sourceNode
            if ((circuit + j)->type == 0 && (circuit + j)->startNode == sourceNode){
                //Getting startnode and endnode of element
                int startNode = (circuit + j)->startNode;
                int endNode = (circuit + j)->endNode;
                //Resistors connect to the ground and have same startnode with source, and getting coefficients for different node voltage
                if ((circuit + j)->endNode == 0){
                    matrix[sourceNode - 1][startNode - 1] =matrix[sourceNode - 1][startNode - 1] + (1 / (circuit + j)->value);
                }
                //Resistor connects to two different node, and getting coefficients for different node voltage
                if ((circuit + j)->endNode != 0){
                    matrix[sourceNode - 1][startNode - 1] =matrix[sourceNode - 1][startNode - 1] + (1 / (circuit + j)->value);
                    matrix[sourceNode - 1][endNode - 1] =matrix[sourceNode - 1][endNode - 1] - (1 / (circuit + j)->value);
                }
            }
            //2.the resistors endnode is the same as the sourceNode
            else if ((circuit + j)->type == 0 && (circuit + j)->endNode == sourceNode){
                //Getting startnode and endnode of element
                int startNode = (circuit + j)->startNode;
                int endNode = (circuit + j)->endNode;
                //Getting coefficients for different node voltage
                matrix[sourceNode - 1][startNode - 1]=matrix[sourceNode - 1][startNode - 1] - (1 / (circuit + j)->value);
                matrix[sourceNode - 1][endNode - 1] =matrix[sourceNode - 1][endNode - 1] + (1 / (circuit + j)->value);
            }
            //3.3.the current endnode is same as the source Node
            else if ((circuit + j)->type == 1 && (circuit + j)->endNode == sourceNode){
                //Entering node current minus leaving node current
                matrix[sourceNode - 1][numNodes] = matrix[sourceNode - 1][numNodes] - (circuit + j)->value;
            }
        }
    }
    //Using dynamic memory allocation create 2D array and store every value at matrix from cm to arr
    double *arr = (double *)malloc(sizeof(double) * numNodes * (numNodes + 1));
    for (int i = 0; i < numNodes; i++){
        for (int j = 0; j < numNodes + 1; ++j)
        {
            arr[i * (numNodes + 1) + j] = matrix[i][j];
        }
    }
    //Funcrtion call
    PerformGaussElimination(arr, numNodes, numNodes + 1);
    for (int i = 0; i < numNodes; ++i){
        voltages[i]  = arr[(i+1) * (numNodes+1)-1];
    }

}

void PrintCircuit(char filename[], const Branch *circuit, double *voltages, int numBranches, int numNodes){

    FILE* fp = fopen(filename, "a+");
    for (int i = 1; i <= numNodes; i++){
        //Print Voltage
        fprintf(fp,"V%d = %10.4lf\n", i, voltages[i - 1]);
        //print element starting as same node
        for (int j = 0; j < numBranches; j++){
            if ((circuit + j)->startNode == i){
                if ((circuit + j)->type == 1){
                    //Print sources
                    fprintf(fp,"Node %d - %10.2lf Amps - Node %d\n", i, (circuit + j)->value, (circuit + j)->endNode);
                }
                else{
                    //print resistors
                    fprintf(fp,"Node %d - %10.2lf Ohms - Node %d\n", i, (circuit + j)->value, (circuit + j)->endNode);
                }
            }
        }
    }
    fclose(fp);
}

void PerformGaussElimination(double *arr, int ROWS, int COLS){
    //Create 2D array using dynamic memory allocation
    double **a = (double **)malloc(COLS * sizeof(double));
    for (int i = 0; i < COLS; i++){
        a[i] = (double *)malloc(ROWS * sizeof(double));
    }
    double x;
    double y;
    //Map 1D array to 2D array through rows and columns mapping
    for (int i = 0; i < ROWS; ++i){
        for (int j = 0; j < COLS; ++j){
            a[i][j] = arr[i * COLS + j];
        }
    }
    //Calculate  Gauss-Jordan elimination
    for (int i = 0; i < ROWS; ++i){
        for (int j = 0; j < COLS; ++j){
            if (i != j){
                y = a[i][i];
                x = a[j][i] / y;
                for (int k = 0; k < COLS; ++k){
                    a[j][k] -= (a[i][k] * x);
                }
            }
        }
    }
    //Irrelevant numbers return to 0
    for (int i = 0; i < ROWS; ++i){
        for (int j = 0; j < COLS; ++j){
            if ((i != j) && (j != ROWS)){
                a[i][j] = 0;
            }
        }
    }
    //Return the diagonal of the matrix to 1, print and calculate final result
    for (int i = 0; i < ROWS; ++i){
        a[i][ROWS] /= a[i][i];
        a[i][i] = 1;
        // printf("V%d = %.4lf\n", i + 1, a[i][ROWS]);
        //save voltage to array for voltage
    }
    //Copy the final number from 2D array to 1D array
    for (int i = 0; i < ROWS; ++i){
        for (int j = 0; j < COLS; ++j){
            arr[i * COLS + j] = a[i][j];
        }
    }
    //Free memory for a
    free(a);
    //Return 1D array back to main function
}
