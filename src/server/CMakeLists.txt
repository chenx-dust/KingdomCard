set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_LIST main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(server ${SRC_LIST})
