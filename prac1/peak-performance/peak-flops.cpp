#include <cassert>
#include <cstdio>
#include <memory>
#include <omp.h>
#include "util.h"

using namespace std;

const int array_size = 4096;
const int max_threads = 1024;

struct {
  size_t repeat_times = 7; // total, including warmup
  size_t warmup_times = 2;
  bool print_each_time = true;
  size_t total_iterations = 64000000000L; // to adjust total running time
  size_t threads = 1;
  bool more_tests = false; // tests more inner_iterations values
} options;

alignas(64) float a[max_threads][array_size];
alignas(64) float b[max_threads][array_size];

void init() {
# pragma omp parallel for
  for (size_t j = 0; j < max_threads; ++j) {
    for (size_t i = 0; i < array_size; ++i) {
      a[j][i] = 1.0f + i;
      b[j][i] = i - 1.0f;
    }
  }
}

template<int inner_iterations>
[[gnu::noinline]] // Non standard, but supported by GCC, ICC and CLang
void calculate(size_t total_iterations) {
  static_assert(inner_iterations <= array_size);
  assert(total_iterations % inner_iterations == 0);
  const size_t outer_iterations = total_iterations / inner_iterations;
# pragma omp parallel for num_threads(options.threads) schedule(static)
  for (size_t j = 0; j < outer_iterations; ++j) {
    int tid = omp_get_thread_num();
    float *la = assume_aligned<64>(a[tid]);
    float *lb = assume_aligned<64>(b[tid]);
    for (int i = 0; i < inner_iterations; ++i) {
      la[i] = 2.1f * lb[i] + la[i];
    }    
  }
}

template<int inner_iterations>
void measure() {
  long n_flop = options.total_iterations * 2;
  printf("Measuring time to do %ld floating point operations (%ld GFLOPs) with %zu threads and %d inner_iterations:\n", n_flop, n_flop / 1000000000, options.threads, inner_iterations);
  vector<double> times;
  vector<double> flops;

  for (size_t i = 0; i < options.repeat_times; ++i) {
    init();
    double elapsed_time = measure_time(calculate<inner_iterations>, options.total_iterations);
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
  printf("Average time (s): %7.2f±%.2f  GFLOPS: %7.2f±%.2f    threads = %zu inner_iterations = %d\n", average_time, stddev_time, average_flops / 1000000000, stddev_flops / 1000000000, options.threads, inner_iterations);
}

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (!parse_bool_arg(argv[i], "print-each-time", options.print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", options.repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", options.warmup_times)
        && !parse_size_arg(argv[i], "total-iterations", options.total_iterations)
        && !parse_bool_arg(argv[i], "more-tests", options.more_tests)
        && !parse_size_arg(argv[i], "threads", options.threads)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  if (options.threads > max_threads) {
    fprintf(stderr, "--threads must be less or equal than %d\n", max_threads);
    return 1;
  }

  if (options.total_iterations % array_size != 0) {
    fprintf(stderr, "--total_iterations must be multiple of %d\n", array_size);
    return 1;
  }

  if (options.more_tests) {
    measure<4>();  
    measure<8>();  
  }
  measure<16>();  
  measure<32>();  
  measure<64>();  
  measure<128>();  
  measure<256>();  
  if (options.more_tests) {
    measure<512>();  
    measure<1024>();  
  }
  measure<2048>();  
  if (options.more_tests) {
    measure<4096>();
  }
  
  return 0;
}
