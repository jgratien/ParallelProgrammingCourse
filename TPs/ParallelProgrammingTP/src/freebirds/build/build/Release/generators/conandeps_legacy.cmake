message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(GTest)
find_package(fmt)
find_package(glfw3)
find_package(glad)
find_package(PNG)

set(CONANDEPS_LEGACY  gtest::gtest  fmt::fmt  glfw  glad::glad  PNG::PNG )