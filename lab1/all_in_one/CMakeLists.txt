cmake_minimum_required (VERSION 2.8.2)
project (hpc)
add_executable(hpc.omp main.c sweeps/mandelbrot.c sweeps/gameoflife.c sweeps/heat.c vtk.c field.c filling.c simdata.c sweeps/add.c)


if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions("-Wall -std=gnu99")
endif()

find_library(MATH_LIBRARY NAMES m)
target_link_libraries(hpc.omp ${MATH_LIBRARY})

find_package(OpenMP REQUIRED)

if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
