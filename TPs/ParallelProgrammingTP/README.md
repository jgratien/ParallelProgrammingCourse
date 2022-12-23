Command lines

cd UE3-HPC/CPU/ParallelProgrammingCourse/TPs/ParallelProgrammingTP

source cemef.env

cd build

make install 2>&1 | tee make.log

oarsub -l core=32,walltime=00:00:15 -S ./run

oarsub -p "nbcores='24'" -l core=24,walltime=00:00:60 -S ./run