#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <random>
#include <omp.h>
#ifndef USE_MKL
#include <cblas.h>
#endif
#include "util.h"
#include "matrix.h"

using namespace std;

struct {
  size_t size_M = 500;
  size_t size_N = 500;
  size_t size_K = 500;
  string implementation_name = "basic";
  size_t repeat_times = 7; // total, including warmup
  size_t warmup_times = 2;
  bool print_each_time = true;
  bool print_operands = false;
  bool print_result = false;
} options;
  
typedef void multiply_matrix_func(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB);

// Implementations in multiply_matrix.cpp
multiply_matrix_func multiply_matrix_basic;
multiply_matrix_func multiply_matrix_ikj;
multiply_matrix_func multiply_matrix_basic_omp;
multiply_matrix_func multiply_matrix_ikj_omp;
multiply_matrix_func multiply_matrix_blas;

template<typename T>
void init_random(Matrix<T>& m) {
# pragma omp parallel for
  for (size_t i = 0; i < m.height; ++i) {
    mt19937 generator(i); // 32 bit Mersenne Twister pseudo-random generator
    uniform_real_distribution<T> distribution(-5,5);
    for (size_t j = 0; j < m.width; ++j) {
      m[i][j] = distribution(generator);
    }
  }  
}

void display_omp_info() {
  printf("omp_get_num_procs: %d\n", omp_get_num_procs());
  printf("omp_get_max_threads: %d\n", omp_get_max_threads());
}

void display_blas_info() {
#ifndef USE_MKL
  printf("openblas_get_config: %s\n", openblas_get_config());
  printf("openblas_get_parallel: %d\n", openblas_get_parallel());
  printf("openblas_get_num_procs: %d\n", openblas_get_num_procs());
  printf("openblas_get_num_threads: %d\n", openblas_get_num_threads());
#endif
}

void measure_matrix_multiplication(const string& implementation_name) {
  long n_flop = options.size_M * options.size_N * options.size_K * 2;
  printf("Measuring time to multiply matrices A (%ld×%ld) and B (%ld×%ld) (%.3f GFLOPs) with implementation “%s”:\n", options.size_M, options.size_K, options.size_K, options.size_N, double(n_flop) / 1000000000, implementation_name.c_str());

  multiply_matrix_func* multiply_matrix_selected;
  if (options.implementation_name == "basic") {
    multiply_matrix_selected = multiply_matrix_basic;
  } else if (options.implementation_name == "ikj") {
    multiply_matrix_selected = multiply_matrix_ikj;
  } else if (options.implementation_name == "basic_omp") {
    multiply_matrix_selected = multiply_matrix_basic_omp;
    display_omp_info();
  } else if (options.implementation_name == "ikj_omp") {
    multiply_matrix_selected = multiply_matrix_ikj_omp;
    display_omp_info();
  } else if (options.implementation_name == "blas") {
    multiply_matrix_selected = multiply_matrix_blas;
    display_blas_info();
  } else {
    fprintf(stderr, "Unknown implementation: %s\n", implementation_name.c_str());
    abort();
  }
  
  vector<double> times;
  vector<double> flops;
  
  Matrix<float> mA(options.size_M, options.size_K);
  Matrix<float> mB(options.size_K, options.size_N);
  Matrix<float> mC(options.size_M, options.size_N);
  init_random(mA);
  init_random(mB);
  if (options.print_operands) {
    printf("Matrix A:\n");
    printf_matrix("%7.3f", mA);
    printf("Matrix B:\n");
    printf_matrix("%7.3f", mB);
  }
  for (size_t i = 0; i < options.repeat_times; ++i) {
    double elapsed_time = measure_time(multiply_matrix_selected, mC, mA, mB);
    if (i == 0 && options.print_result) {
      printf("Result:\n");
      printf_matrix("%7.3f", mC);
    }
    if (i >= options.warmup_times) {
      times.push_back(elapsed_time);
      flops.push_back(n_flop / elapsed_time);
    }
    if (options.print_each_time) {
      printf("    Run %2ld/%2ld: %7.2fs ⇒ %7.2f GFLOPS  %s\n", i + 1, options.repeat_times, elapsed_time, n_flop / elapsed_time / 1000000000, i < options.warmup_times ? "(warmup)" : "");
    }
  }
  
  double average_time = vector_average(times);
  double stddev_time = vector_stddev(times);
  double average_flops = vector_average_harmonic(flops);
  double stddev_flops = vector_stddev_harmonic(flops);
  printf("Average time (s): %7.2f±%.2f  GFLOPS: %7.2f±%.2f    size_M = %ld size_N = %ld size_K = %ld implementation = %s\n", average_time, stddev_time, average_flops / 1000000000, stddev_flops / 1000000000, options.size_N, options.size_N, options.size_K, implementation_name.c_str());
}

int main(int argc, char** argv) {
  size_t square_size = 0;
  
  for (int i = 1; i < argc; ++i) {
    if (!parse_size_arg(argv[i], "size_m", options.size_M)
        && !parse_size_arg(argv[i], "size_n", options.size_N)
        && !parse_size_arg(argv[i], "size_k", options.size_K)
        && !parse_size_arg(argv[i], "square-size", square_size)
        && !parse_string_arg(argv[i], "implementation", options.implementation_name)
        && !parse_bool_arg(argv[i], "print-each-time", options.print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", options.repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", options.warmup_times)
        && !parse_bool_arg(argv[i], "print-operands", options.print_operands)
        && !parse_bool_arg(argv[i], "print-result", options.print_result)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  if (square_size != 0) {
    options.size_M = square_size;
    options.size_N = square_size;
    options.size_K = square_size;
  }
  
  measure_matrix_multiplication(options.implementation_name);  
  
  return 0;
}
