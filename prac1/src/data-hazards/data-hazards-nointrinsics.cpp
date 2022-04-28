#include <cassert>
#include <cstdio>
#include <memory>
#include <omp.h>
#include "util.h"

using namespace std;

struct {
  size_t repeat_times = 3; // total, including warmup
  size_t warmup_times = 1;
  bool print_each_time = false;
  size_t total_iterations = 640000000L;
} options;

template<int num_vars>
[[gnu::noinline]] // Non standard, but supported by GCC, ICC and CLang
void calculate(float& result, size_t total_iterations) {
  float a[num_vars];
  float one = 1.0f;
  for (size_t i = 0; i < num_vars; ++i) {
    a[i] = 0.0f;
  }
  for (size_t j = 0; j < total_iterations; ++j) {
    for (int k = 0; k < num_vars; ++k) {
      a[k] = a[k] + one;
    } 
  }
  for (size_t i = 1; i < num_vars; ++i) {
    a[0] = a[0] + a[i];
  }
  result = a[0];
}

template<int num_vars>
void measure() {
  long n_flop = options.total_iterations * num_vars;
  printf("Measuring time to do %ld floating point operations (%.3f GFLOPs) with %d variables:\n", n_flop, double(n_flop) / 1000000000, num_vars);
  vector<double> times;
  vector<double> flops;

  float first_result = -1;
  for (size_t i = 0; i < options.repeat_times; ++i) {
    float result;
    double elapsed_time = measure_time(calculate<num_vars>, result, options.total_iterations);
    if (i == 0) {
      first_result = result;
    } else {
      assert(result == first_result);
    }
    if (i >= options.warmup_times) {
      times.push_back(elapsed_time);
      flops.push_back(n_flop / elapsed_time);
    }
    if (options.print_each_time) {
      printf("    Run %2ld/%2ld: %7.2fs ⇒ %7.2f GFLOPS  %s  result = %f\n", i + 1, options.repeat_times, elapsed_time, n_flop / elapsed_time / 1000000000, i < options.warmup_times ? "(warmup)" : "        ", result);
    }
  }

  double average_time = vector_average(times);
  double stddev_time = vector_stddev(times);
  double average_flops = vector_average_harmonic(flops);
  double stddev_flops = vector_stddev_harmonic(flops);
  printf("Average time (s): %7.2f±%.2f  GFLOPS:   %7.2f±%.2f    num_vars = %d  result = %f\n", average_time, stddev_time, average_flops / 1000000000, stddev_flops / 1000000000, num_vars, first_result);
}

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (!parse_bool_arg(argv[i], "print-each-time", options.print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", options.repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", options.warmup_times)
        && !parse_size_arg(argv[i], "total-iterations", options.total_iterations)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  measure<1>();
  measure<2>();
  measure<3>();
  measure<4>();
  measure<5>();
  measure<6>();
  measure<7>();
  measure<8>();
  measure<9>();
  measure<10>();
  measure<11>();
  measure<12>();
  measure<13>();
  measure<14>();
  measure<15>();
  measure<16>();
  measure<17>();
  measure<18>();
  measure<19>();
  measure<20>();
  measure<21>();
  measure<22>();
  measure<23>();
  measure<24>();
  measure<25>();
  measure<26>();
  measure<27>();
  measure<28>();
  measure<29>();
  measure<30>();
  measure<31>();
  measure<32>();

  return 0;
}
