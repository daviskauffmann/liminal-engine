cmake_minimum_required(VERSION 3.0.0)

project(liminal)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(minecraft)
add_subdirectory(runtime)
