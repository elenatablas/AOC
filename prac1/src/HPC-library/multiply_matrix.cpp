#include "matrix.h"

void multiply_matrix_basic(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB) {
  assert(srcA.height == dest.height && srcB.width == dest.width && srcA.width == srcB.height);
  for (size_t i = 0; i < srcA.height; ++i) {
    for (size_t j = 0; j < srcB.width; ++j) {
      dest[i][j] = 0;
      for (size_t k = 0; k < srcA.width; ++k) {
        dest[i][j] += srcA[i][k]* srcB[k][j];
      }
    }
  }
}

void multiply_matrix_ikj(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB) {
  assert(srcA.height == dest.height && srcB.width == dest.width && srcA.width == srcB.height);
  for (size_t i = 0; i < dest.height; ++i) {
    for (size_t j = 0; j < dest.width; ++j) {
      dest[i][j] = 0;
    }
  }
  for (size_t i = 0; i < srcA.height; ++i) {
    for (size_t k = 0; k < srcA.width; ++k) {
      for (size_t j = 0; j < srcB.width; ++j) {
        dest[i][j] += srcA[i][k]* srcB[k][j];
      }
    }
  }
}

void multiply_matrix_basic_omp(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB) {
  assert(srcA.height == dest.height && srcB.width == dest.width && srcA.width == srcB.height);
# pragma omp parallel for collapse(2)
  for (size_t i = 0; i < dest.height; ++i) {
    for (size_t j = 0; j < dest.width; ++j) {
      dest[i][j] = 0;
      for (size_t k = 0; k < srcA.width; ++k) {
        dest[i][j] += srcA[i][k] * srcB[k][j];
      }
    }
  }
}

void multiply_matrix_ikj_omp(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB) {
  assert(srcA.height == dest.height && srcB.width == dest.width && srcA.width == srcB.height);
# pragma omp parallel for collapse(2)
  for (size_t i = 0; i < dest.height; ++i) {
    for (size_t j = 0; j < dest.width; ++j) {
      dest[i][j] = 0;
    }
  }
# pragma omp parallel for collapse(2)
  for (size_t i = 0; i < srcA.height; ++i) {
    for (size_t k = 0; k < srcA.width; ++k) {
      for (size_t j = 0; j < srcB.width; ++j) {
        dest[i][j] += srcA[i][k]* srcB[k][j];
      }
    }
  }
}

#ifndef USE_MKL
#include <cblas.h>
#else // USE_MKL
#include <mkl.h>
#endif

void multiply_matrix_blas(Matrix<float>& dest, const Matrix<float>& srcA, const Matrix<float>& srcB) {
  assert(srcA.height == dest.height && srcB.width == dest.width && srcA.width == srcB.height);
  /* C = ALPHA × (A × B) + BETA × C */
  cblas_sgemm(CblasRowMajor,
              CblasNoTrans, CblasNoTrans, /* Do not transpose anythng */
              dest.height, dest.width, srcA.width, /* M N K */
              1, /* ALPHA */
              srcA.data, /* M × K matrix */
              srcA.width_aligned /* lda */,
              srcB.data, /* K × N matrix */
              srcB.width_aligned /* ldb */,
              0, /* BETA */
              dest.data, /* M × N matrix */
              dest.width_aligned /* ldc */);
}

