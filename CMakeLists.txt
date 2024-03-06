# Build for Decompressor program

cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(Decompressor)

# decompressor
add_executable(decompressor)
target_sources(decompressor PRIVATE decompressor.cpp)
target_compile_features(decompressor PRIVATE cxx_std_17)
target_compile_options(decompressor PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /Wextra /O3>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -O3>
)

# libarchive
find_package(LibArchive REQUIRED)
target_link_libraries(decompressor PRIVATE LibArchive::LibArchive)

# run
add_custom_target(run
    COMMENT "Decompressor"
    COMMAND $<TARGET_FILE:decompressor> < ${CMAKE_CURRENT_SOURCE_DIR}/poem.tar.gz
    DEPENDS decompressor
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# check
find_program(DIFF NAMES diff)
add_custom_target(check
    COMMENT "Decompressor Check"
    COMMAND $<TARGET_FILE:decompressor> < ${CMAKE_CURRENT_SOURCE_DIR}/poem.tar.gz | ${DIFF} ${CMAKE_CURRENT_SOURCE_DIR}/poem.txt -
    DEPENDS decompressor
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# install
install(PROGRAMS $<TARGET_FILE:decompressor> TYPE BIN)
