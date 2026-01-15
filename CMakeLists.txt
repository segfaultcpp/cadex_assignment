cmake_minimum_required(VERSION 4.0)
project(cadex)
file(GLOB_RECURSE CADEX_SRCS "./src/*.cpp")
file(GLOB_RECURSE CADEX_HDRS "./src/*.hpp")
add_executable(cadex ${CADEX_SRCS} ${CADEX_HDRS})
set_target_properties(cadex
                  PROPERTIES
                    CXX_STANDARD 23
                    CXX_STANDARD_REQUIRED ON)

option(USE_F64 "Use double precision floating points for computations" OFF)

if (${USE_F64})
  target_compile_options(cadex PRIVATE -DUSE_F64)
endif()
