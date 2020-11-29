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

int** initarray(int size){
    int** array = allocarray(size);
    int i, j;

    for (i = 0; i<size; i++){
        for (j = 0;  j<size; j++){
            array[i][j] = INT_MAX;
        }
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


int floydpipeline(int** graph, int K, int i, int j){
    int returnval ;
    if (graph[i][j] > (graph[i][K] + graph[K][j]) ){
        returnval= (graph[i][K] + graph[K][j]);
    }

    else{
        returnval = graph[i][j] ;
    }    
    return returnval ;
}

void buildgraph(int** adjgraph, char* filename, int nodes){
    FILE *fptr;
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
}

int main(int argc, char **argv) {
    int procid, numprocs;
    int size = atoi(argv[2]);
    int ** adjgraph = initarray(size) ;
    double t1, t2; 
    int i, j, val;

    MPI_Status status ;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&procid);

    
    if (numprocs != size){
        printf("Error, numprocs has to equal K") ;
        exit(0);
    }

    if(procid == numprocs -1){
        t1 = MPI_Wtime(); 
    }

    if(procid ==0){
        buildgraph(adjgraph, argv[1], size);
        for (i = 0; i<size ; i++){
            for(j = 0; j<size; j++){
                val = floydpipeline(adjgraph, procid, i, j) ;
                MPI_Send(&val, 1, MPI_INT,procid + 1, 0, MPI_COMM_WORLD);
                MPI_Send(&i, 1, MPI_INT, procid + 1, 0, MPI_COMM_WORLD);
                MPI_Send(&j, 1, MPI_INT, procid + 1, 0, MPI_COMM_WORLD);
                }
        }
    }
    else {
        while(adjgraph[size-1][size-1] == INT_MAX) {
            MPI_Recv(&val, size * size, MPI_INT, procid-1, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&i, 1, MPI_INT, procid-1, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&j, 1, MPI_INT, procid-1, 0, MPI_COMM_WORLD, &status);
            adjgraph[i][j] = val ;
        }

        for (i = 0; i<size ; i++){
            for(j = 0; j<size; j++){
                val = floydpipeline(adjgraph, procid, i, j) ;
                if (procid != (numprocs - 1)) { //only send if not last proc
                    MPI_Send(&val, 1, MPI_INT,procid + 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&i, 1, MPI_INT, procid + 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&j, 1, MPI_INT, procid + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
    }
    
    
    if(procid == (numprocs-1)){
        printf("Sol: \n");
        printarray(adjgraph, size);
        t2 = MPI_Wtime(); 
        printf( "Elapsed time is %f\n", t2 - t1 ); 
    }
    
    MPI_Finalize();
    

}