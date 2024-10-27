# ParallelProgrammingCourse
Parallel Programming Course

## I/ Introduction

This is a project for Education purpose :
- Introduction to parallel programming
- Tps to experiment uptodate C++ technologies to write efficient application

## II/ TPs

### A/ Using docker environment

a QuickSheet for docker main commands is [here](TPs/ParallelProgrammingTP/doc/Docker.md)

```bash
$ cd TPs/ParallelProgrammingTP
$ export UID=$(id -u)
$ export GID=$(id -g)
$ docker pull gratienj/parallelprogramming-tp-ubuntu2204:latest
$ docker run -it --rm -v $PWD:/app/ParallelProgrammingTP  gratienj/parallelprogramming-tp-ubuntu2204:latest bash
# to manage file rights permission you can use --user flag
$ docker run -it --rm --user $UID:$GID -v $PWD:/app/ParallelProgrammingTP  gratienj/parallelprogramming-tp-ubuntu2204:latest bash
#root@container
#root@container > cd app/ParallelProgrammingTP
#root@container > source ubuntu22.04.env
#root@container > mkdir build ; cd build
#root@container > cmake ..
#root@container > make 
#root@container > make install

```

### B/ How to update docker image and push in docker registry for <user>

```bash
$ cd TPs/ParallelProgrammingTP/docker
$ docker build -t <user>/<image-name>:<tag> .
$ docker login
$ docker push  <user>/<image-name>:<tag>

```
 
