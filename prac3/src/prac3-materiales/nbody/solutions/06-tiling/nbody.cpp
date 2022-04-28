#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <omp.h>

using namespace std;

struct ParticleSet { 
  float *x, *y, *z;
  float *vx, *vy, *vz; 
};

// Adapted to the new SoA: ParticleSet
void MoveParticles_Orig(const int nParticles, ParticleSet& particle, const float dt) {

  // Loop over particles that experience force
  for (int i = 0; i < nParticles; i++) { 

    // Components of the gravity force on particle i
    float Fx = 0, Fy = 0, Fz = 0; 
      
    // Loop over particles that exert force: vectorization expected here
    for (int j = 0; j < nParticles; j++) { 
      
      // Avoid singularity and interaction with self
      const float softening = 1e-20;

      // Newton's law of universal gravity
      const float dx = particle.x[j] - particle.x[i];
      const float dy = particle.y[j] - particle.y[i];
      const float dz = particle.z[j] - particle.z[i];
      const float drSquared = dx*dx + dy*dy + dz*dz + softening;
      const float drPower32 = pow(drSquared, 3.0/2.0);
	
      // Calculate the net force
      Fx += dx / drPower32;  
      Fy += dy / drPower32;  
      Fz += dz / drPower32;
    }

    // Accelerate particles in response to the gravitational force
    particle.vx[i] += dt*Fx; 
    particle.vy[i] += dt*Fy; 
    particle.vz[i] += dt*Fz;
  }

  // Move particles according to their velocities
  // O(N) work, so using a serial loop
  for (int i = 0 ; i < nParticles; i++) { 
    particle.x[i]  += particle.vx[i]*dt;
    particle.y[i]  += particle.vy[i]*dt;
    particle.z[i]  += particle.vz[i]*dt;
  }
}

void MoveParticles(const int nParticles, ParticleSet& particle, const float dt) {

const int TILE=4096*1;

  // Loop over particles that experience force
#pragma omp parallel for collapse(2)
  for (int jj = 0; jj < nParticles; jj+=TILE) {
    for (int i = 0; i < nParticles; i++) { 

      // Components of the gravity force on particle i
      float Fx = 0, Fy = 0, Fz = 0; 
      
      // Loop over particles that exert force: vectorization expected here
#if defined (__INTEL_COMPILER)
#pragma vector aligned
#endif
      for (int j = jj; j < jj+TILE; j++) { 
      
        // Avoid singularity and interaction with self
        const float softening = 1e-20f;

        // Newton's law of universal gravity
        const float dx = particle.x[j] - particle.x[i];
        const float dy = particle.y[j] - particle.y[i];
        const float dz = particle.z[j] - particle.z[i];
        const float rr = 1.0f/sqrtf(dx*dx + dy*dy + dz*dz + softening);
        const float drPowerN32 = rr*rr*rr;
	
        // Calculate the net force
        Fx += dx * drPowerN32;  
        Fy += dy * drPowerN32;  
        Fz += dz * drPowerN32;
      }

      // Accelerate particles in response to the gravitational force
#pragma omp atomic
      particle.vx[i] += dt*Fx;
#pragma omp atomic
      particle.vy[i] += dt*Fy;
#pragma omp atomic
      particle.vz[i] += dt*Fz;
    }
  }

  // Move particles according to their velocities
  // O(N) work, so using a serial loop
#if defined (__INTEL_COMPILER)
#pragma vector aligned
#endif
  for (int i = 0 ; i < nParticles; i++) {
    particle.x[i]  += particle.vx[i]*dt;
    particle.y[i]  += particle.vy[i]*dt;
    particle.z[i]  += particle.vz[i]*dt;
  }
}

const float tolerance = 1e-2f;

bool compare_particle_arrays(int nParticles, const ParticleSet& a, const ParticleSet& b) {
  for(int i = 0; i < nParticles; i++) {
    if (abs(a.x[i] - b.x[i]) > tolerance) {
      printf("%d x %g %g %g\n", i, a.x[i], b.x[i], abs(a.x[i] - b.x[i]));
      return false;
    }
    if (abs(a.y[i] - b.y[i]) > tolerance) {
      printf("%d y %g %g %g\n", i, a.y[i], b.y[i], abs(a.y[i] - b.y[i]));
      return false;
    }
    if (abs(a.z[i] - b.z[i]) > tolerance) {
      printf("%d z %g %g %g\n", i, a.z[i], b.z[i], abs(a.z[i] - b.z[i]));
      return false;
    }
    if (abs(a.vx[i] - b.vx[i]) > tolerance) {
      printf("%d vx %g %g %g\n", i, a.vx[i], b.vx[i], abs(a.vx[i] - b.vx[i]));
      return false;
    }
    if (abs(a.vy[i] - b.vy[i]) > tolerance) {
      printf("%d vy %g %g %g\n", i, a.vy[i], b.vy[i], abs(a.vy[i] - b.vy[i]));
      return false;
    }
    if (abs(a.vz[i] - b.vz[i]) > tolerance) {
      printf("%d vz %g %g %g\n", i, a.vz[i], b.vz[i], abs(a.vz[i] - b.vz[i]));
      return false;
    }
  }
  return true;
}
  
