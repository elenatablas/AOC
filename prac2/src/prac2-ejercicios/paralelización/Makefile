
default: heat-gcc

all: heat-gcc heat-icc heat-clang

SOURCES_COMMON_CPP=main.cpp util.cpp
SOURCES_COMMON_H=matrix.h heat.h util.h
SOURCES_COMMON=$(SOURCES_COMMON_CPP) $(SOURCES_COMMON_H)

%-gcc: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-clang: %.cpp $(SOURCES_COMMON)
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-icc: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-debug: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -O0 -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

.PHONY: tests-gcc tests-clang tests-icc tests-all
tests-gcc: heat-gcc
	./run-tests ./heat-gcc

tests-clang: heat-clang
	./run-tests ./heat-clang

tests-icc: heat-icc
	./run-tests ./heat-icc

tests-all: tests-gcc tests-clang tests-icc

.PHONY: times-gcc times-clang times-icc times-all
times-gcc: heat-gcc
	VERSIONS_TESTS="gcc" ./benchmark-heat

times-clang: heat-clang
	VERSIONS_TESTS="clang" ./benchmark-heat

times-icc: heat-icc
	VERSIONS_TESTS="icc" ./benchmark-heat

times-all: 
	./benchmark-heat


.PHONY: clean
clean:
	rm -f heat-gcc heat-clang heat-icc heat-debug
