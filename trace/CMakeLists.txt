cmake_minimum_required(VERSION 3.13.4)
project(trace-info)

#===============================================================================
# 1. LOAD LLVM CONFIGURATION
#===============================================================================

set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")

find_package(LLVM 14 REQUIRED CONFIG)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

#===============================================================================
# 2. LLVM-TUTOR BUILD CONFIGURATION
#===============================================================================

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

#===============================================================================
# 3. ADD THE TARGET
#===============================================================================
add_library(TraceInfo SHARED TraceInfo.cpp)

# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(TraceInfo
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
