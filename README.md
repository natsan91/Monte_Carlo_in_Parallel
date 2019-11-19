# Monte Carlo in Parallel

<img src="https://github.com/natsan91/Monte_Carlo_in_Parallel/blob/master/Integrand.jpg" alt="Integrand" style="zoom:67%;" />

This repository provides code for numerically computing the integral 

```
![equation]([img]http://www.sciweavers.org/tex2img.php?eq=%5Cint_0%5E%5Cinfty%20e%5E%7B-x%7D%5Ccos%28x%29%5C%2Cdx&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0[/img])
```

$$
\mathcal{I} = \int_0^\infty e^{-x} \cos(x)\,dx
$$


using Monte Carlo sampling. The integral is relatively simple to compute by hand (it equals 1/2), but Monte Carlo is a convenient way to do the  problem numerically even though the domain of integration is infinite. This integral can be computed numerically by considering 


$$
p(x) = e^{-x}
$$


to be a probability distribution (note that it is a valid distribution as it is nonnegative and its
total integral on 0 to 1 is 1), then the above integral is equivalent to the expectation
$$
\mathcal{I} = E[cos(X)] \, \text{with } X \text{ drawn from }p(x).
$$
Therefore, we can compute the value of the integral via Monte Carlo integration by repeatedly drawing samples from the exponential distribution, taking the cosine of each sample, and then taking the mean of all the results. After a sufficiently large amount of samples, we obtain an estimate near the true value of 1/2.

The repository contains three different programs to do this: one in C, one using [Open_MPI](https://www.open-mpi.org/), and one that uses [CUDA](https://developer.nvidia.com/cuda-zone) which can be run on NVIDIA GPUs. These three methods are worth comparing as Monte Carlo integration is trivially parallelizable (or can be scaled into a parallel computing environment with little to no algorithmic adjustments). Note that some other types of Monte Carlo methods, such as Markov Chain Monte Carlo (or MCMC) are not so easily parallelized. The C program does the integration (by drawing samples) in serial, the MPI program draws samples on multiple CPUs, and the CUDA program draws samples in parallel on a GPU. By playing with these three programs gives a sense of how parallelization can improve efficiency in computing.