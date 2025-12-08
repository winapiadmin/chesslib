cmake_minimum_required(VERSION 3.16)
project(chesslib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Compiler tuning ---
set(SAN_ENABLED OFF CACHE BOOL "Whether to enable ASan or not")
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
  set(ARCH_FLAG "/arch:AVX2" CACHE STRING "MSVC architecture flag (/arch:SSE2, /arch:AVX, /arch:AVX2, /arch:AVX512)")
  add_compile_options(/constexpr:steps2000000000 /constexpr:depth1024 ${ARCH_FLAG})
endif()

add_compile_definitions(GENERATE_AT_RUNTIME)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_compile_definitions(_DEBUG)
endif()
# --- Core Library ---
set(SOURCES
    position.cpp
    attacks.cpp "zobrist.cpp"
 "moves_io.cpp" "printers.cpp" "movegen.cpp")
add_library(chesslib STATIC ${SOURCES})
target_include_directories(chesslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
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