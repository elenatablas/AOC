
default: data-hazards-gcc 

all: data-hazards-gcc data-hazards-icc data-hazards-clang \
	data-hazards-nointrinsics-gcc data-hazards-nointrinsics-icc data-hazards-nointrinsics-clang

SOURCES_COMMON_CPP=util.cpp
SOURCES_COMMON_H=util.h
SOURCES_COMMON=$(SOURCES_COMMON_CPP) $(SOURCES_COMMON_H)

%-gcc: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-clang: %.cpp $(SOURCES_COMMON)
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-icc: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

%-debug: %.cpp $(SOURCES_COMMON)
	g++ -std=c++20 -Wall -O0 -g -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

.PHONY: clean
clean:
	rm -f data-hazards-gcc data-hazards-debug data-hazards-icc data-hazards-clang \
		data-hazards-nointrinsics-gcc data-hazards-nointrinsics-icc data-hazards-nointrinsics-clang

