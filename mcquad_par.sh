#!/bin/bash
# Assuming mpi is active and loaded, the following command runs mcquad_par on 8 
# available cores.
mpirun -np 8 mcquad_par mcquad_par.in mcquad_par.out

