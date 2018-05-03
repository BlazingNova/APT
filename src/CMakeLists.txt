cmake_minimum_required(VERSION 2.8)
project(sim)

set(CMAKE_CXX_STANDARD 11)

add_executable(sim
        AES.cpp
        cache.cpp
        cache.h
        core.cpp
        core.h
        dram.cpp
        dram.h
        memsys.cpp
        memsys.h
        sim.cpp
        types.h)
