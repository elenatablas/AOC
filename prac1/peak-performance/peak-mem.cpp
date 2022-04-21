#include "util.h"
#include <cstdio>
#include <cstring>
#include <memory>
#include <omp.h>

using namespace std;

struct {
  size_t repeat_times = 7; // total, including warmup
  size_t warmup_times = 2;
  bool print_each_time = true;
  size_t array_size = 128 * 1024 * 1024;
  size_t threads = 1;
} options;

double* a = nullptr;
double* b = nullptr;

void init() {
  a = static_cast<double*>(aligned_alloc(64, options.array_size * sizeof(a[0])));
  b = static_cast<double*>(aligned_alloc(64, options.array_size * sizeof(a[0])));
# pragma omp parallel for
  for (size_t i = 0; i < options.array_size; ++i) {
    a[i] = i;
  }
}

template<bool use_memcpy>
void calculate() {
  double *la = assume_aligned<64>(a);
  double *lb = assume_aligned<64>(b);
  if (use_memcpy) {
    size_t per_thread_elements = options.array_size / options.threads;
#   pragma omp parallel num_threads(options.threads)
    memcpy(lb + per_thread_elements * omp_get_thread_num(),
           la + per_thread_elements * omp_get_thread_num(),
           per_thread_elements * sizeof(a[0]));     // usando std::copy se obtienen los mismos resultados
  } else {
#   pragma omp parallel for num_threads(options.threads) schedule(static)
    for (size_t i = 0; i < options.array_size; ++i) {
      lb[i] = la[i];
    }
  }
}

template<bool use_memcpy>
void measure() {
  long n_bytes = 2 * options.array_size * sizeof(a[0]);
  printf("Measuring time to copy %ld MiB with %zu threads %s:\n", n_bytes / 2 / 1024 / 1024, options.threads, use_memcpy ? "using memcpy" : "using a loop");
  vector<double> times;
  vector<double> bps;

  for (size_t i = 0; i < options.repeat_times; ++i) {
    double elapsed_time = measure_time(calculate<use_memcpy>);
    if (i >= options.warmup_times) {
      times.push_back(elapsed_time);
      bps.push_back(n_bytes / elapsed_time);
    }
    if (options.print_each_time) {
      printf("    Run %2ld/%2ld: %7.2fs ⇒ %7.2f GiB/s %7.2f GB/s  %s\n", i + 1, options.repeat_times, elapsed_time, n_bytes / elapsed_time / (1024*1024*1024), n_bytes / elapsed_time / (1000*1000*1000), i < options.warmup_times ? "(warmup)" : "");
    }
  }
  
  double average_time = vector_average(times);
  double stddev_time = vector_stddev(times);
  printf("Average time (s): %7.2f±%.2f      threads = %zu\n", average_time, stddev_time, options.threads);
  double average_bps = vector_average_harmonic(bps);
  double stddev_bps = vector_stddev_harmonic(bps);
  printf("Average GiB/s:    %7.2f±%.2f      threads = %zu\n", average_bps / (1024*1024*1024), stddev_bps / (1024*1024*1024), options.threads);
  printf("Average GB/s:     %7.2f±%.2f      threads = %zu\n\n", average_bps / (1000*1000*1000), stddev_bps / (1000*1000*1000), options.threads);
}

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (!parse_bool_arg(argv[i], "print-each-time", options.print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", options.repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", options.warmup_times)
        && !parse_size_arg(argv[i], "array-size", options.array_size)
        && !parse_size_arg(argv[i], "threads", options.threads)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  init();

  measure<false>();  
  measure<true>();  

  free(a);
  free(b);
  
  return 0;
}
