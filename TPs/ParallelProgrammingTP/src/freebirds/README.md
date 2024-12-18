
# Project Name: Boid Simulation

## Description

This project simulates boid behavior (a flocking algorithm) using different parallelization techniques (sequential, OpenMP, and TBB). It includes a grid-based environment where agents (boids) interact with each other based on simple rules like alignment, cohesion, and separation. The simulation can be run in different modes to compare the performance of the algorithms using OpenMP or TBB for parallelization.

## Features

- **Grid-based environment**: Agents are placed on a grid, and their interactions are determined based on neighboring agents in nearby grid cells.
- **Parallelized simulations**: You can choose between sequential execution, OpenMP (multi-threading), or TBB (Threading Building Blocks) for parallel processing.
- **Agent behaviors**: The boids (agents) follow simple flocking rules such as alignment, cohesion, and avoidance of predators or obstacles.
- **Dynamic threading**: The number of threads used can be set dynamically for both OpenMP and TBB, adjusting based on the number of CPU cores available or a specified value.

## Prerequisites

Before running the project, ensure you have the following installed:

- **CMake**: A build system for C++ projects.
- **A C++ compiler**: Such as GCC or Clang.
- **OpenMP**: A parallel programming model for shared memory systems.
- **Intel Threading Building Blocks (TBB)**: A library for parallel programming.
- **GLFW**: A library for creating windows with OpenGL contexts.
- **GLAD**: An OpenGL loader to handle OpenGL extensions.
- **fmt**: A modern formatting library for C++.

To install dependencies (on Ubuntu-based systems), you can run:

```bash
sudo apt-get install cmake g++ libomp-dev libtbb-dev libglfw3-dev libglad-dev libfmt-dev conan
```

## Building the Project

### Step 1: Create a build directory and navigate to it

```bash
mkdir build
cd build
```

### Step 2: Install dependencies using Conan

```bash
conan install .. -of . --build=missing
```

### Step 3: Generate Makefiles using CMake

```bash
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
```

### Step 4: Build the project

```bash
cmake --build .
```

This will generate the executable `freebirds` inside the `src` directory.

## Running the Project

After building the project, you can run the simulation in one of the three modes: **sequential**, **OpenMP**, or **TBB**. The executable is located in the `src` directory.

To run the program, use the following command:

```bash
cd src
./freebirds mode nbAgents NbThreads
```

Where:

- `mode` is the execution mode:
    - `0`: Sequential (no parallelization)
    - `1`: OpenMP (parallelization using OpenMP)
    - `2`: TBB (parallelization using Intel TBB)
- `nbAgents` is the number of agents in the simulation.
- `NbThreads` is the number of threads to use (if applicable, for OpenMP or TBB).
