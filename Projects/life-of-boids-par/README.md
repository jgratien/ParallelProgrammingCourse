## setup

```bash
source cemef.env
mkdir build
cd build
cmake ..
make install
```

## Run
```bash
./bin/life-of-boids --nb-threads 24 --nb-birds 1024
```
## Before running on OAR
- Clear the oarlogs/ dir

