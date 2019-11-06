# Monte Carlo in Parallel

This repository provides code for numerically computing an 
[infinite integral](https://github.com/natsan91/Monte_Carlo_in_Parallel/blob/master/Infinite_Integral.pdf "pdf showing integral")
using Monte Carlo sampling. The integral is relatively simple to compute by hand, but Monte Carlo is a convenient way to do the 
problem numerically even though the domain of integration is infinite. 

The repository contains three different programs to do this: one in C, one using [Open_MPI](https://www.open-mpi.org/), and one that uses
[CUDA](https://developer.nvidia.com/cuda-zone) which can be run on NVIDIA GPUs. These three methods are worth comparing as Monte Carlo
integration is trivially parallelizable (or can be scaled into a parallel computing environment with little to no algorithmic adjustments).
Note that some other types of Monte Carlo methods, such as Markov Chain Monte Carlo (or MCMC) are not so easily parallelized.
The C program does the integration (by drawing samples) in serial, the MPI program draws samples on multiple CPUs, and the CUDA program 
draws samples in parallel on a GPU. By playing with these three programs gives a sense of how parallelization can improve efficiency in computing.