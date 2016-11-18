cmake_minimum_required (VERSION 2.8.2)
project (hpc)
add_executable(hpc.mpi main.c sweeps/mandelbrot.c sweeps/gameoflife.c sweeps/heat.c sweeps/add.c vtk.c field.c filling.c simdata.c mpidata.c  )


if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions("-Wall -std=gnu99")
endif()

find_library(MATH_LIBRARY NAMES m)
#target_link_libraries(hpc.omp ${MATH_LIBRARY})
target_link_libraries(hpc.mpi ${MATH_LIBRARY})

find_package(MPI REQUIRED)

if(MPI_FOUND OR MPI_C_FOUND)
  if(CMAKE_PATCH_VERSION GREATER 2)
    set_property(TARGET hpc.mpi APPEND_STRING PROPERTY COMPILE_DEFINITIONS "MPI=1")
    set_property(TARGET hpc.mpi APPEND_STRING PROPERTY INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
    target_link_libraries(hpc.mpi ${MPI_C_LIBRARIES})
  else()
    # Workaround for cmake 2.8.2
    if(MPI_COMPILE_FLAGS)
      set_target_properties(hpc.mpi PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()
    if(MPI_LINK_FLAGS)
      set_target_properties(hpc.mpi PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()
    set_property(TARGET hpc.mpi APPEND PROPERTY COMPILE_DEFINITIONS "MPI=1")
    include_directories(${MPI_INCLUDE_PATH})
    target_link_libraries(hpc.mpi ${MPI_LIBRARIES})
  endif()
endif()

find_package(OpenMP REQUIRED)

if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
