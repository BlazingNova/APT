cmake_minimum_required(VERSION 2.8)
project(sim)

set(CMAKE_CXX_STANDARD 11)

add_executable(sim
        src/AES.cpp
        src/cache.cpp
        src/cache.h
        src/core.cpp
        src/core.h
        src/dram.cpp
        src/dram.h
        src/memsys.cpp
        src/memsys.h
        src/sim.cpp
        src/types.h)
