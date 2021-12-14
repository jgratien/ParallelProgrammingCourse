#!/bin/bash
echo "Lauching build script"
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.build_type=Release default
sudo apt-get update
sudo apt-get -y install libgtk2.0-dev libgl1-mesa-dev libvdpau-dev libva-dev
if [ -d build ]; then rm build -r; fi
mkdir build && cd build
conan install .. --build missing
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config "Release"