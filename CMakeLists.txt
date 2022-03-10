cmake_minimum_required (VERSION 2.6)

project (vedirect-hex-parser)

include_directories(main)
add_library(vedirect-hex-parser SHARED main/vhp_parser.cpp main/vhp_registers.cpp main/vhp_command.cpp)
add_executable(sampleMain vhp_main.cpp)
target_link_libraries(sampleMain vedirect-hex-parser)
