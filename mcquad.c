/*
  
  Program written by Nathan Sanford
  
  Performs Monte Carlo Integration using recurrence relations for the mean
  and variance in order to minimize storage space.
      mean[n]=((n-1)/n)*mean[n-1] + (1/n)*g(x_n)
      variance[n]=((n-2)/(n-1))*variance[n-1] + (1/n)*(g(x_n)-mean[n-1])^2

  Inputs: text file with number of samples to be used
  Outputs: prints mean and variance of MC simulation to screen

  Editing History:
  4/5/2016 Initial Draft
  4/6/2016 Added comments
  4/11/2016 Changed it so that seed is printed to terminal
  4/19/2016 Final Draft 
  10/30/19 Edited f and g functions to match notes
  11/6/19 Edited explanatory comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
  int main( int argc , char* argv [])

  The main program performs the monte carlo integration with N samples.
  Uses recurrence relations for mean and variance
      mean[n]=((n-1)/n)*mean[n-1] + (1/n)*g(x_n)
      variance[n]=((n-2)/(n-1))*variance[n-1] + (1/n)*(g(x_n)-mean[n-1])^2
  so then it outputs mean[N] and variance[N] by this notation. Calls drand48
  to get initial random number. drand48 is initialized using srand48 and the
  computer time so each run is different.

  Inputs:
  int argc: should be 2, just function name and text file name

  char* argv[]: argv[1] is the name of the of the text file simply containing N
                the number of samples.

  Outputs: Prints the number of samples used, as well as the seed for the 
           random number generator to the terminal. Then prints the mean 
	   and variance of the MC simulations, as well as the 
           time elapsed. 
*/

int main(int argc, char* argv[])
{  
  if (argc != 2) {
    printf("Incorrect usage: only enter the input data file name\n");
    return 0;
  }
  FILE* inputfile = fopen(argv[1], "r");
  if (!inputfile) {
    printf("Unable to open input file\n");
    return 0;
  }
   
  // start reading input data using function fscanf here
  int N;
  fscanf(inputfile, "%d", &N); // read an integer N for number of samples
  fclose(inputfile);
  
  clock_t start = clock();

  // ------------------variable declarations-------------------
  double z, y, x; // temporary variables used in drawing samples
  double  mean=0, variance=0; // for storing mean and variance 
  time_t seed; // for storing seed for rng from computer time
  // ----------------------------------------------------------
  printf("Using %d samples \n",N);
  time(&seed); // set seed 
  printf("Seed from clock time is %ld \n",(long) seed); // print seed
  srand48((long) seed); // set seed based on clock so each run is different
  // loop through samples
  for (int n=1; n<=N; n++){
    z=drand48(); //uniform RN
    y=p(z); //exponential RN
    x=f(y); //sample   
    // update variance
    if (n>1) // can't compute variance until there are at least 2 samples
      variance=((n-2.0)/(n-1.0))*variance + (1.0/n)*pow(x-mean,2);
    // update mean
    mean=((n-1.0)/(n))*mean + x/n;
  }
  // print output
  printf("Estimate for integral is %lf \n",mean);
  printf("Estimate for variance is %lf \n",variance);
  // print time elapsed 
  printf("Time elapsed: %g seconds\n", (float)(clock()-start)/CLOCKS_PER_SEC);
  return 0;
}
