#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <omp.h>

using namespace std;

struct ParticleType { 
  float x, y, z;
  float vx, vy, vz; 
};

void MoveParticles_Orig(const int nParticles, ParticleType* const particle, const float dt) {

  // Loop over particles that experience force
  for (int i = 0; i < nParticles; i++) { 

    // Components of the gravity force on particle i
    float Fx = 0, Fy = 0, Fz = 0; 
      
    // Loop over particles that exert force: vectorization expected here
    for (int j = 0; j < nParticles; j++) { 
      
      // Avoid singularity and interaction with self
      const float softening = 1e-20;

      // Newton's law of universal gravity
      const float dx = particle[j].x - particle[i].x;
      const float dy = particle[j].y - particle[i].y;
      const float dz = particle[j].z - particle[i].z;
      const float drSquared = dx*dx + dy*dy + dz*dz + softening;
      const float drPower32 = pow(drSquared, 3.0/2.0);
	
      // Calculate the net force
      Fx += dx / drPower32;  
      Fy += dy / drPower32;  
      Fz += dz / drPower32;
    }

    // Accelerate particles in response to the gravitational force
    particle[i].vx += dt*Fx; 
    particle[i].vy += dt*Fy; 
    particle[i].vz += dt*Fz;
  }

  // Move particles according to their velocities
  // O(N) work, so using a serial loop
  for (int i = 0 ; i < nParticles; i++) { 
    particle[i].x  += particle[i].vx*dt;
    particle[i].y  += particle[i].vy*dt;
    particle[i].z  += particle[i].vz*dt;
  }
}

void MoveParticles(const int nParticles, ParticleType* const particle, const float dt) {

  // Loop over particles that experience force
  for (int i = 0; i < nParticles; i++) { 

    // Components of the gravity force on particle i
    float Fx = 0, Fy = 0, Fz = 0; 
      
    // Loop over particles that exert force: vectorization expected here
    for (int j = 0; j < nParticles; j++) { 
      
      // Avoid singularity and interaction with self
      const float softening = 1e-20;

      // Newton's law of universal gravity
      const float dx = particle[j].x - particle[i].x;
      const float dy = particle[j].y - particle[i].y;
      const float dz = particle[j].z - particle[i].z;
      const float rr = 1.0f/sqrtf(dx*dx + dy*dy + dz*dz + softening);
      const float drPowerN32 = rr*rr*rr;
	
      // Calculate the net force
      Fx += dx * drPowerN32;  
      Fy += dy * drPowerN32;  
      Fz += dz * drPowerN32;
    }

    // Accelerate particles in response to the gravitational force
    particle[i].vx += dt*Fx; 
    particle[i].vy += dt*Fy; 
    particle[i].vz += dt*Fz;
  }

  // Move particles according to their velocities
  // O(N) work, so using a serial loop
  for (int i = 0 ; i < nParticles; i++) { 
    particle[i].x  += particle[i].vx*dt;
    particle[i].y  += particle[i].vy*dt;
    particle[i].z  += particle[i].vz*dt;
  }
}

const float tolerance = 1e-2f;

bool compare_particle_arrays(int nParticles, const ParticleType* const a, const ParticleType* const b) {
  for(int i = 0; i < nParticles; i++) {
    if (abs(a[i].x - b[i].x) > tolerance) {
      printf("%d x %g %g %g\n", i, a[i].x, b[i].x, abs(a[i].x - b[i].x));
      return false;
    }
    if (abs(a[i].y - b[i].y) > tolerance) {
      printf("%d y %g %g %g\n", i, a[i].y, b[i].y, abs(a[i].y - b[i].y));
      return false;
    }
    if (abs(a[i].z - b[i].z) > tolerance) {
      printf("%d z %g %g %g\n", i, a[i].z, b[i].z, abs(a[i].z - b[i].z));
      return false;
    }
    if (abs(a[i].vx - b[i].vx) > tolerance) {
      printf("%d vx %g %g %g\n", i, a[i].vx, b[i].vx, abs(a[i].vx - b[i].vx));
      return false;
    }
    if (abs(a[i].vy - b[i].vy) > tolerance) {
      printf("%d vy %g %g %g\n", i, a[i].vy, b[i].vy, abs(a[i].vy - b[i].vy));
      return false;
    }
    if (abs(a[i].vz - b[i].vz) > tolerance) {
      printf("%d vz %g %g %g\n", i, a[i].vz, b[i].vz, abs(a[i].vz - b[i].vz));
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
  
  // Particle data stored as an Array of Structures (AoS)
  // this is good object-oriented programming style,
  // but inefficient for the purposes of vectorization
  ParticleType* particle = new ParticleType[nParticles];
  
  // Initialize random number generator and particles
  mt19937 generator(0); // 32 bit Mersenne Twister pseudo-random generator
  uniform_real_distribution<float> distribution(0,1);
  for(int i = 0; i < nParticles; i++) {
    particle[i].x = distribution(generator);
    particle[i].y = distribution(generator);
    particle[i].z = distribution(generator);
    particle[i].vx = distribution(generator);
    particle[i].vy = distribution(generator);
    particle[i].vz = distribution(generator);
  }

  // Para calcular el resultado orginal y comprobar que da lo mismo
  ParticleType* particle_reference = new ParticleType[nParticles];
  // Setting particle_reference to the same initial values
  for(int i = 0; i < nParticles; i++) {
    particle_reference[i] = particle[i];
  }
  // Calcualte reference solution
  MoveParticles_Orig(nParticles, particle_reference, dt);
  
  // Perform benchmark
  printf("\n\033[1mNBODY Version 02\033[0m\n");
  printf("\nPropagating %d particles using 1 thread on %s...\n\n", 
	 nParticles, "CPU");
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

  delete[] particle;
  delete[] particle_reference;

  return 0;
}
