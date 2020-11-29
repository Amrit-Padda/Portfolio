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
            if (i==j){
                array[i][j] = 0 ;
            }
            else{
                array[i][j] = INF;
            }
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


void floydparallel(int** graph, int** return_graph, int K, int i_val, int j_val){
    if (graph[i_val][j_val] > (graph[i_val][K] + graph[K][j_val]) ){
        return_graph[i_val][j_val] = (graph[i_val][K] + graph[K][j_val]);
    }

    else{
        return_graph[i_val][j_val] = graph[i_val][j_val] ;
    }    
    
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
    int ** adjgraph = allocarray(size) ;
    int ** resultarray = initarray(size) ;
    int K;
    int istart, jstart;

    double t1, t2; 

    MPI_Status status ;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&procid);

    int numcells = sqrt((size * size)/numprocs) ;
    if (numprocs % 2 != 0){
        printf("Error, numprocs must be even") ;
        exit(0);
    }

    if(procid==0) {
        t1 = MPI_Wtime();
        buildgraph(adjgraph, argv[1], size);
        int i, j;
        int proc = 0 ;
        
        for (i = 0; i<size ; i+=numcells){
            for(j = 0; j<size; j+=numcells){
                MPI_Send(&adjgraph[0][0], size * size, MPI_INT,proc, 0, MPI_COMM_WORLD);
                MPI_Send(&i, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
                MPI_Send(&j, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
                proc++ ;
                }
        }
        istart = 0;
        jstart = 0;
        
    }
    else {
        MPI_Recv(&adjgraph[0][0], size * size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&istart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&jstart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    for(K=0;K<size;K++){
        int i, j;
        int iend = istart + numcells;
        int jend = jstart + numcells;

        MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for(i=istart; i<iend; i++){
            for(j=jstart; j<jend; j++){
                floydparallel(adjgraph, resultarray, K, i, j);
            }
        }
        
        MPI_Allreduce(&resultarray[0][0], &adjgraph[0][0], size * size, MPI_INT, MPI_MIN, MPI_COMM_WORLD);  
    }
    
    if(procid == 0){
        printf("Sol: \n");
        printarray(adjgraph, size);
        t2 = MPI_Wtime(); 
        printf( "Elapsed time is %f\n", t2 - t1 ); 
    }
    
    MPI_Finalize();
    

}