cmake_minimum_required(VERSION 3.16)
project(MultiTypeChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enable warnings and treat them as errors
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fconstexpr-steps=2000000000 -fconstexpr-depth=1024)
elseif (CMAKE_CXX_COMPILE_ID STREQUAL "GCC")
    add_compile_options(-fconstexpr-ops-limit=2000000000 -fconstexpr-depth=1024)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/constexpr:steps2000000000 /constexpr:depth1024)
endif()
# Enable AddressSanitizer when in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling AddressSanitizer")
    set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

# Main library sources
set(SOURCES
    position.cpp
)

add_library(MultiTypeChess STATIC ${SOURCES})
target_include_directories(MultiTypeChess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Catch2 test executable
add_executable(test_chess
    tests/hello_catch2.test.cpp
    tests/catch_amalgamated.cpp
    position.cpp
)
target_link_libraries(test_chess PRIVATE MultiTypeChess)

enable_testing()
add_test(NAME MultiTypeChessTests COMMAND test_chess)