#ifndef _BINNING_H_
#define _BINNING_H_

#ifdef DOUBLE_PRECISION
#define FTYPE double
#define SIN sin
#define COS cos
#else
#define FTYPE float
#define SIN sinf
#define COS cosf
#endif

struct Particle { 
  FTYPE r;
  FTYPE phi;
};

// Input data arrives as an array of cylindrical coordinates
// of particles: r and phi. Size of the array is numDataPoints
struct InputDataType {
  int numDataPoints;
  Particle* particles;
};

// The output type is a matrix of bins
const int nBinsX = 10;
const int nBinsY = 10;
typedef int BinsType[nBinsX][nBinsY];

// We assume that the radial coordinate does not exceed this value
const FTYPE maxMagnitudeR=5.0;

// Boundaries of bins:
const FTYPE xMin = -maxMagnitudeR*1.000001;
const FTYPE xMax = +maxMagnitudeR*1.000001;
const FTYPE yMin = -maxMagnitudeR*1.000001;
const FTYPE yMax = +maxMagnitudeR*1.000001;

// Reciprocal of widths of bins:
const FTYPE binsPerUnitX = (FTYPE)nBinsX/(xMax - xMin);
const FTYPE binsPerUnitY = (FTYPE)nBinsY/(yMax - yMin);

void BinParticles(const InputDataType& inputData, BinsType& outputBins);

#endif
