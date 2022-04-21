#include <cassert>
#include <cstdio>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <fstream>
#include <omp.h>
#include "util.h"

using namespace std;

const int array_size = 4096;
const int max_threads = 1024;

struct {
  double test_duration = 15;
  size_t threads = 1;
} options;

alignas(64) float a[max_threads][array_size];
alignas(64) float b[max_threads][array_size];

atomic<double> average_flops = 0;
atomic<double> stddev_flops = 0;
atomic<bool> finish_calculating = false;

void init() {
# pragma omp parallel for
  for (size_t j = 0; j < max_threads; ++j) {
    for (size_t i = 0; i < array_size; ++i) {
      a[j][i] = 1.0f + i;
      b[j][i] = i - 1.0f;
    }
  }
}

void calculate(size_t total_iterations) {
  const size_t inner_iterations = 64;
  static_assert(inner_iterations <= array_size);
  assert(total_iterations % inner_iterations == 0);
  const size_t outer_iterations = total_iterations / inner_iterations;
# pragma omp parallel for num_threads(options.threads) schedule(static)
  for (size_t j = 0; j < outer_iterations; ++j) {
    int tid = omp_get_thread_num();
    float *la = assume_aligned<64>(a[tid]);
    float *lb = assume_aligned<64>(b[tid]);
    for (size_t i = 0; i < inner_iterations; ++i) {
      la[i] = 2.1f * lb[i] + la[i];
    }    
  }
}

void measure_flops() {
  const size_t total_iterations = 32000000000L;
  const long n_flop = total_iterations * 2;
  vector<double> flops;
  while (!finish_calculating) {
    init();
    double elapsed_time = measure_time(calculate, total_iterations);
    flops.push_back(n_flop / elapsed_time);
    average_flops = vector_average_harmonic(flops);
    stddev_flops = vector_stddev_harmonic(flops);
    // calculate the average GFLOPS only for the last few iterations
    if (flops.size() > 10) {
      flops.erase(flops.begin(), flops.begin() + 5);
    }
  }
}

void measure_freq() {
  vector<double> avg_freqs;
  vector<double> max_freqs;
  const int interval_ms = 100;
  printf("| Avg. GFLOPS\t| Avg. frequency\t| Avg. max. frequency\t|\n");
  double start_time = omp_get_wtime();
  while (omp_get_wtime() - start_time < options.test_duration) {
    ifstream cpuinfo("/proc/cpuinfo");
    assert(cpuinfo.is_open());
    string s;
    vector<double> individual_freqs;    
    while (getline(cpuinfo, s)) {
      if (s.starts_with("cpu MHz")) {
        auto p = s.rfind("\t: ");
        assert(p != string::npos);
        double f = stod(s.substr(p + 3).c_str());
        individual_freqs.push_back(f);
      }
    }
    cpuinfo.close();
    avg_freqs.push_back(vector_average_harmonic(individual_freqs));
    max_freqs.push_back(*max_element(individual_freqs.begin(), individual_freqs.end()));
    double average_avg_freq = vector_average_harmonic(avg_freqs);
    double stddev_avg_freq = vector_stddev_harmonic(avg_freqs);
    double average_max_freq = vector_average_harmonic(max_freqs);
    double stddev_max_freq = vector_stddev_harmonic(max_freqs);
    printf("|               |                       |                       |     \r"
           "| %7.2f±%4.2f\t|   %7.2f±%04.2f\t|     %7.2f±%4.2f\t|      \r", average_flops / 1000000000, stddev_flops / 1000000000, average_avg_freq, stddev_avg_freq, average_max_freq, stddev_max_freq);
    fflush(stdout);
    size_t max_size = 2000 / interval_ms; // keep only last 2 seconds of data
    if (avg_freqs.size() > max_size) {
      assert(avg_freqs.size() == max_freqs.size());
      avg_freqs.erase(avg_freqs.begin(), avg_freqs.begin() + max_size / 2);
      max_freqs.erase(max_freqs.begin(), max_freqs.begin() + max_size / 2);
    }
    this_thread::sleep_for(chrono::milliseconds(interval_ms));
  }
  printf("\n");  
  finish_calculating = true;
}

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (!parse_double_arg(argv[i], "test-duration", options.test_duration)
        && !parse_size_arg(argv[i], "threads", options.threads)) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  if (options.threads > max_threads) {
    fprintf(stderr, "--threads must be less or equal than %d\n", max_threads);
    return 1;
  }

  printf("Measuring frequency while running %ld threads during %gs:\n", options.threads, options.test_duration);
  
  thread t(measure_freq);
  measure_flops();  
  t.join();

  return 0;
}
