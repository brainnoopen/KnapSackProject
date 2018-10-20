#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

//subblock size
#define constant_row 50
#define constant_col 150

void item_generator(int n, int wt[n], int val[n]) 
{
    int i = 0;
    
    for (i = 0; i < n; i++) {
        wt[i] = (random() % 6) + 1;
        val[i] = (random() % 10) + 1;
    }
}

int min(int i, int j) {
        return (i<j) ? i : j;
}

void knapsack(int n, int weight, int sub_row, int wt[sub_row], int val[sub_row], 
            int begin, int rank, int size) {
        int recv_rank = (rank-1)%size;   //rank to receive data
        int send_rank = (rank+1)%size; // rank to send data
        if( begin == 0 ) {      // deal with first block, since it doesn't receive data from any nodes
                int result[sub_row][weight];                
                int i, j;
        
                for (j = 0; j < weight; j += constant_col) 
                {
                        int sub_col = min(constant_col, weight-j);
                        int k;
                        for (k = j; k < j + sub_col; k++) 
                        {
                                if (wt[0] > k) {
                                        result[0][k] = 0;
                                } else {
                                        result[0][k] = val[0];
                                }        
                        }
                        //compute subblock
                        for (i = 1; i < sub_row; i++) 
                        {
                                for (k = j; k < j + sub_col; k++) 
                                {
                                        if ( (k<wt[i]) || 
                                             (result[i-1][k] >= result[i-1][k-wt[i]] + val[i])) {
                                                result[i][k] = result[i-1][k];
                                        } else {
                                                result[i][k] = result[i-1][k-wt[i]] + val[i];
                                        }
                                }       
                        }
                        //send last row to next node
                        MPI_Send(&result[sub_row-1][j], sub_col, MPI_INT, send_rank, j, MPI_COMM_WORLD);
                }

        } 
        else {
                int result[sub_row+1][weight];  // use the first row to store the data from last node
                int i, j;
                for (j = 0; j < weight; j += constant_col) 
                {
                        int sub_col = min(constant_col, weight-j);
                        int k;
                        // receive data from last node
                        MPI_Recv(&result[0][j], sub_col, MPI_INT, recv_rank, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        for (i = 1; i <= sub_row; i++) 
                        {
                                for (k = j; k < j + sub_col; k++) {
                                        int prev_i = i-1;  //prev_i is index for wt and val
                                        if ( (k<wt[prev_i]) || 
                                             (result[i-1][k] >= result[i-1][k-wt[prev_i]] + val[prev_i])) {
                                                result[i][k] = result[i-1][k];
                                        } else {
                                                result[i][k] = result[i-1][k-wt[prev_i]] + val[prev_i];
                                        }
                                }
                        }
            
                    if (begin + sub_row == n && j + sub_col == weight) {
                                //computer the last subblock of last row, print the final result 
                                printf("max val: %d \n", result[sub_row][weight-1]);
                    } else if (begin + sub_row != n){
                                // if it is not last row, we need send data to next node.
                                MPI_Send(&result[sub_row][j], sub_col, MPI_INT, send_rank, j, MPI_COMM_WORLD);
                    }
                }
            }
}



int main(int argc, char *argv[]) {
    int i, n = 0;
    int W;
    unsigned long long usec;
    struct timeval tstart, tend;

    if (argc > 2 && argc < 4) {
        n = atoi(argv[1]);      /* Number of things */
        W = atoi(argv[2]);      /* Capacity of knapsack */
    }
    if ((n <= 0) || (W <= 0) ) {
        printf("number items and Capacity need greater than 0 \n");
        exit(1);
    }
    
    
    int size, rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    gettimeofday(&tstart, NULL); //start time

    for (i = 0; i < n; i += constant_row) {
                int sub_row  = min(constant_row, n-i);
        int *wt = malloc(sub_row * sizeof(int));   //initial array wt locally
        int *val = malloc(sub_row * sizeof(int));   //initial array val locally
        item_generator (sub_row, wt, val);
                if ((i/constant_row) % size == rank)
                        knapsack(n, W, sub_row, wt, val, i, rank, size);  //solve subblock
        free(wt);
        free(val);
        }

    gettimeofday(&tend, NULL); //end time

    if (tend.tv_usec > tstart.tv_usec) 
    {
        usec = (tend.tv_sec - tstart.tv_sec) * 1000000
            + tend.tv_usec - tstart.tv_usec;
    } 
    else 
    {
        usec = (tend.tv_sec - (tstart.tv_sec + 1)) * 1000000
            + (1000000 + tend.tv_usec - tstart.tv_usec);
    }

    if (rank == 0) 
    {
        fprintf(stdout, "%d * %d in %d nodes finished in %f seconds.\n", n, W, size, (double)usec/1000000.0);
    }

    MPI_Finalize();
    
}