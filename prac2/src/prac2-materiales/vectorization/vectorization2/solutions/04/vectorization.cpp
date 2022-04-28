/* Copyright (c) 2013-2015, Colfax International. All Right Reserved.
This file, labs/3/3.01-vectorization/solutions/instruction-02/vectorization.cc,
is a part of Supplementary Code for Practical Exercises for the handbook
"Parallel Programming and Optimization with Intel Xeon Phi Coprocessors",
2nd Edition -- 2015, Colfax International,
ISBN 9780988523401 (paper), 9780988523425 (PDF), 9780988523432 (Kindle).
Redistribution or commercial usage without written permission 
from Colfax International is prohibited.
Contact information can be found at http://colfax-intl.com/     */



#include <cstdio>
#include <cstdlib>
#include <cmath>

//adds two numbers and returns the sum
double my_scalar_add(double, double);
//adds the second vector to the first
void my_vector_add(int, double*, double*);

int main(){
    
  const int n=10000;
  const int maxIter = 5;
  double a[n], b[n];

  for(int i = 0 ; i < n ; i++) {  // Initialize
     a[i]=1.0/(double)n;
     b[i]=2.0;
  }
  
  for(int i = 0 ; i < n ; i++)   // Addition
     b[i]= a[i] + b[i];

#pragma omp simd
  for(int i = 0 ; i < n ; i++)   // Scalar function call
     a[i]=my_scalar_add(a[i],b[i]);

  my_vector_add(n,a,b);          // Vector function call
}
