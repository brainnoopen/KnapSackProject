#include<stdio.h>
#define num 10000
#define capacaity 30000

int K[num+1][capacaity+1];

int max(int a, int b) { return (a > b)? a : b; }

int knapSack(int W, int wt[], int val[], int n)
{
   int i, w;
   //int K[n+1][W+1];
 
   for (i = 0; i <= n; i++)
   {
       for (w = 0; w <= W; w++)
       {
           if (i==0 || w==0)
               K[i][w] = 0;
           else if (wt[i-1] <= w)
                 K[i][w] = max(val[i-1] + K[i-1][w-wt[i-1]],  K[i-1][w]);
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
    
    printf("the optimal value is %d\n", knapSack(W, wt, val, n));
    return 0;
}