int main(const int argc, const char** argv) {

  // Problem size and other parameters
  const int nParticles = (argc > 1 ? atoi(argv[1]) : 16384);
  const int nSteps = 10;  // Duration of test
  const float dt = 0.01f; // Particle propagation time step

  // Particle data stored as a Structure of Arrays (SoA)
  // this may not be good in object-oriented programming,
  // however, makes vectorization much more efficient
  ParticleSet particle; 
  particle.x  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle.y  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle.z  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle.vx = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle.vy = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle.vz = (float*) aligned_alloc(32, sizeof(float)*nParticles);

  // Initialize random number generator and particles
  mt19937 generator(0); // 32 bit Mersenne Twister pseudo-random generator
  uniform_real_distribution<float> distribution(0,1);
  for(int i = 0; i < nParticles; i++) {
    particle.x[i] = distribution(generator);
    particle.y[i] = distribution(generator);
    particle.z[i] = distribution(generator);
    particle.vx[i] = distribution(generator);
    particle.vy[i] = distribution(generator);
    particle.vz[i] = distribution(generator);
  }

  // Para calcular el resultado orginal y comprobar que da lo mismo
  ParticleSet particle_reference; 
  particle_reference.x  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle_reference.y  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle_reference.z  = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle_reference.vx = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle_reference.vy = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  particle_reference.vz = (float*) aligned_alloc(32, sizeof(float)*nParticles);
  // Setting particle_reference to the same initial values
  for(int i = 0; i < nParticles; i++) {
    particle_reference.x[i] = particle.x[i];
    particle_reference.y[i] = particle.y[i];
    particle_reference.z[i] = particle.z[i];
    particle_reference.vx[i] = particle.vx[i];
    particle_reference.vy[i] = particle.vy[i];
    particle_reference.vz[i] = particle.vz[i];
  }
  // Calcualte reference solution
  MoveParticles_Orig(nParticles, particle_reference, dt);
  
  // Perform benchmark
  printf("\n\033[1mNBODY Version 05\033[0m\n");
  printf("\nPropagating %d particles using %d threads on %s...\n\n", 
	 nParticles, omp_get_max_threads(), "CPU");
  double rate = 0, dRate = 0; // Benchmarking data
  const int skipSteps = 3; // Skip first iterations to warm-up
  printf("\033[1m%5s %10s %10s %8s\033[0m\n", "Step", "Time, s", "Interact/s", "GFLOP/s"); fflush(stdout);
  for (int step = 1; step <= nSteps; step++) {

    const double tStart = omp_get_wtime(); // Start timing
    MoveParticles(nParticles, particle, dt);
    const double tEnd = omp_get_wtime(); // End timing

    const float HztoInts   = float(nParticles)*float(nParticles-1) ;
    const float HztoGFLOPs = 20.0*1e-9*float(nParticles)*float(nParticles-1);

    if (step > skipSteps) { // Collect statistics
      rate  += HztoGFLOPs/(tEnd - tStart); 
      dRate += HztoGFLOPs*HztoGFLOPs/((tEnd - tStart)*(tEnd-tStart)); 
    }

    printf("%5d %10.3e %10.3e %8.1f %s\n", 
	   step, (tEnd-tStart), HztoInts/(tEnd-tStart), HztoGFLOPs/(tEnd-tStart), (step<=skipSteps?"*":""));
    fflush(stdout);

    // Checking final values
    if (step == 1) {
      if (!compare_particle_arrays(nParticles, particle_reference, particle)) {
        printf("-----------------------------------------------------\n");
        printf("\033[31mDO PANIC: The final result is wrong \033[0m\n");
        printf("-----------------------------------------------------\n");
        return 1;
      }
    }
  }
  rate/=(double)(nSteps-skipSteps); 
  dRate=sqrt(dRate/(double)(nSteps-skipSteps)-rate*rate);
  printf("-----------------------------------------------------\n");
  printf("\033[1m%s %4s \033[32m%10.1f +- %.1f GFLOP/s\033[0m\n",
	 "Average performance:", "", rate, dRate);
  printf("-----------------------------------------------------\n");
  printf("* - warm-up, not included in average.\n");
  printf("The final result after 1 iteration was correct.\n\n");

  free( particle.x );
  free( particle.y );
  free( particle.z );
  free( particle.vx );
  free( particle.vy );
  free( particle.vz );
  free( particle_reference.x );
  free( particle_reference.y );
  free( particle_reference.z );
  free( particle_reference.vx );
  free( particle_reference.vy );
  free( particle_reference.vz );

  return 0;
}

