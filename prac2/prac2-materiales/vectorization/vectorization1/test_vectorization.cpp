#include <cstddef>
#include <memory>

using namespace std;

void sum_vectors_vectorized(float* dest, const float* srcA, const float* srcB, size_t array_size) {
  for (size_t i = 0; i < array_size; ++i) {
    dest[i] = srcA[i] + srcB[i];
  }
}

void sum_vectors_vectorized_restrict(float*__restrict__ dest, const float*__restrict__ srcA, const float*__restrict__ srcB, size_t array_size) {
  for (size_t i = 0; i < array_size; ++i) {
    dest[i] = srcA[i] + srcB[i];
  }
}

void sum_vectors_vectorized_aligned(float*__restrict__ dest, const float*__restrict__ srcA, const float*__restrict__ srcB, size_t array_size) {
  const float*__restrict__ a = assume_aligned<64>(srcA);
  const float*__restrict__ b = assume_aligned<64>(srcB);
  float*__restrict__ d = assume_aligned<64>(dest);
  for (size_t i = 0; i < array_size; ++i) {
    d[i] = a[i] + b[i];
  }
}

#if defined(__GNUC__) && !(defined(__INTEL_COMPILER) || defined(__clang__)) // Both ICC and CLang impersonate GCC by default (allegedly to increase compatibility), which makes harder identifying the actual GCC.
__attribute__((optimize("no-tree-vectorize")))
#endif
void sum_vectors_no_vectorized(float* dest, const float* srcA, const float* srcB, size_t array_size) {
#if defined(__clang__)
#pragma clang loop vectorize(disable)
#elif defined(__INTEL_COMPILER)
#pragma novector
#endif
  for (size_t i = 0; i < array_size; ++i) {
    dest[i] = srcA[i] + srcB[i];
  }
}

