# Ubuntu 20.04 dependencies for Alien
FROM ubuntu:20.04

LABEL maintainer.email=<jean-marc.gratien@ifpen.fr>

ARG DEBIAN_FRONTEND=noninteractive
ARG TZ=Europe/Paris
#ARG http_proxy=http://irproxy.ifpen.fr:8082
#ARG https_proxy=http://irproxy.ifpen.fr:8082


RUN apt-get update \
    && apt-get install --no-install-recommends -y build-essential \
     autoconf autotools-dev cmake gawk gnuplot \
     gcc g++ gdb \
     curl libssl-dev wget git \
     flex libfl-dev libreadline-dev zlib1g-dev \
     openmpi-bin libopenmpi-dev mpi-default-bin mpi-default-dev \
     libgmp-dev libmpfr-dev libmpc-dev libtbb-dev \
     libboost-all-dev libboost-program-options-dev libboost-filesystem-dev libopencv-dev libcgal-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

#RUN apt-get -y install git 
#RUN apt-get -y install git subversion iproute2 procps lsb-release libssl-dev wget

# Install C++ tools
#RUN apt-get -y install build-essential npm rsyn clang-9 g++-8 python3  python3-dev  python3-setuptools  python3-sympy
#RUN apt-get -y install build-essential autoconf autotools-dev cmake gawk gnuplot
#RUN apt-get -y install flex libfl-dev libreadline-dev zlib1g-dev openmpi-bin libopenmpi-dev mpi-default-bin mpi-default-dev
#RUN apt-get -y install libgmp-dev libmpfr-dev libmpc-dev
#

