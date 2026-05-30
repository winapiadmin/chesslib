#  a chess library (bonus: you can integrate more piece types!) which 
#  supports Chess960 and is decently fast enough
#  Copyright (C) 2025-2026  winapiadmin
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
cmake_minimum_required(VERSION 3.14)
project(chesslib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Core Library ---
set(SOURCES
    position.cpp
    attacks.cpp "zobrist.cpp"
 "moves_io.cpp" "printers.cpp" "movegen.cpp")
add_library(chesslib STATIC ${SOURCES})
target_include_directories(chesslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(chesslib PRIVATE GENERATE_AT_RUNTIME)
set(CHESSLIB_ERROR_MODE "THROW" CACHE STRING "ASSERT|THROW|NONE")
set_property(CACHE CHESSLIB_ERROR_MODE PROPERTY STRINGS ASSERT THROW NONE)

if(NOT CHESSLIB_ERROR_MODE MATCHES "^(ASSERT|THROW|NONE)$")
    message(FATAL_ERROR "Invalid ERROR_MODE: ${CHESSLIB_ERROR_MODE}")
endif()
# --- Compiler tuning ---
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # clang-cl: forward GCC/Clang style constexpr flags via /clang:
    target_compile_options(chesslib PRIVATE
      /clang:-march=native
      /clang:-mtune=native
    )
  else()
    # native clang++ on *nix or Windows
    target_compile_options(chesslib PRIVATE
      -march=native -mtune=native
    )
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(chesslib PRIVATE -march=native -mtune=native)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(ARCH_FLAG "/arch:AVX2" CACHE STRING "MSVC architecture flag (/arch:SSE2, /arch:AVX, /arch:AVX2, /arch:AVX512)")
  target_compile_options(chesslib PRIVATE ${ARCH_FLAG})
endif()

target_compile_definitions(chesslib PUBLIC _CHESSLIB_ERROR_MODE_${CHESSLIB_ERROR_MODE})
# --- Enable CTest integration ---
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.5.2
    )
    FetchContent_MakeAvailable(doctest)

    # --- Test executable ---
    add_executable(test_normal tests.cpp)
    add_executable(test_chess960 chess960_tests.cpp)
    add_executable(NonImportantTests non_core_tests.cpp)

    target_link_libraries(test_normal PRIVATE chesslib doctest::doctest)
    target_link_libraries(NonImportantTests PRIVATE chesslib doctest::doctest)
    target_link_libraries(test_chess960 PRIVATE chesslib doctest::doctest)

    add_test(NAME test_normal COMMAND test_normal --abort-after=1)
    add_test(NAME test_api COMMAND NonImportantTests --abort-after=1)
    add_test(NAME test_chess960 COMMAND test_chess960 --abort-after=1)
    if (UNIX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(SANITIZERS "" CACHE STRING "sanitizers such as undefined,address")
    
        if (NOT "${SANITIZERS}" STREQUAL "")
            add_compile_options(-fsanitize=${SANITIZERS} -fno-omit-frame-pointer)
            add_link_options(-fsanitize=${SANITIZERS})
        endif()
    endif()

endif()
