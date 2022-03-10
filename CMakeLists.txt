cmake_minimum_required (VERSION 2.6)

project (vedirect-hex-parser)

add_library(vedirect-hex-parser SHARED vhp_parser.cpp vhp_registers.cpp)
add_executable(sampleMain vhp_main.cpp)
target_link_libraries(sampleMain vedirect-hex-parser)
