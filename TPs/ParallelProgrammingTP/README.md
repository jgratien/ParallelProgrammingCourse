## Useful command lines:

cd ParallelProgrammingCourse/TPs/ParallelProgrammingTP && source cemef.env

cd build && make install 2>&1 | tee make.log && cd ..

## To execute:
### Dense Matrix:
oarsub -l host=1/core=28,walltime=00:60:00 -S ./run

### Sparse Matrix:
oarsub -l host=1/core=28,walltime=00:60:00 -S ./runs

### Life of Boids:
oarsub -l host=1/core=28,walltime=00:60:00 -S ./runl
