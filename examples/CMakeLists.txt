cmake_minimum_required(VERSION 3.10)

option(ADD_EXAMPLES "Creates examples" OFF)

if(ADD_EXAMPLES)
    add_subdirectory(sevseg/m74hc595/single/onecolor)
    add_subdirectory(sevseg/m74hc595/single/bicolor)
    add_subdirectory(sevseg/m74hc595/multi/onecolor)
endif()
