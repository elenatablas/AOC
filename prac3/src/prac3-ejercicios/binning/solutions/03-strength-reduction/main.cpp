#include <cassert>
#include <random>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <memory>
#include <omp.h>

#include "util.h"
#include "binning.h"

using namespace std;

void init_input_data(InputDataType& data, size_t seed, size_t numDataPoints) {
  data.numDataPoints = numDataPoints;
  data.particles = (Particle*) aligned_alloc(64, sizeof(Particle) * numDataPoints);
  mt19937 generator(seed); // 32 bit Mersenne Twister pseudo-random generator
  uniform_real_distribution<FTYPE> distr(0.0, maxMagnitudeR);
  uniform_real_distribution<FTYPE> distphi(0.0, 2.0*M_PI);
  for (size_t i = 0; i < numDataPoints; ++i) {
    data.particles[i].r = distr(generator);
    data.particles[i].phi = distphi(generator);
  }
}

void free_input_data(InputDataType& data) {
  free(data.particles);
  data.particles = nullptr;
  data.numDataPoints = 0;
}

void write_result(const string& out_file, const BinsType& binnedData) {
  ofstream out(out_file);
  if (out.fail()) {
    printf("Problem opening output result file.\n");
    exit(1);
  }
  for (size_t i = 0; i < nBinsX; ++i) {
    for (size_t j = 0; j < nBinsY; ++j) {
      out << binnedData[i][j] << "\t";
    }
    out << endl;
  }
  if (out.fail()) {
    printf("Problem writing result file.\n");
    exit(1);
  }
  out.close();
}

void load_result(const string& check_file, BinsType& binnedData) {
  ifstream in(check_file);
  if (!in.good()) {
    printf("Problem opening check result file.\n");
    exit(1);
  }
  for (size_t i = 0; i < nBinsX; ++i) {
    for (size_t j = 0; j < nBinsY; ++j) {
      in >> binnedData[i][j];
    }
  }
  if (in.fail()) {
    printf("Problem reading result file.\n");
    exit(1);
  }
  in.close();
}

void check_result(const string& check_file, const BinsType& binnedData) {
  BinsType check_data;
  load_result(check_file, check_data);
  int maxDiff = 0;
  int nBinsDiff = 0;
  int nPartsCheck = 0;
  int nParts = 0;
  for (int i = 0; i < nBinsX; i++) {
    for (int j = 0; j < nBinsY; j++) {
      assert(binnedData[i][j] >= 0);
      assert(check_data[i][j] >= 0);
      nParts = nParts + binnedData[i][j];
      nPartsCheck = nPartsCheck + check_data[i][j];
      int diff = abs(binnedData[i][j] - check_data[i][j]);
      if (diff > 0) {
        maxDiff = max(maxDiff, diff);
        ++nBinsDiff;
      }
    }
  }
  if (nBinsDiff > 0) {
    int maxDiffThr = double(nParts) / (nBinsX * nBinsY) / 100000;
    printf("Number of different bins: %d,  maximum bin difference: %d (threshold %d),  particle count: %d,  reference particle count: %d\n", nBinsDiff, maxDiff, maxDiffThr, nParts, nPartsCheck);
    if (nParts != nPartsCheck) {
      printf(ESC_RED "The number of input and output particles does not match with %s." ESC_RESET "\n", check_file.c_str());
    }
    if (maxDiff > maxDiffThr) {
      printf(ESC_RED "The number of missclassified particles in some bins is too high with respect to %s." ESC_RESET "\n", check_file.c_str());
    } else {
      printf(ESC_GREEN "The result is similar enough to %s." ESC_RESET "\n", check_file.c_str());
    }
  } else {
    assert(nPartsCheck == nParts && maxDiff == 0);
    printf(ESC_GREEN "The result is exactly as in %s." ESC_RESET "\n", check_file.c_str());
  }
}

void reset_bins(BinsType& bins) {
  for (int i = 0; i < nBinsX; i++) {
    for (int j = 0; j < nBinsY; j++) {
      bins[i][j] = 0;
    }
  }
}

int main(const int argc, const char** argv) {
  string result_output_file = "";
  string result_check_file = "";
  size_t numDataPoints = 134217728; // 1<<27; // Problem size of the data sample
  size_t seed = 1;
  size_t repeat_times = 7; // total, including warmup
  size_t warmup_times = 2;

  for (int i = 1; i < argc; ++i) {
    if (!parse_size_arg(argv[i], "num-data-points", numDataPoints)
        && !parse_string_arg(argv[i], "write-result-file", result_output_file)
        && !parse_string_arg(argv[i], "check-result-file", result_check_file)
        && !parse_size_arg(argv[i], "seed", seed)
        && !parse_size_arg(argv[i], "repeat-times", repeat_times)
        && !parse_size_arg(argv[i], "warmup-times", warmup_times)
      ) {
      fprintf(stderr, "Incorrect argument: %s\n", argv[i]);
      return 1;
    }
  }

  printf("Measuring time to bin %ld particles (%.3f GP) using %s\n", numDataPoints, double(numDataPoints) / 1000000000,
#ifdef DOUBLE_PRECISION
         "double precision"
#else
	 "single precision"
#endif
);

  vector<double> times;
  vector<double> pps;

  InputDataType inputData;
  init_input_data(inputData, seed, numDataPoints);
  BinsType binnedData;
  
  for (size_t i = 0; i < repeat_times; ++i) {
    reset_bins(binnedData);
    double elapsed_time = measure_time(BinParticles, inputData, binnedData);
    if (i == 0) {
      if (result_output_file != "") {
        write_result(result_output_file, binnedData);
      }
      if (result_check_file != "") {
        check_result(result_check_file, binnedData);
      }
    }
    if (i >= warmup_times) {
      times.push_back(elapsed_time);
      pps.push_back(double(numDataPoints) / elapsed_time);
    }
    printf("    Run %2ld/%2ld: %7.2fs ⇒ %5.4g GP/s  %s\n", i + 1, repeat_times, elapsed_time, double(numDataPoints) / elapsed_time / 1000000000, i < warmup_times ? "(warmup)" : "");
  }
  
  double average_time = vector_average(times);
  double stddev_time = vector_stddev(times);
  double average_pps = vector_average_harmonic(pps);
  double stddev_pps = vector_stddev_harmonic(pps);
  printf(ESC_BOLD "Average time (s): %7.2f±%.2f  GP/s: %5.4g±%.4g " ESC_RESET "   numDataPoints = %ld \n", average_time, stddev_time, average_pps / 1000000000, stddev_pps / 1000000000, numDataPoints);

  free_input_data(inputData);

  return 0;
}
