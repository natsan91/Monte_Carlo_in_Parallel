/*
  
  Program written by Nathan Sanford
  
  Performs Monte Carlo Integration trials in a parallel environment using MPI. 
  The trials are distributed evenly amongst the processors. Each trial 
  consists of taking a number of samples and using a recurrence relation
  to calculate the  mean
                 mean[n]=((n-1)/n)*mean[n-1] + (1/n)*g(x_n)
  Each processor is seeded by taking the computer time in the first processor 
  at start time and then adding the rank of the processor.

  Inputs: Names of 2 text files. The first is an input file which consists 
          of the number of samples per trial and the number of samples. The 
		  second is for an output file to which the result of each trial is
		  written.
  Outputs: Writes the number of trials and the result of each trial to the 
           output file. Additionally, writes the total simulation time to 
		   the terminal at the end of simulation.

  Editing History:
  5/3/2016 Initial Draft
  5/9/2016 Added comments
  11/6/2019 Edited filename to distinguish that this code is parallel
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

/*
  double f(double x)
  
  The function we're averaging in the monte carlo simulation.

  Inputs:
  double x: double drawn from exponential distribution

  Outputs:
  double f: cos(x) or the functional value of the random number for MC 
            simulation
*/
double f(double x)
{
  return cos(x);
}

/*
  double p(double x)
  
  The function which gives the sample drawn from the exponential distribution.

  Inputs:
  double x: double drawn from uniform distribution on [0,1)

  Outputs:
  double p: double obtained by performing the transformation -log(x)
*/
double p(double x)
{
  return -log(x);
}

/*
  double mcquad(int N)

  Performs one trial of Monte Carlo integration with N samples and returns the 
  mean.

  Inputs: 
  int N: number of samples

  Outputs:
  double mcquad: Mean of mc simulations

*/

double mcquad(int N)
{
  // ------------------variable declarations-------------------
  double z, y, x; // temporary variables used in drawing samples
  double  mean=0;// variance=0; // for storing mean and variance
  // ----------------------------------------------------------
  // loop through samples
  for (int n=1; n<=N; n++){
    z=drand48(); //uniform RN
    y=p(z); //exponential RN
    x=f(y); //sample
    mean+=x;
  }
  mean*=(1.0/N);
  return mean;
}

/*
  int main( int argc , char* argv [])

  The main program gets the number of trials and the number of samples 
  for each trial. It then distributes the trials to the available processors 
  and initializes the seed on each processor. It then gathers the results 
  from each trial and writes the results to the output file. It calls srand48 
  to set the initial seeds and uses the clock time on the first processor plus 
  the process rank as the seed.

  Inputs: should be 3, just function name and input and output text file names

  Outputs: Prints the number of trials and the results of each trial to the
           output file. Additionally prints to the terminal screen the
           time elapsed. 
*/

int main(int argc, char* argv[])
{  
  MPI_Init(&argc,&argv);// initialize MPI

  // ---------------------------Variable Declarations---------------------
  int rank, size, N, T, numruns;
  double* results; // for storing results of all the trials
  time_t mytime;
  long initseed;
  // ---------------------------------------------------------------------

  // Determine the rank of the current process
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  // Determine the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  // start keeping track of time for efficiency tests
  double precision = MPI_Wtick();
  double starttime = MPI_Wtime();

  // rank 0 process reads the input file 
  if (rank==0){ 
    if (argc != 3) {
      printf("Incorrect usage: enter the input and output data file names\n");
      return 0;
    }
    FILE* inputfile = fopen(argv[1], "r"); // inputfile only needed temporarily
    if (!inputfile) {
      printf("Unable to open input file\n");
      return 0;
    }
   
    // start reading input data using function fscanf here
    fscanf(inputfile, "%d", &N); // read an integer N for number of samples
    fscanf(inputfile, "%d", &T); // read an integer T for number of trials
    fclose(inputfile);
    numruns=T/size; // number of runs per processor
    // set initial seed
    time(&mytime);
    initseed=(long) mytime;
  }
  // broadcast seed
  MPI_Bcast(&initseed,1,MPI_INT,0,MPI_COMM_WORLD);
  // broadcast N
  MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
  //broadcast numruns
  MPI_Bcast(&numruns,1,MPI_INT,0,MPI_COMM_WORLD);

  // do the monte carlo simulations, there are numruns per processor
  long seed=initseed+rank;
  srand48(seed);//set seed
  double mc[numruns]; // local storage for mc results on each processor
  for(int j=0; j<numruns;j++)  
    mc[j] = mcquad(N);//

  // gather all the monte carlo results
  if (rank==0){
    results = (double*)malloc(T*sizeof(double));
  }
  MPI_Gather(&mc,numruns,MPI_DOUBLE,results,numruns,MPI_DOUBLE,0,MPI_COMM_WORLD);

  // write output of trials to file
  if (rank==0){
      FILE *fileid=fopen(argv[2],"w"); // open output file
      fwrite(&T,sizeof(int),1,fileid);
      fwrite(results,sizeof(double),T,fileid); // write output
      free(results);
      fclose(fileid);
    }  

  // print final execution time to terminal screen
  if (rank==0){
    double time_elapsed = MPI_Wtime() - starttime;
    printf("Execution time = %le seconds, with precision %le seconds \n",
	   time_elapsed,precision);
  }

  MPI_Finalize(); //shut down MPI
  return 0;
}
