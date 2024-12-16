compile : ./build.sh
run : ./life-of-boids np nb_birds mode (seq,tbb,mpi)

run benchmark : oarsub -l core=32,walltime=00:60:00 -S ./run.oar
Results are write in Bench_birds.log
