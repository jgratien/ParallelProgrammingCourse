echo 'Processing update and installs'
conan profile new --detect default
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.build_type=Debug default
sudo apt update && sudo apt install -y libgtk2.0-dev libgl1-mesa-dev
echo "Building files"
if [ -d build ];
then
  rm build -r
fi
mkdir build
cd build
# conan install ..
conan install .. --build opencv --build openexr --build ade --build gtest --build libtiff --build protobuf
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config "Debug"
echo "Running tests..."
ctest
# winpty docker run -it --rm -v /$PWD:/data conanio/gcc9:latest //bin/bash
