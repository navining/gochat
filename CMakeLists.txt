cmake_minimum_required(VERSION 3.0)
project(main)

# Compile arguments
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# Executable files output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Header file include path
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/database)
include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# Add subdirectory
add_subdirectory(src)



