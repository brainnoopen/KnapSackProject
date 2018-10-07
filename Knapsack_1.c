#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int knapsack(int, int); //Returns the required optimal value by creating part of the Memory Function  
int max(int, int);

#define n 40
#define num 200

int weights[n]; //Array containing weights of all items
int values[n]; //Array containing value of all items
int W = num; //Capacity of KnapSack
int F[n+1][num]; //Memory Function

int main(int argc, char** argv)
{


	FILE *fp  = fopen(argv[1], "r");
    if(fp == NULL)
        return -1;
    int tmpA, tmpB, count = 1;
    while(fscanf(fp,"%d %d",&tmpA, &tmpB) != EOF){
        printf("%d, %d\n", tmpA, tmpB);
        weights[count] = tmpA;
        values[count++] = tmpB;
    }
    
    for(int i = 1; i < count; i++)
        printf("weights[%d] = %d, values[%d] = %d\n",i,weights[i],i,values[i]);
      

    fclose(fp);

    //scanf("%d", &W);

	int i, j;
	

	for(i=0;i<=W;i++)
		F[0][i] = 0;
	for(i=0;i<=n;i++)
		F[i][0] = 0;
	for(i=1;i<=n;i++)
	{
		for(j=1;j<=W;j++)
			F[i][j] = -1;
	}

	/*if{debug = 1}{
		for(i=0;i<=n;i++)
		{
			for(j=0;j<=W;j++)
				printf("%d", F[i][j]);
		printf("\n");
		}
	}*/

	int result = knapsack(n, W); //Invoke Memory Function
	
	printf("\n\t--------\t");	
	for(j=1;j<=W;j++)
	{
			printf("-------");	
	}
	
	printf("\n");
	for(i=1;i<=n;i++)
	{
		printf("\t%d\t|\t",i);
		for(j=1;j<=W;j++)
		{
			printf("%d\t",F[i][j]);			
		}
		printf("\n");
	}
	printf("Optimal value= %d\n", result);
	return 0;
}


int knapsack(int i, int j)
{
	int value;
	if(F[i][j] < 0)
	{
		if(j < weights[i])  //If weight of item is more than current capacity
			value = knapsack(i-1, j); //Value of previous item
		else
			value = max(knapsack(i-1, j), (values[i] + knapsack(i-1, j - weights[i]))); //Include it and compare value with case of not including it
		F[i][j] = value;
	}
	return F[i][j];
}

int max(int a, int b)
{
	return (a>b?a:b);
}