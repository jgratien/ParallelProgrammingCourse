# Docker QUICK SHEET


## I/ Introduction


### A/ What is Docker

```
WIKIPEDIA : 
an open-source project that automates the deployment of software applications inside
containers by providing an additional layer of abstraction and automation of OS-level 
virtualization on Linux.
```

A tool to create portable environnement to run executables.

- The OS layer, physical ressources (memory, hard disk, CPU, GPU are shared)

- different from Virtual Machines that have their own OS Layer and private ressources 

![](./docker-containers-vms.png)


### B/ Docker main concepts

- Container :

- Images :

- Docker engine

- Docker Registry : database to manage and share images

### C/ How to install

- Exemple of procedure to install docker on Ubuntu

```bash
$ sudo apt-get update

$ sudo apt-get install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    lsb-release
    
$ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

$ sudo apt-get update

$ sudo apt-get install docker-ce docker-ce-cli containerd.io

$ sudo apt install docker-compose
```

- Test installation

```bash
$ sudo docker run hello-world
```

- Post install procedure

Create docker group than add user to group
```bash
$ sudo groupadd docker

$ sudo usermod -aG docker $USER
```

- Enable start on boot and manage docker services

Enable docker service (by default activated on Ubuntu

```bash
$ sudo systemctl enable docker.service
$ sudo systemctl enable containerd.service
```

- Disable docker service

```bash
$ sudo systemctl disable docker.service
$  sudo systemctl disable containerd.service
```

- Configure remote acces

--First solution : Editing the systemctl docker.service config file

```bash

$ sudo vi /lib/systemd/system/docker.service
#
# modify the following
ExecStart=/usr/bin/dockerd -H fd:// -H tcp://127.0.0.1:2375 -H unix:///var/run/docker.sock
# ...

```

Reload the systemctl configuration than restart docker service

```bash
$ sudo systemctl daemon-reload
$ sudo systemctl restart docker.service

# CHECK MODIFICATIONS
$ sudo netstat -lntp | grep dockerd
```

--Second solution (cannot be used with first solution because of conflicts)

Set the hosts array in the /etc/docker/daemon.json to connect to the UNIX socket and an IP address, as follows:

```bash
{
  "hosts": [
    "fd://",
    "unix:///var/run/docker.sock",
    "tcp://127.0.0.1:2375"
   ]
}

```

Restart docker service

```bash
$ sudo systemctl restart docker.service

# CHECK MODIFICATIONS
$ sudo netstat -lntp | grep dockerd
```


- Secure the daemon remote acces with tls : https://docs.docker.com/engine/security/protect-access/}

Generate tls keys for the server and for the client with the following procedure:

Let HOST be the dns name of the docker remote server with IP :10.10.10.20

```bash

$ openssl genrsa -aes256 -out ca-key.pem 4096
$ openssl req -new -x509 -days 365 -key ca-key.pem -sha256 -out ca.pem
$ openssl genrsa -out server-key.pem 4096
$ openssl req -subj "/CN=$HOST" -sha256 -new -key server-key.pem -out server.csr
$ echo subjectAltName = DNS:$HOST,IP:10.10.10.20,IP:127.0.0.1 >> extfile.cnf
$ echo extendedKeyUsage = serverAuth >> extfile.cnf
$ openssl x509 -req -days 365 -sha256 -in server.csr -CA ca.pem -CAkey ca-key.pem \
  -CAcreateserial -out server-cert.pem -extfile extfile.cnf
$ openssl genrsa -out key.pem 4096
$ echo extendedKeyUsage = clientAuth > extfile-client.cnf
$ openssl x509 -req -days 365 -sha256 -in client.csr -CA ca.pem -CAkey ca-key.pem \
  -CAcreateserial -out cert.pem -extfile extfile-client.cnf
$ chmod -v 0400 ca-key.pem key.pem server-key.pem
$ chmod -v 0444 ca.pem server-cert.pem cert.pem
$ cp ca.pem server-cert.pem server-key.pem /etc/docker/var/.
```

Add the following options to the docker.service configuration

```bash
--tlsverify \
--tlscacert=/etc/docker/var/ca.pem \
--tlscert=/etc/docker/var/server-cert.pem \
--tlskey=/etc/docker/var/server-key.pem 
```

In the client side put the client key files in the $HOME/.docker directory:

```bash
cp ca.perm cert.perm and key.perm $HOME/.docker
```

## II/ Main commands

```bash
# List of images
$ docker images

# List and states of images
$ docker ps -images
$ docker container ls

# Get "busybox" image
$ docker pull busybox

# run iteractively busybox image 
$ docker run -it busybox sh

$ docker rm 305297d7a235 ff0a5c3750b9

$ docker run --rm ...

$ docker container prune

# run one image give name to container expose all ports
$ docker run -d -P --name "my-container-name" "docker-image-name"

# print port association
$ docker port "my-container-name"

#run on image associate port 8888 on Host to 80 in container
$ docker run -p 8888:80 "docker-image-name"

#stop container
$ docker stop "my-container-name"

# build image and give tag-name
$ docker build -t <tag-name> <path_to_DockerFile_directory>

# copy files in a container
$ docker cp foo.txt container_id:/foo.txt

```
## III/ Get official images or from Docker registry

```bash
>docker --version

>docker search 'my-appli'

>docker container run -ti my-appli/version:tag

```

## IV/ How to run an application within a containner

- Best praticse : no data within container

- mount volume to keep data

- mount port to exchange with other applications

```bash
docker container run -ti --rm -v $PWD:/data -w /data \
    my-apply/version:tag ./script-run.sh
```


## VI/ How to build your own Application image

```bash
cd ubuntu/tag0 ; docker build --build-arg http_proxy=http://irproxy:8082 -t myusername/myappli:tag0 .
cd ubuntu/tag1 ; docker build -t myusername/myappli:tag1 .
cd ubuntu/tag2 ; docker build -t myusername/myappli:tag2 .
```

```bash
docker run -d -rm -v $PWD:/data -w /data \
                  -p 8888:8000 myusername/myappli:tag0 \
                  ./script-run.sh
```


## VII/ How to push your own Appli image to a Docker registry


Exemple with gitlab.ifpen.fr

```
# FIRST LOGIN
>docker login gitlab.ifpen.fr:4567

# BUILD YOUR IMAGE
docker build -t gitlab.ifpen.fr:4567/myproject/myappli .

# PUSH IMAGGE to GITLAB IFPEN REGISTRY
docker push gitlab.ifpen.fr:4567/myproject/myappli
```

