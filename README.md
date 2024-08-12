# HairSim
A particle-based simulation of hair written in C++ with GLOO.
## Summary
This program is based on the particle-based method of hair simulation described by L. Petrovic, M. Henne, J. Anderson's *Volumetric
Methods for Simulation and Rendering of Hair* from Pixar Animation Studios, and aims to emulate the inextensibility and micro-scale interactions of hair.

The simulation uses FTL-based position and velocity
corrections to guarantee that the distance between hair particles is constant, which imitates the inextensibility of individual strands. The simulation space is split into voxel grids that store the particle density and average velocity at each vertex in the grid.
 The average velocity is used for velocity smoothing, which encourages hair to behave as a whole and simulates frictional interactions between strands. The gradient of the density grid with respect to particle position is used to simulate hair-hair repulsion and create the appearance of volume.

