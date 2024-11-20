cmake_minimum_required(VERSION 3.1)
project(PM-EDAA
  VERSION 0.0.1
  DESCRIPTION "Pattern Matching: Proyecto 2 EDAA"
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("${PROJECT_SOURCE_DIR}/external/googletest/include"
  ${CMAKE_HOME_DIRECTORY}/include
  ${CMAKE_BINARY_DIR}/external/sdsl-lite/include
  ${CMAKE_BINARY_DIR}/external/sdsl-lite/external/libdivsufsort/include/
  ${CMAKE_HOME_DIRECTORY}/external/googletest/include
  ${CMAKE_HOME_DIRECTORY}/external/sdsl-lite/external/FastPFor/headers/
  ${STXXL_INCLUDE_DIRS}
)
add_subdirectory(external/sdsl-lite)

add_executable(main src/main.cpp)
target_link_libraries(main sdsl divsufsort divsufsort64)
