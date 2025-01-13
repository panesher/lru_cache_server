cmake_minimum_required(VERSION 3.10)

project(ServerProject)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 20)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=thread")

find_package(Boost REQUIRED COMPONENTS filesystem system)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Collect all source files (.cpp)
file(GLOB_RECURSE SOURCES "server/*.cpp")

# Collect all header files (.h)
file(GLOB_RECURSE HEADERS "server/*.h")

# Add the executable or library
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/server)
