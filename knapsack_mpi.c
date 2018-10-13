#include<stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define num 5000
#define capacaity 18000
//subblock size
#define row 64
#define column 512
//int K[num+1][capacaity+1];
#define exta_row 78
int result[num+1+exta_row][capacaity+1];

int max(int a, int b) { return (a > b)? a : b; }
int min(int i, int j) {
        return (i<j) ? i : j;
}

//knapSack(n, W, r, wt, val, i, rank, size);
// r 代表row， r = min(row,n-i);
// i的值用来判断是否为第一个block，传递给start变量
// rank是process的序号，rank=0代表主process
void knapSack(int n, int W, int r, int wt[], int val[], 
			 int start, int rank, int size)
{
   //int i, j;
   //int K[n+1][W+1];
   //int exta_row = n / 64;
   int t = start/64;
   
   int recv_rank = (rank-1)%size;   //rank to receive data
   int send_rank = (rank+1)%size; // rank to send data
   // deal with first block, r = 64，将problem按照row划分为几个小problem
   //since it doesn't receive data from any nodes
   if(start == 0)
   {	
   		int i, j; // temp value for loop function
   		//int result[r][W];
   		for(j = 0; j < W; j += column)
   		{
   		// cols用来决定node纵轴的工作区间,cols一般情况下等于column，为512，
    	// 最后一次等于 W-j,假如 W = 1200, 最后一次j = 1024, 那么cols = 176
   			int cols = min(column, W-j);
   			int k; 
   			for (k = j; k < j + cols; k++){
   				//如果item 0 的weight超过k，则为0
   				//如果没超过k，则为item 0的value值
   				if(wt[0] > k){
   					result[0][k] = 0;
   				}
   				else{
   					result[0][k] = val[0];
   				}
   			}
   		//上面计算的就是 i=0的情况,然后i从1到r-1
   		//compute subbloock
   			for (i = 1; i < r; i++)
   			{
   				for (k = j; k < j + cols; k++) 
   				{
   					if( k < wt[i] || result[i-1][k] >= result[i-1][k-wt[i]] + val[i])
   					{
   						result[i][k] = result[i-1][k];
   					}
   					else
   					{
   						result[i][k] = result[i-1][k-wt[i]] + val[i];
   					}
   						
   				}
   			}
   			//每计算完一个区间的值，j从0到512，从512到1024，就把最后一行的值发送给下一个node
   			MPI_Send(&result[65*t-2][j], cols, MPI_INT, send_rank, j, MPI_COMM_WORLD);
   		}
   	}
   	//如果不是第一个block的话
   	else
   	{
   		int i, j; //temp value for loop function
   		//int result[r+1][W];
   		for(j = 0; j < W; j += column)
   		{
   			int cols = min(column, W-j);
   			int k;
        //receive data from last node
        // use the first row to store the data from last node
        // e.g 当start = 64时，第二个block的第一行用来存储第一个block的最后一行
   			MPI_Recv(&result[65*t-1][j],cols,MPI_INT,recv_rank,j,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        
        int control_i = min(63,n-1-65*t); 
        for(i =65*t; i <= 65*t + control_i + exta_row; i++)
        {
          for (k = j; k < j + cols; k++)
          {
            //第一个block结束是在63行,第二个block的时候从64行开始
            //int prev_i = 65*t-1; 
            //例如计算第65行的weight，实际上对应的是wt[64]的数据
            //计算第130行的weight，实际上对应的是wt[128]的数据
            int actual_wt_index = 65*t-t; //
            if(k < wt[actual_wt_index] || result[i-1][k] >= result[i-1][k-wt[actual_wt_index]] + val[actual_wt_index])
            {
              result[i][k] = result[i-1][k];
            }
            else
            {
              result[i][k] = result[i-1][k-wt[i-actual_wt_index]] + val[actual_wt_index];
            }
          }
        }

        if (start + r == n + exta_row && j+cols == W)
        {
          printf("max profit: %d \n", result[n + exta_row-1][W-1]);
        }
      else if ((start + r) != (n + exta_row))
        {
          MPI_Send(&result[65*t-2][j], cols, MPI_INT, send_rank, j, MPI_COMM_WORLD);
        }
   		}
      
   	}
}


 

int main(int argc, char** argv)
{
    int n,W;
    int i;// 用来判断处理哪个block
    int val[num], wt[num];
    n = num;
    printf("number of items: %d\n", n);

    W = capacaity;
    printf("capacaity of knapSack: %d\n", W);

    // read data from input
    FILE *fp  = fopen(argv[1], "r");
    if(fp == NULL)
        return -1;
    int tmpA, tmpB, count = 0;
    while(fscanf(fp,"%d %d",&tmpA, &tmpB) != EOF){
        //printf("%d, %d\n", tmpA, tmpB);
        wt[count] = tmpA;
        val[count++] = tmpB;
    }
    fclose(fp);
    //end read data

    // begin the MPI part
    int size,rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //int exta_row = n / 64;
    for (i = 0; i < n + exta_row; i += row) {
    	// r用来决定node的工作区间,r一半情况下等于row，为64，
    	// 最后一次等于 n-i,假如 n = 200, 最后一次i = 192, 那么r=8
    	int r = min(row,n+exta_row-i);
    	// 如果i = 64, i/row=64/64=1 ,1%4 = 1
    	// size为process的数量
    	if((i/row) % size == rank)
    		knapSack(n, W, r, wt, val, i, rank, size); //第一次循环i=0
    }

    MPI_Finalize();
    

    
    /*
    for(int i = 0; i < count; i++)
        printf("weights[%d] = %d, values[%d] = %d\n",i,wt[i],i,val[i]);
    */
    
    //printf("the optimal value is %d\n", knapSack(W, wt, val, n));
    return 0;
}



