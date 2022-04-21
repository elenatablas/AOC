
default: vectorization-icc


all: vectorization-icc vectorization-gcc vectorization-clang

REPORT_FLAGS_ICC=-qopt-report=5 -qopt-report-file=$@.optrpt
REPORT_FLAGS_GCC=-fopt-info-all=$@.optrpt
REPORT_FLAGS_CLANG= -foptimization-record-file=$@.optrpt


%-gcc.o: %.cpp
	g++ -c -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math $< -o $@ $(REPORT_FLAGS_GCC) 

vectorization-gcc: vectorization-gcc.o worker-gcc.o
	g++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math $^ -o $@


%-clang.o: %.cpp
	clang++ -c -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math $< -o $@ $(REPORT_FLAGS_CLANG) 

vectorization-clang: vectorization-clang.o worker-clang.o
	clang++ -g -std=c++20 -Wall -march=native -mtune=native -O3 -ffast-math $^ -o $@


%-icc.o: %.cpp
	icpc -c -g -std=c++20 -Wall -xHost -O2 -restrict $< -o $@ $(REPORT_FLAGS_ICC) 

vectorization-icc: vectorization-icc.o worker-icc.o
	icpc -g -std=c++20 -Wall -xHost -O2 -restrict $^ -o $@


.PHONY: clean
clean:
	rm -f \
		vectorization-gcc  vectorization-gcc.o  vectorization-gcc.o.optrpt  worker-gcc.o  worker-gcc.o.optrpt \
		vectorization-clang  vectorization-clang.o  vectorization-clang.o.optrpt  worker-clang.o  worker-clang.o.optrpt \
		vectorization-icc  vectorization-icc.o  vectorization-icc.o.optrpt  worker-icc.o  worker-icc.o.optrpt


