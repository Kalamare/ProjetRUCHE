cmake_minimum_required(VERSION 3.27)
project(Projet C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Projet
        world.h
        world.c
        hive.h
        hive.c
        main.c
        list.h
        list.c
        randomwalker.h
        randomwalker.c
        stack.h
        stack.c
        location.h
        location.c)
