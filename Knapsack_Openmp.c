#include<stdio.h>
#include <omp.h>
#define num 100
#define capacaity 280

int K[num+1][capacaity+1];

int max(int a, int b) { return (a > b)? a : b; }

int knapSack(int W, int wt[], int val[], int n)
{
   int i, w;
   //int K[n+1][W+1];
   int tmp1,tmp2;
 
   for (i = 0; i <= n; i++)
   {
       for (w = 0; w <= W; w++)
       {
           if (i==0 || w==0)
               K[i][w] = 0;
           else if (wt[i-1] <= w)
           {
                #pragma omp parallel sections
              {
		#pragma omp  section
		{
		  tmp1 = K[i-1][w];
		}
		#pragma omp  section
		{
		  tmp2 = val[i-1] + K[i-1][w-wt[i-1]];
		}
              }
              K[i][w] = max(tmp1,tmp2);
           }
           else
                 K[i][w] = K[i-1][w];
       }
   }
 
   return K[n][W];
}

int main(int argc, char** argv)
{
    int n,W;
    int val[num], wt[num];
    int result;

    n = num;
    printf("number of items: %d\n", n);

    W = capacaity;
    printf("capacaity of knapSack: %d\n", W);

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
    /*
    for(int i = 0; i < count; i++)
        printf("weights[%d] = %d, values[%d] = %d\n",i,wt[i],i,val[i]);
    */
    
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
          result = knapSack(W, wt, val, n);
        }
    }
    printf("the optimal value is %d\n", result);
    return 0;
}




