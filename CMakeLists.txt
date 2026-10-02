#
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

# --- Default build type ---
# Without this an unconfigured build (`cmake -S . -B build`) gets no -O flag AND no
# NDEBUG. That is not merely slow: position.cpp and movegen.cpp gate diagnostics on
# `#if defined(_DEBUG) || !defined(NDEBUG)`, so an unconfigured build compiles in a
# std::cerr dump of the whole board and the whole move history on every king capture.
# A single-config generator with no CMAKE_BUILD_TYPE is a near-certain misconfiguration,
# so default it to RelWithDebInfo rather than silently producing a -O0 build.
get_property(_chesslib_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT _chesslib_is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
    message(STATUS "CMAKE_BUILD_TYPE was unset; defaulting to RelWithDebInfo")
endif()

# --- Link-Time Optimization ---
option(ENABLE_LTO "Enable Link-Time Optimization" ON)

if(ENABLE_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# --- Core Library ---
set(SOURCES
    position.cpp
    attacks.cpp
    zobrist.cpp
    moves_io.cpp
    printers.cpp
    movegen.cpp
)

add_library(chesslib STATIC ${SOURCES})

target_include_directories(chesslib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Error handling ---
set(CHESSLIB_ERROR_MODE "THROW" CACHE STRING
    "Error handling mode: ASSERT, THROW, or NONE"
)

set_property(CACHE CHESSLIB_ERROR_MODE PROPERTY STRINGS
    ASSERT
    THROW
    NONE
)

if(NOT CHESSLIB_ERROR_MODE MATCHES "^(ASSERT|THROW|NONE)$")
    message(FATAL_ERROR
        "Invalid CHESSLIB_ERROR_MODE='${CHESSLIB_ERROR_MODE}'. "
        "Expected ASSERT, THROW, or NONE."
    )
endif()

# --- Compiler tuning ---
#
# Native architecture detection deliberately performs a compile+run test.
# This avoids blindly trusting -march=native when running inside a VM,
# container, CI environment, or other setup where CPUID may advertise
# instructions that user-space cannot actually execute.
#
# Override with:
#
#   -DCHESSLIB_NATIVE_ARCH_RUNS=ON
#   -DCHESSLIB_NATIVE_ARCH_RUNS=OFF
#
# AUTO performs the compile+run test.
#

set(CHESSLIB_NATIVE_ARCH_RUNS "AUTO" CACHE STRING
    "Native architecture probe: AUTO, ON, or OFF"
)

set_property(CACHE CHESSLIB_NATIVE_ARCH_RUNS PROPERTY STRINGS
    AUTO
    ON
    OFF
)

if(NOT CHESSLIB_NATIVE_ARCH_RUNS MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR
        "Invalid CHESSLIB_NATIVE_ARCH_RUNS='${CHESSLIB_NATIVE_ARCH_RUNS}'. "
        "Expected AUTO, ON, or OFF."
    )
endif()

include(CheckCXXSourceRuns)
include(CheckCXXCompilerFlag)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
    CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))

    set(_chesslib_native_flags
        /arch:AVX512
    )

    set(_chesslib_fallback_flags
        /arch:AVX2
    )

else()

    set(_chesslib_native_flags
        -march=native
        -mtune=native
    )

    set(_chesslib_fallback_flags
        -march=x86-64-v3
        -mtune=native
    )

endif()

# Convert CMake lists to human-readable compiler flag strings.
string(REPLACE ";" " "
    _chesslib_native_flags_str
    "${_chesslib_native_flags}"
)

string(REPLACE ";" " "
    _chesslib_fallback_flags_str
    "${_chesslib_fallback_flags}"
)

# --- Architecture detection ---

if(CHESSLIB_NATIVE_ARCH_RUNS STREQUAL "AUTO")

    message(STATUS
        "chesslib: probing native architecture support: "
        "${_chesslib_native_flags_str}"
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU|MSVC)$")

        set(CMAKE_REQUIRED_FLAGS
            "${_chesslib_native_flags_str}"
        )

        check_cxx_source_runs("
            #include <immintrin.h>

            int main()
            {
                __m512i a = _mm512_set1_epi32(1);
                __m512i b = _mm512_add_epi32(a, a);

                if (_mm512_reduce_add_epi32(b) != 32)
                    return 1;

            #if defined(__BMI2__)
                if (_pext_u64(0xFF00ULL, 0xFF00ULL) != 0xFFULL)
                    return 1;
            #endif

                return 0;
            }
        " CHESSLIB_NATIVE_ARCH_RUNS)

        unset(CMAKE_REQUIRED_FLAGS)

    else()

        message(WARNING
            "chesslib: compiler '${CMAKE_CXX_COMPILER_ID}' is not "
            "supported by the native architecture self-test; "
            "using fallback architecture flags."
        )

        set(CHESSLIB_NATIVE_ARCH_RUNS FALSE)

    endif()

elseif(CHESSLIB_NATIVE_ARCH_RUNS STREQUAL "ON")

    message(STATUS
        "chesslib: native architecture detection forced ON by user"
    )

    set(CHESSLIB_NATIVE_ARCH_RUNS TRUE)

else()

    message(STATUS
        "chesslib: native architecture detection forced OFF by user"
    )

    set(CHESSLIB_NATIVE_ARCH_RUNS FALSE)

endif()

# --- Select architecture flags ---

if(CHESSLIB_NATIVE_ARCH_RUNS)

    target_compile_options(chesslib PUBLIC
        ${_chesslib_native_flags}
    )

    message(STATUS
        "chesslib: using native architecture flags: "
        "${_chesslib_native_flags_str}"
    )

else()

    message(WARNING
        "chesslib: '${_chesslib_native_flags_str}' failed a compile+run "
        "self-test on this build machine (crashed, or the probe could not "
        "be executed at all, e.g. cross-compiling) -- falling back to "
        "'${_chesslib_fallback_flags_str}'."
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR
       (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
        CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))

        target_compile_options(chesslib PUBLIC
            ${_chesslib_fallback_flags}
        )

        message(STATUS
            "chesslib: using fallback architecture flags: "
            "${_chesslib_fallback_flags_str}"
        )

    else()

        check_cxx_compiler_flag(
            "-march=x86-64-v3"
            CHESSLIB_HAS_X86_64_V3
        )

        if(CHESSLIB_HAS_X86_64_V3)

            target_compile_options(chesslib PUBLIC
                ${_chesslib_fallback_flags}
            )

            message(STATUS
                "chesslib: x86-64-v3 supported; using fallback flags: "
                "${_chesslib_fallback_flags_str}"
            )

        else()

            message(WARNING
                "chesslib: x86-64-v3 is not recognized by this compiler; "
                "building with compiler-default architecture flags."
            )

        endif()

    endif()

endif()

# --- Public error mode ---
target_compile_definitions(chesslib PUBLIC
    _CHESSLIB_ERROR_MODE_${CHESSLIB_ERROR_MODE}
)

# --- Release-style configurations ---
# PUBLIC, not INTERFACE: chesslib's own translation units need NDEBUG too, otherwise
# the `#if defined(_DEBUG) || !defined(NDEBUG)` diagnostics in position.cpp and
# movegen.cpp stay compiled in. Relying on CMAKE_CXX_FLAGS_<CONFIG> alone only works
# for CMake's built-in config names.
target_compile_definitions(chesslib PUBLIC
    "$<$<CONFIG:Release>:NDEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:NDEBUG>"
    "$<$<CONFIG:MinSizeRel>:NDEBUG>"
)

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

    # --- Test executables ---
    add_executable(test_normal
        tests.cpp
    )

    add_executable(test_chess960
        chess960_tests.cpp
    )

    add_executable(NonImportantTests
        non_core_tests.cpp
    )

    add_executable(perft_example
        perft_example.cpp
    )

    target_link_libraries(perft_example PRIVATE
        chesslib
    )

    target_link_libraries(test_normal PRIVATE
        chesslib
        doctest::doctest
    )

    target_link_libraries(NonImportantTests PRIVATE
        chesslib
        doctest::doctest
    )

    target_link_libraries(test_chess960 PRIVATE
        chesslib
        doctest::doctest
    )

    # chess960_tests.cpp is a single ~5800-line TEST_CASE with thousands
    # of sequential perft assertions. GCC's/Clang's per-function optimizer
    # passes scale badly with function size. Optimizing this one TU
    # (verified at -Og/-O1/-O2/-O3, with and without -fno-inline) pushes
    # cc1plus past 4 GB RSS and gets OOM-killed on memory-constrained
    # build machines/CI runners. Only -O0 stayed under that on a 3.9 GB
    # test host.
    #
    # This file only needs to run correctly, not quickly, so pin it to
    # -O0 and skip LTO for it alone. Every other target keeps full
    # optimization.

    set_source_files_properties(
        chess960_tests.cpp
        PROPERTIES
            COMPILE_OPTIONS "-O0"
    )

    set_target_properties(
        test_chess960
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION FALSE
    )

    add_test(
        NAME test_normal
        COMMAND test_normal --abort-after=1
    )

    add_test(
        NAME test_api
        COMMAND NonImportantTests --abort-after=1
    )

    add_test(
        NAME test_chess960
        COMMAND test_chess960 --abort-after=1
    )

    # --- Sanitizers ---
    if(UNIX AND CMAKE_BUILD_TYPE MATCHES "Debug")

        set(SANITIZERS "" CACHE STRING
            "Sanitizers, e.g. undefined,address"
        )

        if(NOT "${SANITIZERS}" STREQUAL "")

            add_compile_options(
                -fsanitize=${SANITIZERS}
                -fno-omit-frame-pointer
            )

            add_link_options(
                -fsanitize=${SANITIZERS}
            )

        endif()

    endif()

endif()
