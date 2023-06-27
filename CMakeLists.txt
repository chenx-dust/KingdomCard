cmake_minimum_required(VERSION 3.25)
project(KingdomCard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/libzmq)
add_subdirectory(thirdparty/cppzmq)
add_subdirectory(thirdparty/protobuf)
add_subdirectory(thirdparty/spdlog)

add_subdirectory(message)
add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(src/test_client)
