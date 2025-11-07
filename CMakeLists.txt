project (MOSP_SOPMOA)
cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_LINKER  "/usr/bin/ld")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DOS_MAC")
SET(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS} -g -Wall")

include_directories("inc")
file(GLOB SOURCES "src/algorithms/*.cpp" "src/problem/*.cpp" "src/utils/*.cpp")

find_package( Boost REQUIRED COMPONENTS program_options log thread filesystem system)
include_directories( ${Boost_INCLUDE_DIRS} )

message(${CMAKE_BINARY_DIR})

add_executable(main src/main.cpp ${SOURCES} ${GCL_NDT_SOURCES})
target_link_libraries(main ${Boost_LIBRARIES})

find_package(OpenMP REQUIRED)
target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)

# sudo apt install libtbb-dev
find_package(TBB REQUIRED)
target_link_libraries(main PUBLIC TBB::tbb)

