#!/bin/bash
rm *.vtk
make clean
make
mpirun -np 5 ./hpc.mpi 1 2 2 3 3 1 out
