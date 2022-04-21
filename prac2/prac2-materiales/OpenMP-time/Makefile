default: integral-reduc-gcc integral-tasks-gcc

all: integral-reduc-gcc integral-reduc-icc integral-reduc-clang integral-tasks-gcc integral-tasks-icc integral-tasks-clang 

SOURCES_COMMON_CPP= #util.cpp
SOURCES_COMMON_H= #util.h
SOURCES_COMMON=$(SOURCES_COMMON_CPP) $(SOURCES_COMMON_H)

%-gcc: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@ 

%-clang: %.cpp $(SOURCES_COMMON)
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@ 

%-icc: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@ 

%-debug: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -O0 -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

.PHONY: clean
clean:
	rm -f -- integral-reduc-gcc integral-reduc-icc integral-reduc-clang integral-reduc-debug integral-tasks-gcc integral-tasks-icc integral-tasks-clang integral-tasks-debug
