#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <omp.h>
#include "util.h"

#include "heat.h"

using namespace std;

void init_problem(Matrix<double>& p, double temp_top, double temp_bottom, double temp_left, double temp_right, double temp_center) {
  for (size_t i = 0; i < p.height; ++i) {
    for (size_t j = 0; j < p.width; ++j) {
      p[i][j] = temp_center;
    }
  }  
  for (size_t i = 0; i < p.height; ++i) {
    p[i][0] = temp_left;
    p[i][p.width - 1] = temp_right;
  }
  for (size_t i = 0; i < p.width; ++i) {
    p[0][i] = temp_top;
    p[p.height - 1][i] = temp_bottom;
  }
}

bool verbose = false;

int main(int argc, char** argv) {
  size_t rows = 10;
  size_t cols = 10;
  double tolerance = 0.0005;
  bool print_result = true;
  bool print_iterations = true;
  bool print_difference = true;
  bool print_each_time = true;
  size_t repeat_times = 1;
  size_t warmup_times = 0;
  bool print_average_time = true;
  double temp_top = 70;
  double temp_bottom = 10;
  double temp_left = 90;
  double temp_right = 20;
  double temp_center = 0;
  
  for (int i = 1; i < argc; ++i) {
    if (!parse_size_arg(argv[i], "rows", rows)
        && !parse_size_arg(argv[i], "cols", cols)
        && !parse_bool_arg(argv[i], "verbose", verbose)
        && !parse_bool_arg(argv[i], "print-result", print_result)
        && !parse_bool_arg(argv[i], "print-iterations", print_iterations)
        && !parse_bool_arg(argv[i], "print-difference", print_difference)
        && !parse_double_arg(argv[i], "tolerance", tolerance)
        && !parse_bool_arg(argv[i], "print-each-time", print_each_time)
        && !parse_size_arg(argv[i], "repeat-times", repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", warmup_times)
        && !parse_bool_arg(argv[i], "print-average-time", print_average_time)
        && !parse_double_arg(argv[i], "temp-top", temp_top)
        && !parse_double_arg(argv[i], "temp-bottom", temp_bottom)
        && !parse_double_arg(argv[i], "temp-left", temp_left)
        && !parse_double_arg(argv[i], "temp-right", temp_right)
        && !parse_double_arg(argv[i], "temp-center", temp_center)) {
      cerr << "Argumento incorrecto: " << argv[i] << endl;
      return 1;
    }
  }
    
  vector<double> times;
  for (size_t i = 0; i < repeat_times; ++i) {
    Matrix<double> state(rows, cols);
    init_problem(state, temp_top, temp_bottom, temp_left, temp_right, temp_center);
    if (i == 0 && verbose) {
      cout << "Initial state:" << endl;
      printf_matrix("%7.3f", state);
    }
    int iterations;
    double difference;
    double start_time = omp_get_wtime();
    solve(state, tolerance, iterations, difference);
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    if (i < warmup_times) {
      times.push_back(elapsed_time);
    }
    if (i == 0) {
      if (print_result) {
        cout << "Result:" << endl;
        printf_matrix("%7.3f", state);
        cout << endl;
      }
      if (print_difference) {
        cout << "Difference: " << setprecision(5) << difference << endl;
      }
      if (print_iterations) {
        cout << "Iterations: " << iterations << endl;
      }
    }
    if (print_each_time) {
      cout << "Time (s) (run " << i + 1 << "/" << repeat_times << "): " << fixed << setw(7) << setprecision(2) << elapsed_time << (i < warmup_times ? "  (warmup)" : "") << endl;
    }
  }
  if (print_average_time) {
    double average_time = vector_average(times);
    double stddev_time = vector_stddev(times);
    cout << "Average time (s): " << fixed << setw(7) << setprecision(2) << average_time << "±" << stddev_time << endl;
  }
  
  return 0;
}
