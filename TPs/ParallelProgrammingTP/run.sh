#!/bin/bash

### script pour executer des jobs sur Cluster Cemef
### [ 04/2019 ]

### indiquer nombre de core et duree max de votre job hh:mm:ss
#OAR -l core=24,walltime=00:50:00
### indiquer le nom de votre job
#OAR -n job_exemple
#OAR -O %jobid%.out
#OAR -E %jobid%.out

### execution par oar de $DENSEPATH avec $MYDATA
### usage : oarsub -S ./job_exemple.sh
### usage : oarsub -l core=32 -S ./job_exemple.sh

### indiquer votre executable
DENSEPATH=/gext/benjamin.bourbon/github/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/densemv_mpi.exe
SPARSEPATH=/gext/benjamin.bourbon/github/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/sparsemv_mpi.exe
# ### indiquer votre fichier de donnees
# MYDATA=


### en execution standand, rien a changer ci-apres

echo DEBUT
date

NP=$(cat $OAR_NODE_FILE | wc -l)
echo execution sur $NP coeurs
echo sur les nodes $(cat $OAR_NODE_FILE)

#MPIRUN="/softs/mpi/openmpi-1.10.7/gcc-7.3.0/bin/mpirun \
#  -n $NP \
#  -machinefile $OAR_NODE_FILE "

MPIRUN="/softs/mpi/openmpi-1.10.7/gcc-7.3.0/bin/mpirun"

set -x
export OMP_NUM_THREADS=1

if [ -f $DENSEPATH ] && [ -f $SPARSEPATH ]; then
  for np in 1 2 4 8 12 16 24
  do
    for nx in 50 100 150 200
    do
    $MPIRUN -np ${np}  $DENSEPATH --nx $nx > logfiles/log-dense-NX${nx}-NPI${np}
    $MPIRUN -np ${np}  $SPARSEPATH --nx $nx > logfiles/log-sparse-NX${nx}-NPI${np}
    done
  done
else
   echo "ERREUR : programme $DENSEPATH inexistant ou programme $SPARSEPATH inexistant"
fi


date
echo FIN
