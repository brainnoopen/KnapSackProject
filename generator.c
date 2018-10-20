#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    fp = fopen("2000in","w+");
    

    srand(time(NULL));

    int objectsNumber = 2000  ;
    //int objectsNumber = 20 * 2;
    int knapsackSize = 8000 ;
    //int knapsackSize = 1024 * 1024 * 10;
    printf("%d %d\n", objectsNumber, knapsackSize);
    while(objectsNumber--)
    {
        //int v = rand() % 128 + 1;
        int v = rand() % 20 + 1;
        int w = rand() % 6 + 1;
        //int w = rand() % 512 + 1;
        // write to file vs write to screen 
        fprintf(fp, "%d %d\n", w, v); // write to file 
        //printf("%d %d\n", w, v);
    }

    return 0;
}
