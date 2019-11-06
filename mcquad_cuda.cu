/*
  Problem 1 for Project 3
 
  Program written by Nathan Sanford
  
  Performs Monte Carlo Integration trials in a parallel cuda environment. 
  The integral being approximated is 
                           -inf
			  /
			 |      exp(-x)g(x) dx
			/
                       -0
  where here g(x)=cos(x) but this can be changed in the kernel function mcquad.
  Performs T trials of N samples per trial by throwing each trial onto a 
  processor that draws all the samples for that trial.

  Inputs: Names of 2 text files. The first is an input file which consists 
          of the number of samples per trial and the number of samples. The 
	  second is for an output file to which the result of each trial is
	  written.
  Outputs: Writes the result of T trials to the 
           output file. Additionally, writes the total simulation time to 
	   the terminal at the end of simulation.

  Edit history:
  5/15/2016 Initial draft
  6/3/2016 Added comments
  6/7/2016 Number of trials no longer written to output file
  11/6/2019 Edited filename to differentiate
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <curand.h>
#include <curand_kernel.h>

/*
  void init(int T, unsigned int seed, curandState_t* states)

  Initializes the vector of states where each state represents a trial. Uses
  the curand built-in curand_init.

  Inputs:
  T is the number of trials, used as a guard so that only T things are 
  initialized and seed is an integer which represents a seed 
  as in many RNGs.

  Outputs:
  The vector states which is T elements long, is initialized using the built-in
  curand capability curand_init.
*/


__global__ void init(int T, unsigned int seed, curandState_t* states) {
  int id=threadIdx.x+blockIdx.x*blockDim.x;
  // we have to initialize the state on each gpu
  if (id<T)
    curand_init(seed,id,0,&states[id]);
}

/*
  void mcquad(int T, int N, curandState_t* states, double* results)

  Performs the mcquad trials. Performs T trials by drawing N samples in 
  each trial and calculating the mean. Uses the curand built-in 
  curand_uniform to draw a RN between 0 and 1 and then transform into the 
  exponential distribution. Makes the states local to each processor to 
  speed up the process as the states are updated with the drawing of each sample.
  The function that we're averaging is cos(x) and it is specified within this 
  kernel.

  Inputs:
  T is the number of trials, used as a guard so that only T things are
  initialized, N is the number of samples per trial, and states is the state 
  of the RNG for each trial, where the states are assumed to have been 
  initialized previously.

  Outputs:
  The vector results which is T elements long, where each element is the result
  of a trial.
*/

__global__ void mcquad(int T, int N, curandState_t* states, double* results){
  int id=threadIdx.x+blockIdx.x*blockDim.x;
  // ------------------variable declarations-------------------
  double z, y, x; // temporary variables used in drawing samples
  double  mean=0; // mean
  // ----------------------------------------------------------
  // loop through samples
  if (id<T){ // guard so that we only perform T trials
    curandState_t localState = states[id];
    for (int n=1; n<=N; n++){
      z=curand_uniform(&localState); //uniform RN between 0 and 1
      y=-log(z); //exponential RN
      x=cos(y); //sample
      mean+=x;
    }
    mean*=(1.0/N);
    results[id]=mean;
  }
}

/*
  int main( int argc , char* argv [])

  The main program gets the number of trials and the number of samples and then 
  throws the trials onto the gpu where each processor performs one trial. It 
  chooses the K20c card and then uses the minimal number of blocks to get to 
  T trials by having each thread on each block perform a trial.

  Inputs: should be 3, just function name and input and output text file names

  Outputs: Prints the results of each trial to the
           output file. Additionally prints to the terminal screen the
           time elapsed using the cuda timing stuff. 
*/

int main(int argc, char* argv[])
{  
  // timing the CUDA way  
  cudaEvent_t start , stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start,0);

  int N,T;
  // read parameters for simulation from input file
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

  // Choose the GPU card
  cudaDeviceProp prop;
  int dev;
  memset(&prop, 0, sizeof(cudaDeviceProp));
  prop.multiProcessorCount = 13;
  cudaChooseDevice(&dev,&prop);
  cudaSetDevice(dev);

  // Get maximum thread count for the device
  cudaGetDeviceProperties(&prop,dev);
  int num_threads = prop.maxThreadsPerBlock;
  int num_blocks = T/num_threads + (T%num_threads ? 1:0);

  // ---------------------------Variable Declarations---------------------
  // for storing results of all the trials
  double* results=(double*)malloc(T*sizeof(double));
  double* dev_results; // results of all trials on device
  curandState_t* states;
  // ---------------------------------------------------------------------
  // allocate memory on device
  cudaMalloc((void**)&states,T*sizeof(curandState_t));
  cudaMalloc((void**)&dev_results, T*sizeof(double));

  // set initial seeds
  init<<<num_blocks,num_threads>>>(T,time(NULL),states);

  // do the monte carlo simulations on device
  mcquad<<<num_blocks,num_threads>>>(T,N,states,dev_results);

  // copy output data from device
  cudaMemcpy(results,dev_results,T*sizeof(double),cudaMemcpyDeviceToHost);

  // write output of trials to file
  FILE *fileid=fopen(argv[2],"w"); // open output file
  fwrite(results,sizeof(double),T,fileid); // write output
  fclose(fileid);    

  // free memory
  free(results);
  cudaFree(states);
  cudaFree(dev_results);

  // timing the cuda way
  cudaEventRecord(stop,0); 
  cudaEventSynchronize(stop);
  float elapsedTime;
  cudaEventElapsedTime(&elapsedTime, start, stop);
  // print final execution time to terminal screen
  printf("Cuda time elapsed: %g seconds \n",elapsedTime/1000.0);
  return 0;
}
