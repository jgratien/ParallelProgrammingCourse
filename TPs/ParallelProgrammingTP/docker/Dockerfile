# Use the official Ubuntu 22.04 image
#FROM ubuntu:22.04
ARG http_proxy=http://irproxy.ifpen.fr:8082
ARG https_proxy=http://irproxy.ifpen.fr:8082
ARG ftp_proxy=http://irproxy.ifpen.fr:8082
ARG ftps_proxy=http://irproxy.ifpen.fr:8082
ARG no_proxy=.ifp.fr,.ifpen.fr,localhost,127.0.0.1

FROM harbor.ifpen.fr/library/ubuntu:22.04

##############################
### Configure Repositories ###
##############################

RUN sed -e 's,http://archive.ubuntu.com/,https://repos.ifpen.fr/repository/ubuntu_update/,g' \
        -e 's,http://security.ubuntu.com/,https://repos.ifpen.fr/repository/ubuntu_update/,g' \ 
        -i /etc/apt/sources.list

RUN apt-get -o Acquire::https::repos.ifpen.fr::Verify-Peer=false -o Acquire::https::repos.ifpen.fr::Verify-Host=false update && \
    apt-get -o Acquire::https::repos.ifpen.fr::Verify-Peer=false -o Acquire::https::repos.ifpen.fr::Verify-Host=false install -y ca-certificates

#######################
### Update packages ###
#######################

RUN apt-get update && apt-get full-upgrade -y \
    && rm -rf /var/lib/apt/lists/*

########################
### Install packages ###
########################

# Update package list and install necessary packages
RUN apt-get update && \
    apt-get install -y \
    apt-utils \
    build-essential \
    gcc \
    g++ \
    git \
    cmake \
    libtbb-dev \
    libeigen3-dev \
    openmpi-common \
    openmpi-bin \
    libboost-all-dev 

# Set the working directory


# Create a volume at /app/data
VOLUME /app/ParallelProgrammingTP

CMD bash


# Your additional instructions, if any, can go here

# CMD or ENTRYPOINT instruction, if needed
