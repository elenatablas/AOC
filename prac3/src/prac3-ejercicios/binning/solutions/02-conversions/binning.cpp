#include <cmath>
#include <omp.h>
#include "util.h"

#include "binning.h"

using namespace std;

void BinParticles(const InputDataType& inputData, BinsType& outputBins) {
  // Loop through all particle coordinates
  for (int i = 0; i < inputData.numDataPoints; i++) { 
    // Transforming from cylindrical to Cartesian coordinates:
    const FTYPE x = inputData.particles[i].r*COS(inputData.particles[i].phi);
    const FTYPE y = inputData.particles[i].r*SIN(inputData.particles[i].phi);

    // Calculating the bin numbers for these coordinates:
    const int iX = int((x - xMin)*binsPerUnitX);
    const int iY = int((y - yMin)*binsPerUnitY);

    // Incrementing the appropriate bin in the counter
    ++outputBins[iX][iY];
  }
}
