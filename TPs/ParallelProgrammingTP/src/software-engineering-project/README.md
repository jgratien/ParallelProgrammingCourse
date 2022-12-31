# Simulation project of a flock of birds

## Description

Boids, “bird-oid objects”, is the combination of simple rules that simulates flock behavior. Boids was originally introduced by computer graphics expert, Craig Reynolds, who also worked on scenes for the original Tron movie from 1982 and Batman Returns in 1992. It consists of three fundamental rules: Cohesion, Alignment, and Separation. Just like many other emergent behaviors, each bird can only register and apply these rules to its immediate neighbors within the limited ranges.

![](extras/life-of-boids.gif)

### Project Features
- Trees (in brown) can drop fruits (in green) around themselves
- Birds (in white) attract one another to form flocks
- Birds can eat fruits
- Predators (in red) can eat birds
- Birds/Predators avoid obstacles (rectangles)
- User can spawn new birds, and predators at the cursor's location by pressing 'b', and 'p' respectively
- Birds can be spawned at locations where fruits are eaten
- Agents can cross the screen and re-appear from the opposite side

### Initialisation Rules
- Begin by initialising obstacles:
    - can't overlap with eah other
    - minimum distance from the border
- Next are predators:
    - can't overlap with over predators or birds
    - can't be close to an obstacle (initial state != near_obstacle)
- Then are birds:
    - same conditions as predators
- Tree:
    - can't overlap with each other
    - minimum distance from an obstacle, minimum distance from the border
- Fruit initialisation is made with tree.DropFruitAndAppend and will be called for every tree in the updateObjects function. Then the fruits will be dropped depending of the random time variable in the tree object. A random number of fruit is dropped between 1 and DEFAULT_NUM_FRUITS_DROPS.

### Laws
- **SeparationLaw:** steer to avoid crowding local flockmates
- **biSeparationLaw:** mean of the angle obtained from separating from a bird and from a predator
- **AlignmentLaw:** steer towards the average heading of local flockmates
- **CohesionLaw:** steer to move towards the average position (center of mass) of local flockmates
- **PreyLaw:** will chase closest bird to eat it, use predator relaxation
- **ObstacleLaw:** steer to avoid flying in an obstacle, differ from separationLaw by using obstacle relaxation
- **FruitLaw:** will chase a fruit as a predator would do, use predator relaxation
- **biFruitLaw:** mean of the angles obtained from chasing a fruit and separating from other birds

### Update of each object
1. For Predators

    - Check closest obstacle, if near, state=near_obstacle and apply ObstacleLaw
    - if state!=near_obstacle, if near other predators, state=separation and apply SeparationLaw
    - if state!=separation, search for closest prey in field of view with no maximum distance constraint, state=near_prey and apply PreyLaw

2. For Birds

    - check the closest predator, if predator close, state=near_predator, if predator too close, m_alive=false
    - if m_alive=false, stop the update here

    - Check the closest obstacle, if near, state=near_obstacle and apply ObstacleLaw
    - if state!=near_obstacle, if state!=near_predator, check closest fruit (so that a fruit is detected only if no predators are around)

    - check every bird in field of view

    - if bird in separation range, if state=near_predator, change state to near_predatorANDseparation and stock bird position in SeparatePosition
    - elseif bird in separation range, if state=near_fruit, change state to near_fruitANDseparation and add bird position in SeparatePosition
    - else bird in separation range, state=separation and stock bird position in SeparatePosition

    - elseif bird in aligment range and (state=aligment or state=constant), change state to aligment and add bird angle to aligment angle

    - elseif bird in cohesion range and (state=cohesion or state=constant), change state to cohesion and add bird position in CohesionPosition

    - switch depending of the state
```
state = near_predatorANDseparation : biSeparationLaw
state = near_predator : separationLaw
state = near_fruitANDseparation : biFruitLaw
state = near_fruit : fruitLaw
state = separation : separationLaw
state = alignment : alignmentLaw
state = cohesion : cohesionLaw
```

3. For Trees

    - Execute DropFruitAndAppend
    - DropFruitAndAppend : if current time > tree time, create a random number of fruit object in cercle around the tree and emplace them at the back of fruits vector.
    - Randomize next drop time

4. For Fruits

    - if alive, push at the back of newFruits vector
    - else, create new bird object at position of the fruit

5. Return the newly created fruits

![](extras/rules.png)

 
## Building the project executable

Download the repository from the main branch. Assuming you are now inside the project's root directory on your computer, you can do the following commands to build the project executable depending on your OS.

### Windows
Create /build directory, build the executable and pass the tests:
```
cd scripts/Windows
bash build.sh
bash test.sh
```

Go to the executable location and launch it:
```
cd ../../build/src
bash life-of-boids
./life-of-boids
```

### MacOS
Create /build directory, build the executable and pass the tests:
```
cd scripts/MacOS
chmod +x build.sh
./build.sh
chmod +x test.sh
./test.sh
```

Go to the executable location and launch it:
```
cd ../../build/src
chmod +x life-of-boids
./life-of-boids
```

## Setting up the project

### Visual Studion 2019
- At the center top, you'll find a search box. There type, CMake Settings.
- Set the configuration to `Release` mode
- Change the CMake toolchain file to point to `build/conon_paths.cmake`
- Save the changes.
- Choose the .exe as the item of execution with green play button at the center top.
- Et Voila!

### CLion (MacOS)
- Open the project in CLion
- Let the default parameters of the Open Project Wizard window and click ```Ok```
- Chose "Life-of-boids" as the target for Run at the center right.
- Et voila!


**Note**: Make sure to update .gitignore file to avoid pushing large or unecessary files on the repository.

## About running the project
1. The parameters of the project are defined in ```config/config.cpp``` and can be changed by the user.

The 3 main parameters that the user can change are:
- The number of birds const ```DEFAULT_NUM_BIRDS```
- The number of predators ```DEFAULT_NUM_PREDATORS```
- The number of obstacles ```DEFAULT_NUM_OBSTACLES```

Most of the constants defined are declared as `Real` which is a `float`.

2. Compile & Build the Project using CMake and execute `build/src/life-of-boids.exe` 
