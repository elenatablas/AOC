default: binning-icc

all: binning-gcc binning-icc binning-clang 

SOURCES_COMMON_CPP=util.cpp main.cpp
SOURCES_COMMON_H=util.h binning.h
SOURCES_COMMON=$(SOURCES_COMMON_CPP) $(SOURCES_COMMON_H)

REPORT_FLAGS_ICC=-qopt-report=5 -qopt-report-file=$@.optrpt
REPORT_FLAGS_GCC=-fopt-info-all=$@.optrpt
REPORT_FLAGS_CLANG= -foptimization-record-file=$@.optrpt

%-gcc: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@ $(REPORT_FLAGS_GCC)

%-clang: %.cpp $(SOURCES_COMMON)
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@  $(REPORT_FLAGS_CLANG)

%-icc: %.cpp $(SOURCES_COMMON)
	icpc -g -std=c++20 -Wall -xHost -O2 -qopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@  $(REPORT_FLAGS_ICC)

%-debug: %.cpp $(SOURCES_COMMON)
	g++ -g -std=c++20 -Wall -O0 -fopenmp $(SOURCES_COMMON_CPP) $*.cpp -o $@

.PHONY: clean
clean:
	rm -f -- binning-gcc binning-icc binning-clang binning-debug \
              binning-gcc.optrpt binning-icc.optrpt binning-clang.optrpt

