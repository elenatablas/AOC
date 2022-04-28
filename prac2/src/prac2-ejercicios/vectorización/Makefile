
default: loops-icc

all: loops-icc loops-gcc loops-clang

loops-icc: loops.cpp
	icpc -std=c++20 -Wall -O2 -xCORE-AVX2 -qopt-report=5 -qopt-report-file=$@.optrpt $^ -o$@

loops-gcc: loops.cpp
	g++ -g -std=c++20 -Wall -march=skylake -mtune=skylake -O3 -ffast-math -fopt-info-all=$@.optrpt $^ -o$@

loops-clang: loops.cpp
	clang++ -g -std=c++20 -Wall -march=skylake -mtune=skylake -O3 -ffast-math -foptimization-record-file=$@.optrpt $^ -o$@

.PHONY: clean
clean:
	rm -f loops-icc loops-icc.optrpt loops-gcc loops-gcc.optrpt loops-clang loops-clang.optrpt

