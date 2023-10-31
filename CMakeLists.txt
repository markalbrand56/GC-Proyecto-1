cmake_minimum_required(VERSION 3.26)
project(GC_Proyecto_1)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_INCLUDE_DIR C:/Users/marka/Coding/Recursos/Librerías/SDL2/include)
set(SDL2_LIB_DIR C:/Users/marka/Coding/Recursos/Librerías/SDL2/lib/x64)

find_package(glm CONFIG REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(GC_Proyecto_1 src/main.cpp)

target_link_libraries(GC_Proyecto_1 SDL2main SDL2 glm::glm)