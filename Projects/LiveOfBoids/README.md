# Parallelization of the life_of_boids project using omp and tbb

## Build

You can run these commands in a directory in the root folder.
```
cmake .
cmake --build .
```

## Running the program

### Using the executable

When built, the executable file to launch is **freebirds**. By default, the program will be generated with 1000 boids.

```
./src/freebirds
./src/freebirds --nbAgents
./src/freebirds --nbAgents --nbThreads  
./src/freebirds --nbAgents --nbThreads --mode
```
**mode in ["seq", "omp", "tbb"]**

### Using the script run
```
oarsub -l core=32,walltime=00:60:00 -S ./run
```