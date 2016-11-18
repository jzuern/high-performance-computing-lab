SHELL = /bin/bash
BUILD_TYPE=release # may be set to 'debug' as needed

all:
	@if [ ! -d build ]; then \
	  mkdir build; \
	fi
	@if [ ! -f build/Makefile ]; then \
	  cd build; \
	  cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..; \
	fi
	@make -s -C build all;
	@if [ ! -f hpc.omp ]; then \
	  ln -s build/bin/hpc.omp hpc.omp; \
	fi
	@if [ ! -f hpc.mpi ]; then \
	  ln -s build/bin/hpc.mpi hpc.mpi; \
	fi
	@if [ ! -f vtk2png ]; then \
	  ln -s build/bin/vtk2png vtk2png; \
	fi

clean:
	@rm -f hpc.omp
	@rm -f hpc.mpi
	@rm -f vtk2png
	@rm -rf build
