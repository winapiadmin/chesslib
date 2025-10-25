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
    )
  else()
    # native clang++ on *nix or Windows
    add_compile_options(
      -fconstexpr-steps=2000000000
      -fconstexpr-depth=1024
      -march=native -mtune=native
    )
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fconstexpr-ops-limit=2000000000 -fconstexpr-depth=1024 -march=native -mtune=native)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/constexpr:steps2000000000 /constexpr:depth1024 /arch:AVX) # I'll assume everybody has this since this is 2025.
endif()

# --- AddressSanitizer for Debug builds ---

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CheckCXXCompilerFlag)
    include(CheckCXXSourceCompiles)

    # ----------------------------
    # Sanitizer flags per compiler
    # ----------------------------
    if(MSVC)
        set(ASAN_FLAG "/fsanitize=address")
        set(UBSAN_FLAG "/fsanitize=undefined")
    else()
        set(ASAN_FLAG "-fsanitize=address")
        set(UBSAN_FLAG "-fsanitize=undefined")
    endif()

    # ----------------------------
    # Helper function to check a sanitizer
    # ----------------------------
    function(check_sanitizer FLAG VAR_NAME)
        check_cxx_compiler_flag(${FLAG} COMPILER_SUPPORTS_FLAG)

        if(COMPILER_SUPPORTS_FLAG)
            message(STATUS "Compiler supports ${FLAG}")

            # Compile + link test
            set(CMAKE_REQUIRED_FLAGS "${FLAG}")
            check_cxx_source_compiles("
                int main() { return 0; }
            " CAN_LINK)

            if(CAN_LINK)
                # Optional runtime check
                file(WRITE "${CMAKE_BINARY_DIR}/sanitizer_test.cpp" "int main() { return 0; }")
                try_run(RUN_RESULT COMPILE_RESULT
                    "${CMAKE_BINARY_DIR}"
                    "${CMAKE_BINARY_DIR}/sanitizer_test.cpp"
                    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=${FLAG}"
                )
                if(RUN_RESULT EQUAL 0)
                    message(STATUS "${FLAG} works at runtime")
                    set(${VAR_NAME} TRUE PARENT_SCOPE)
                else()
                    message(WARNING "${FLAG} compile/link succeeded but runtime test failed")
                    set(${VAR_NAME} FALSE PARENT_SCOPE)
                endif()
            else()
                message(WARNING "Cannot compile/link with ${FLAG}")
                set(${VAR_NAME} FALSE PARENT_SCOPE)
            endif()
        else()
            message(STATUS "Compiler does not support ${FLAG}")
            set(${VAR_NAME} FALSE PARENT_SCOPE)
        endif()
    endfunction()

    # ----------------------------
    # Check ASan
    # ----------------------------
    check_sanitizer(${ASAN_FLAG} ENABLE_ASAN)

    # ----------------------------
    # Check UBSan
    # ----------------------------
    check_sanitizer(${UBSAN_FLAG} ENABLE_UBSAN)

    # ----------------------------
    # Set CXX flags if supported
    # ----------------------------
    if(ENABLE_ASAN)
        message(STATUS "Enabling ASan for build")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAG}" CACHE STRING "CXX flags" FORCE)
    endif()

    if(ENABLE_UBSAN)
        message(STATUS "Enabling UBSan for build")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UBSAN_FLAG}" CACHE STRING "CXX flags" FORCE)
    endif()

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
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)
add_executable(speedperft perft_speed.cpp)
target_link_libraries(speedperft PRIVATE chesslib)
# --- Test executable ---
add_executable(test_chess
    tests.cpp
)
target_link_libraries(test_chess PRIVATE chesslib)
target_include_directories(test_chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${doctest_SOURCE_DIR})

# --- Enable CTest integration ---
enable_testing()
add_test(NAME tests COMMAND test_chess)