#!/bin/bash
rm *.vtk
make clean
make
time mpirun --mca orte_base_help_aggregate 0 -np 5 ./hpc.mpi 1 2 2 10 10 100 out
