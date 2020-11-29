#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define INF 99999 

int ** allocarray(int size){
    int *data = (int *)malloc(size*size*sizeof(int));
    int** array = (int**)malloc(size * sizeof(int *)); 
    
    int i; 
    for (i=0; i<size; i++){
        array[i] = &(data[size*i]);
    }
    return array ;
}

void printarray(int ** array, int size){
    int i,j;
    for(i=0;i<size;i++) {
        for(j=0;j<size;j++) {
            printf("%3d ",array[i][j]);
        }
        printf("\n");
    }
}
int** floyditer(int** graph, int K, int size ){
    int** D = allocarray(size) ;
    int i,j;      
    for(i=0; i<size; i++){ //k=0 is already given
        for(j=0; j<size; j++){
            if (graph[i][j] > (graph[i][K] + graph[K][j]) ){
                D[i][j] = (graph[i][K] + graph[K][j]);
            }

            else{
                D[i][j] = graph[i][j] ;
            }     
        }
    }

    return D ;
}

int** floyd(int** graph, int size){
    int** startgraph = graph; //for first iteration we want to pass the given adjacency graph
    int** returnedgraph ;
    int i;

    for (i=0; i<size;i++){
        returnedgraph = floyditer(startgraph, i, size);
        startgraph = returnedgraph ;
    }

    return returnedgraph ;
}

int** buildgraph(char* filename, int nodes){
    FILE *fptr;
    int ** adjgraph = allocarray(nodes) ;
    char line[256];
    char * pch;
  
    if ((fptr = fopen(filename, "r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    
    int rowctr = 0 ;
    int columnctr = 0;

    while (fgets(line, sizeof(line), fptr)) {
        pch = strtok (line," ,\r\n");
        while (pch != NULL)
        {            
            if (strcmp(pch, "inf") == 0){
                adjgraph[rowctr][columnctr] = INF;
            }
            else {
                adjgraph[rowctr][columnctr] = atoi(pch) ;
            }
                
            columnctr++;
            pch = strtok (NULL, " ,\r\n");
        }
        
        rowctr++;
        columnctr = 0 ;
        
    }

    fclose(fptr);
    return adjgraph ;
}
int main(int argc, char **argv) {
    int procid, numprocs;
    int ** adjgraph;
    int ** result;
    int size = atoi(argv[2]);
    double t1, t2; 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&procid);

    if(procid==0) {
        t1 = MPI_Wtime();
        adjgraph = buildgraph(argv[1], size);
        result = floyd(adjgraph, size);
        printf("Output array: \n") ;
        printarray(result, size);
        t2 = MPI_Wtime(); 
        printf( "Elapsed time is %f\n", t2 - t1 ); 
    }
    MPI_Finalize();
    

}