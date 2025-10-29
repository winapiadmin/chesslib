cmake_minimum_required(VERSION 3.16)
project(chesslib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Compiler tuning ---
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # clang-cl: forward GCC/Clang style constexpr flags via /clang:
    add_compile_options(
      /clang:-fconstexpr-steps=2000000000
      /clang:-fconstexpr-depth=1024
      /clang:-march=native
      /clang:-mtune=native
      /clang:-ftemplate-backtrace-limit=0
    )
  else()
    # native clang++ on *nix or Windows
    add_compile_options(
      -fconstexpr-steps=2000000000
      -fconstexpr-depth=1024
      -march=native -mtune=native
      -ftemplate-backtrace-limit=0
    )
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fconstexpr-ops-limit=2000000000 -fconstexpr-depth=1024 -march=native -mtune=native -ftemplate-backtrace-limit=0)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/constexpr:steps2000000000 /constexpr:depth1024 /arch:AVX) # I'll assume everybody has this since this is 2025.
endif()

add_compile_definitions(GENERATE_AT_RUNTIME)
# --- Core Library ---
set(SOURCES
    position.cpp
    attacks.cpp
    zobrist.h
)
add_library(chesslib STATIC ${SOURCES})
target_include_directories(chesslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(perft_tt examples/perft_with_tt.cpp)
target_link_libraries(perft_tt PRIVATE chesslib)
# --- Enable CTest integration ---
enable_testing()
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)
# --- Test executable ---
add_executable(test_chess
    tests.cpp
)
target_link_libraries(test_chess PRIVATE chesslib)
target_include_directories(test_chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${doctest_SOURCE_DIR})
add_test(NAME tests COMMAND test_chess)