
default: matrix-gcc

all: matrix-gcc matrix-icc matrix-clang matrix-iccmkl

BLAS_INCLUDES=-I/usr/include/openblas/
BLAS_LIBS=-l openblaso

SOURCES_COMMON_CPP=util.cpp multiply_matrix.cpp
SOURCES_COMMON_H=util.h matrix.h
SOURCES_COMMON=$(SOURCES_COMMON_CPP) $(SOURCES_COMMON_H)

%-gcc: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(BLAS_INCLUDES) $(BLAS_LIBS) $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-clang: %.cpp $(SOURCES_COMMON)
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(BLAS_INCLUDES) $(BLAS_LIBS) $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-icc: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp $(BLAS_INCLUDES) $(BLAS_LIBS) $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-iccmkl: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp -mkl -DUSE_MKL $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-debug: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -O0 -fopenmp  $(BLAS_INCLUDES) $(BLAS_LIBS) $(SOURCES_COMMON_CPP) $*.cpp -o $@

.PHONY: clean
clean:
	rm -f matrix-gcc matrix-icc matrix-clang matrix-iccmkl

