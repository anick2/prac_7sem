cmake_minimum_required(VERSION 3.17)
project(myproject)

set(CMAKE_CXX_STANDARD 14)

add_executable(myproject main.cpp temperature.cpp temperature.h mutation.cpp mutation.h solution.cpp solution.h simulated_annealing.h sockets.cpp sockets.h)