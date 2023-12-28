cmake_minimum_required(VERSION 3.12)

project("sniperllvm")

# Set the LLVM_DIR variable to the path where LLVMConfig.cmake is located
set(LLVM_DIR "usr/lib/llvm-14/lib/cmake/llvm/LLVMConfig.cmake" CACHE PATH "Path to LLVMConfig.cmake")

# Find the LLVM package
find_package(LLVM REQUIRED CONFIG)

# Set additional flags for LLVM
llvm_map_components_to_libnames(llvm_libs core)

# Add your source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the static library
add_library(sniperllvm STATIC ${SOURCES})

# Set include directories for the library
target_include_directories(sniperllvm PUBLIC ${LLVM_INCLUDE_DIRS})

# Link the library with LLVM libraries
target_link_libraries(sniperllvm PUBLIC ${llvm_libs})

# Set C++ standard
target_compile_features(sniperllvm PUBLIC cxx_std_11)

# Optionally, you can enable optimizations
target_compile_options(sniperllvm PRIVATE -O3)

# clang++ $(llvm-config --cxxflags) $(llvm-config --ldflags --libs core) test.cpp -o test -I. -L./model/build -lsniperllvm