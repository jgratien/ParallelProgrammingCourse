# Parallel Programming Project : 

To build the project : 

- cd build
- cmake ..
- make install

To run the project : 
oarsub -l host=1/core=28,walltime=00:01:00 -S ./run => for Dense and Sparse matrices
oarsub -l host=1/core=28,walltime=00:01:00 -S ./run_b => for life of boids

Projct path : 

/home/gext/meryame.boudhar/U3/ParallelProgrammingCourse
