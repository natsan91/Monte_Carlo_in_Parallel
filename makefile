all: mcquad mcquad_par mcquad_cuda

runmcquad: mcquad
	./mcquad mcquad.in

mcquad: mcquad.o
	gcc -o mcquad mcquad.o -lm

mcquad.o: mcquad.c
	gcc -c mcquad.c -std=gnu99 

mcquad_par: mcquad_par.o
	mpicc -std=gnu99 -g -o mcquad_par mcquad_par.o 

mcquad_par.o: mcquad_par.c
	mpicc -std=gnu99 -g -c mcquad_par.c
	
mcquad_cuda: mcquad_cuda.o
	nvcc -o mcquad_cuda mcquad_cuda.o 

mcquad_cuda.o: mcquad_cuda.cu
	nvcc -c mcquad_cuda.cu

clean:
	-rm mcquad
	-rm mcquad_par
	-rm mcquad_cuda
	-rm *.o

cleanoutput:
	-rm *.o[0-9][0-9]*
	-rm *.e[0-9][0-9]*
	-rm *.out


