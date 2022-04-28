#include "heat.h"

#include <iostream>

using namespace std;

extern bool verbose;

/*
 * Simula la propagación de calor en una superficie representada por
 * la matriz «state».
 *
 * Recibe el estado inicial en «state» y modifica la matriz
 * iterativamente hasta que el cambio medio por elemento elemento
 * entre dos itereciones es menor que «tolerance».
 *
 * Devuelve el estado final en «state», el número de iteraciones
 * empleado en «iterations» y el mayor cambio de un elemento durante
 * la última itereción en «last_difference».
 */
void solve(Matrix<double>& state, double tolerance, int& iterations, double& last_difference) {
  Matrix<double> next_state = state;
  iterations = 0;
  double difference;
  do {
    difference = 0;
    for (size_t i = 1; i < state.height - 1; ++i) {
      for (size_t j = 1; j < state.width - 1; ++j) {
        next_state[i][j] = (state[i][j]
                            + state[i + 1][j    ]  
                            + state[i - 1][j    ]  
                            + state[i    ][j + 1]  
                            + state[i    ][j - 1]) / 5;
        difference = difference + abs(next_state[i][j] - state[i][j]);
      }
    }

    state.swap_data(next_state);
    
    if (verbose) {
      cout << "Iteration " << iterations << ":" << endl;
      printf_matrix("%7.3f", state);
      cout << "Difference: " << difference << endl;
    }
    ++iterations;
  } while (difference / (state.height * state.width) > tolerance);
  last_difference = difference / (state.height * state.width);
}

