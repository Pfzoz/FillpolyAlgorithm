cmake_minimum_required(VERSION 3.7...3.28)

project(
    Fillpoly 
    VERSION 1.0 
    LANGUAGES CXX)


add_executable(fillpoly src/fillpoly.cc)

target_link_directories(fillpoly sdl_scenes)