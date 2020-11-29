/* parallel quicksort on hypercube */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

MPI_Comm comm;
MPI_Comm newcomm;


void quicksort(int *A, int p, int r)
{
  if (p < r) {
    int q = partition(A, p, r);
    quicksort(A, p, q);
    quicksort(A, q+1, r);
  }
}

void print_array(int* A, int n)
{
  int i;
  for (i=0; i<n; i++) {
    printf("%d ", A[i]);
  }
  printf("\n") ;
}

int partition(int *A, int startidx, int endidx)
{
  //Takes an array and partitions it around a random pivot
  int randompivot, i, j;
  randompivot = A [ startidx + (int)( ((double)endidx-startidx)*rand()/(RAND_MAX + 1.0) ) ];

  i = startidx - 1;
  j = endidx + 1;
  
  //move values around pivot
  while (1) {
    do {
      j = j - 1;
    } while (A[j]>randompivot); 
    do {
      i = i + 1;
    } while (A[i]<randompivot);
    if (i<j) {
      int tmp;
      tmp = A[i];
      A[i] = A[j];
      A[j] = tmp;
    }
    else
      return j;
  }
}


int pivotpartition(int *A, int startidx, int endidx, int dimension)
{
  int randompivot, i, j, nprocs, id, color;
  int size = endidx-startidx;
  int *tmp;
  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&id);

  randompivot = A [ startidx + (int)( ((double)endidx-startidx)*rand()/(RAND_MAX + 1.0) ) ];

  // communicate pivot by taking avg
  int SUM = 0;
  MPI_Allreduce(&randompivot, &SUM, 1, MPI_INT, MPI_SUM, comm);
  randompivot = SUM/nprocs;
 
  //splitting the comm based on ith bit
  if (id & (1<<dimension))
    color = 1;
  else
    color = 0;

  MPI_Comm_split(comm, color, id,&newcomm) ;
  comm = newcomm;
  MPI_Comm_size(comm,&nprocs); 
  
  //move items around pivot
  i = startidx - 1;
  j = endidx + 1;
  while (1) {
    do {
      j = j - 1;
    } while (A[j]>randompivot);
    do {
      i = i + 1;
    } while (A[i]<randompivot);
    if (i<j) {
      int tmp;
      tmp = A[i];
      A[i] = A[j];
      A[j] = tmp;
    }
    else
      return j;
  }
}

void hypercube_quicksort(int *A, int n) {
  int processid;
  int i;
  int numprocs;
  int dimensions;
  int *buffer;			

  buffer = (int*)malloc(2*n*sizeof(int));

  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&processid);
 
  dimensions = log(numprocs) / log(2);

  for (i=dimensions-1; i>=0; i--) {
    int q = pivotpartition(A,0,n-1,i);
    int link = 1<<i;
    int b1size = q+1; 
    int b2size = n-(q+1); 
    int C1size,C2size,newsize;

    printf("%d: ",processid); 
    print_array(A, n);
    
    if (!(processid & link)) {	// ith bit 0 
      //send b2 (smaller values)
      //exchange sizes 
      
      MPI_Send(&b2size, 1, MPI_INT, processid^link, 0, MPI_COMM_WORLD);
      MPI_Recv(&C1size, 1, MPI_INT, processid^link, 0, MPI_COMM_WORLD,
	       MPI_STATUS_IGNORE);

      /* send b2 */
      MPI_Send(A+b1size, b2size, MPI_INT, processid^link, 0, MPI_COMM_WORLD);
      
      /* receive b1 */
      newsize = b1size + C1size;

      if (! (buffer = (int*)malloc(newsize*sizeof(int)))) {
        printf("alloc failed!!!!\n");
        exit(-1);
      }
      memcpy(buffer, A, (q+1)*sizeof(int));
      MPI_Recv(buffer+(q+1), C1size, MPI_INT, processid^link, 0,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      free(A);
      A = buffer;
      n = newsize;
      printf("%d: ",processid); 
      print_array(A, n);
    }
    else {
      //send b1 (larger values)

      MPI_Recv(&C2size, 1, MPI_INT, processid^link, 0, MPI_COMM_WORLD,
	       MPI_STATUS_IGNORE);
      MPI_Send(&b1size, 1, MPI_INT, processid^link, 0, MPI_COMM_WORLD);
    
      //receive 
      newsize = b2size + C2size;
      if (! (buffer = (int*)malloc(newsize*sizeof(int)))) {
        printf("malloc failed!!!!\n");
        exit(-1);
      }
      
      memcpy(buffer,A+b1size,b2size*sizeof(int));
      MPI_Recv(buffer+b2size, C2size, MPI_INT, processid^link, 0, MPI_COMM_WORLD,
	       MPI_STATUS_IGNORE);

      //send
      MPI_Send(A, b1size, MPI_INT, processid^link, 0, MPI_COMM_WORLD);
     

      free(A);
      A = buffer;
      n = newsize;
      
      
      printf("%d: ",processid); 
      print_array(A, n);
    }
  }
  
  quicksort(A,0,n-1); 
  printf("%d: sorted array: ", processid);
  print_array(A,n);
}

void construct_array(int *array, int total_size)
{
  int i ;
  
  srand(time(0));
  for (i=0; i < total_size; i++) {
    array[i] = (rand() % ((total_size * total_size) - 1));
  }
  
}

void main(int argc, char **argv) {
  int* unsortedarray;
  int procid, numprocs;
  int numelements;
  int total_size;
  double start,end;
  int dimensions ; 

  
  //total_size = 1000 ;
  total_size = atol(argv[1]) ;
  unsortedarray = (int*)malloc(total_size*sizeof(int));

  comm = MPI_COMM_WORLD;

  MPI_Init(&argc,&argv);

  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&procid);

  numelements = total_size/numprocs;
  dimensions = log( numprocs ) / log( 2 ) ;

  if(procid==0) {
    construct_array(unsortedarray, total_size); 
    printf ("hypercube has %d dimensions\n",dimensions);
    printf ("%d processors\n",numprocs);
    printf("Unsorted array populated with %d values\n", total_size) ;
    print_array(unsortedarray, total_size);
  }
  MPI_Scatter(unsortedarray, numelements, MPI_INT, unsortedarray, numelements, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  start = MPI_Wtime();

  hypercube_quicksort(unsortedarray, numelements);

  MPI_Barrier(MPI_COMM_WORLD);

  end = MPI_Wtime();
  if (procid==0) {
    printf("time=%f\n",end-start);
  }
  
  MPI_Finalize();

}
