cmake_minimum_required(VERSION 3.0.0)
project(liminal)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(player)
