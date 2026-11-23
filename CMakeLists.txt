
cmake_minimum_required(VERSION 3.16)

project(EdgeOS VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core library
add_library(edgeos_core STATIC
    src/AVL.cpp
    src/BinaryHeap.cpp
    src/CommandProcessor.cpp
    src/FileStorager.cpp
    src/HashTable.cpp
    src/HistoryStack.cpp
    src/Scheduler.cpp
    src/Task.cpp

    include/AVL.hpp
    include/BinaryHeap.hpp
    include/CommandProcessor.hpp
    include/FileStorager.hpp
    include/HashTable.hpp
    include/HistoryStack.hpp
    include/Scheduler.hpp
    include/Task.hpp
)

target_include_directories(edgeos_core PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

# Main executable
add_executable(EdgeOS
    src/main.cpp
)

target_link_libraries(EdgeOS PRIVATE edgeos_core)

set_target_properties(EdgeOS PROPERTIES
    OUTPUT_NAME edgeos
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(edgeos_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(EdgeOS PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

