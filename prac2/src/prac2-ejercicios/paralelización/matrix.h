#ifndef _matrix_h_
#define _matrix_h_

#include <cassert>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cstdlib>

template<typename T, size_t alignment>
constexpr size_t round_up_aligned(size_t w) {
  static_assert(alignment % sizeof(T) == 0);
  constexpr auto alignment_elements = alignment / sizeof(T);
  return (w + alignment_elements - 1) / alignment_elements * alignment_elements;
}

template<typename T, size_t matrix_alignment = 64>
struct Matrix {
  size_t width;
  size_t height;
  size_t width_aligned;
  T *data;

  Matrix(size_t h, size_t w) : width(w), height(h), width_aligned(round_up_aligned<T,matrix_alignment>(width)), data(static_cast<T*>(std::aligned_alloc(matrix_alignment, sizeof(T) * width_aligned * height))) {
    static_assert(matrix_alignment % sizeof(T) == 0);
    assert(width_aligned % (matrix_alignment / sizeof(T)) == 0);
    assert(width_aligned >= width);
  }
  Matrix(const Matrix& o) : width(o.width), height(o.height), width_aligned(round_up_aligned<T,matrix_alignment>(width)), data(static_cast<T*>(std::aligned_alloc(matrix_alignment, sizeof(T) * width_aligned * height))) {
    static_assert(matrix_alignment % sizeof(T) == 0);
    assert(width_aligned % (matrix_alignment / sizeof(T)) == 0);
    assert(width_aligned >= width);
    assert(o.width_aligned == width_aligned);
    std::copy(&o.data[0], &o.data[height * width_aligned], data);
  }
  Matrix& operator=(const Matrix& o) {
    if (o.width_aligned * o.height != width_aligned * height) {
      std::free(data);
      data = std::aligned_alloc(matrix_alignment, sizeof(T) * o.width_aligned * o.height);
    }
    width = o.width;
    height = o.height;
    width_aligned = round_up_aligned<T,matrix_alignment>(width);
    assert(o.width_aligned == width_aligned);
    std::copy(&o.data[0], &o.data[height * width_aligned], data);
    return *this;
  }
  
  ~Matrix() { std::free(data); }
  
  inline const T* operator[](size_t row) const { return std::assume_aligned<matrix_alignment>(&data[row * width_aligned]); }
  inline T* operator[](size_t row) { return std::assume_aligned<matrix_alignment>(&data[row * width_aligned]); }

  void swap_data(Matrix<T>& o) {
    assert(width == o.width);
    assert(height == o.height);
    assert(width_aligned == o.width_aligned);
    T* t = o.data;
    o.data = data;
    data = t;
  }
};

template<typename T, size_t a>
void printf_matrix(const char* element_format, const Matrix<T,a>& m) {
    for (size_t i = 0; i < m.height; ++i) {
      for (size_t j = 0; j < m.width; ++j) {
        printf(element_format, m[i][j]);
        putchar(' ');
      }
      putchar('\n');
    }
}
#endif
