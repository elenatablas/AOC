#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <random>
#include <omp.h>
#include "util.h"

using namespace std;

struct {
  size_t array_size = 512;
  size_t outer_iterations = 1000000;
  string implementation_name = "no-vectorized";
  size_t repeat_times = 7; // total, including warmup
  size_t warmup_times = 2;
  bool print_each_time = true;
} options;
  
typedef void sum_vectors_func(float* dest, const float* srcA, const float* srcB, size_t array_size);
// Implementations in test_vectorization.cpp
sum_vectors_func sum_vectors_no_vectorized;
sum_vectors_func sum_vectors_vectorized;
sum_vectors_func sum_vectors_vectorized_restrict;
sum_vectors_func sum_vectors_vectorized_aligned;

void init_array(float* a, size_t size) {
# pragma omp parallel for
  for (size_t i = 0; i < size; ++i) {
    a[i] = i;
  }
}

void repeat_sum_vectors(sum_vectors_func selected_func, float* dest, const float* srcA, const float* srcB, size_t array_size, size_t outer_iterations) {
  // Repeat the benchmark many times to be able to measure its execution time more accurately
  for (size_t j = 0; j < outer_iterations; ++j) {
    selected_func(dest, srcA, srcB, array_size);
  }
}

void measure_vector_sum(const string& implementation_name) {
  long n_flop = options.array_size * options.outer_iterations;
  printf("Measuring time to sum vectors of %ld elements %ld times (%.3f GFLOPs) with implementation “%s”:\n", options.array_size, options.outer_iterations, double(n_flop) / 1000000000, implementation_name.c_str());

  sum_vectors_func* sum_vectors_selected;
  if (options.implementation_name == "no-vectorized") {
    sum_vectors_selected = sum_vectors_no_vectorized;
  } else if (options.implementation_name == "vectorized") {
    sum_vectors_selected = sum_vectors_vectorized;
  } else if (options.implementation_name == "vectorized-restrict") {
    sum_vectors_selected = sum_vectors_vectorized_restrict;
  } else if (options.implementation_name == "vectorized-aligned") {
    sum_vectors_selected = sum_vectors_vectorized_aligned;
  } else {
    fprintf(stderr, "Unknown implementation: %s\n", implementation_name.c_str());
    abort();
  }
  
  vector<double> times;
  vector<double> flops;
  
  float *vA = (float*) aligned_alloc(64, sizeof(float) * options.array_size);
  float *vB = (float*) aligned_alloc(64, sizeof(float) * options.array_size);
  float *vC = (float*) aligned_alloc(64, sizeof(float) * options.array_size);
  init_array(vA, options.array_size);
  init_array(vB, options.array_size);
  
  for (size_t i = 0; i < options.repeat_times; ++i) {
    double elapsed_time = measure_time(repeat_sum_vectors, sum_vectors_selected, vC, vA, vB, options.array_size, options.outer_iterations);
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
  printf("Average time (s): %7.2f±%.2f  GFLOPS: %7.2f±%.2f    array_size = %ld outer_iterations = %ld implementation = %s\n", average_time, stddev_time, average_flops / 1000000000, stddev_flops / 1000000000, options.array_size, options.outer_iterations, implementation_name.c_str());

  free(vA);
  free(vB);
  free(vC);
}

int main(int argc, char** argv) { 
  for (int i = 1; i < argc; ++i) {
    if (!parse_size_arg(argv[i], "vector-size", options.array_size)
        && !parse_size_arg(argv[i], "outer-iterations", options.outer_iterations)
        && !parse_string_arg(argv[i], "implementation", options.implementation_name)
        && !parse_bool_arg(argv[i], "print-each-time", options.print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", options.repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", options.warmup_times)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }
 
  measure_vector_sum(options.implementation_name);  
  
  return 0;
}
