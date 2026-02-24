cmake_minimum_required(VERSION 3.16)
project(chesslib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
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
      -ftemplate-backtrace-limit=0 -static
    )
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fconstexpr-ops-limit=2000000000 -fconstexpr-depth=1024 -march=native -mtune=native -ftemplate-backtrace-limit=0 -static)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(ARCH_FLAG "/arch:AVX2" CACHE STRING "MSVC architecture flag (/arch:SSE2, /arch:AVX, /arch:AVX2, /arch:AVX512)")
  add_compile_options(/constexpr:steps2000000000 /constexpr:depth1024 ${ARCH_FLAG})
endif()

if(ENABLE_COVERAGE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
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
add_executable(NonImportantTests
    non_core_tests.cpp
)
target_link_libraries(test_chess PRIVATE chesslib)
target_include_directories(test_chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${doctest_SOURCE_DIR})
target_link_libraries(NonImportantTests PRIVATE chesslib)
target_include_directories(NonImportantTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${doctest_SOURCE_DIR})
add_test(NAME test_core COMMAND test_chess)
add_test(NAME api_tests COMMAND NonImportantTests)
if(ENABLE_COVERAGE)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        foreach(t chesslib test_chess NonImportantTests)
            target_compile_options(${t} PRIVATE
                --coverage
                -O0
            )
            target_link_options(${t} PRIVATE
                --coverage
            )
        endforeach()

        find_program(GCOVR gcovr REQUIRED)

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage
            COMMAND ${CMAKE_CTEST_COMMAND}
            COMMAND ${GCOVR}
                -r ${CMAKE_SOURCE_DIR}
                --html
                --html-details
                -o coverage/index.html
                --exclude-directories tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating GCC coverage report"
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        foreach(t chesslib test_chess NonImportantTests)
            target_compile_options(${t} PRIVATE
                -fprofile-instr-generate
                -fcoverage-mapping
                -O0
            )
            target_link_options(${t} PRIVATE
                -fprofile-instr-generate
            )
        endforeach()

        if(WIN32)

            find_program(LLVM_PROFDATA llvm-profdata REQUIRED)
            find_program(LLVM_COV llvm-cov REQUIRED)

            add_custom_target(coverage
                COMMAND ${CMAKE_COMMAND} -E make_directory coverage

                COMMAND ${CMAKE_COMMAND} -E env
                    LLVM_PROFILE_FILE=coverage/coverage.profraw
                    CTEST_PARALLEL_LEVEL=1
                    ${CMAKE_CTEST_COMMAND}

                COMMAND ${LLVM_PROFDATA} merge
                    -sparse
                    coverage/coverage.profraw
                    -o coverage/coverage.profdata

                COMMAND ${LLVM_COV} show
                    $<TARGET_FILE:test_chess>
                    $<TARGET_FILE:NonImportantTests>
                    -instr-profile=coverage/coverage.profdata
                    -format=html
                    -output-dir=coverage
                    -ignore-filename-regex="tests|doctest"
                    -Xdemangler=llvm-cxxfilt

                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating Clang coverage report (Windows)"
            )
        else()

            find_program(LLVM_PROFDATA llvm-profdata REQUIRED)
            find_program(LLVM_COV llvm-cov REQUIRED)

            add_custom_target(coverage
                COMMAND ${CMAKE_COMMAND} -E make_directory coverage

                COMMAND ${CMAKE_COMMAND} -E env
                    LLVM_PROFILE_FILE=coverage/%m.profraw
                    ${CMAKE_CTEST_COMMAND}

                COMMAND ${LLVM_PROFDATA} merge
                    -sparse
                    coverage
                    -o coverage/coverage.profdata

                COMMAND ${LLVM_COV} show
                    $<TARGET_FILE:test_chess>
                    $<TARGET_FILE:NonImportantTests>
                    -instr-profile=coverage/coverage.profdata
                    -format=html
                    -output-dir=coverage
                    -ignore-filename-regex="tests|doctest"
                    -Xdemangler=c++filt

                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating Clang coverage report"
            )
        endif()
    endif()

endif()