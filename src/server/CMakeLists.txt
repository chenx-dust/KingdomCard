set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_LIST basic/Player.cpp
        basic/GameController.cpp
        basic/CardEnum.cpp
        cards/Attack.cpp
        cards/Heal.cpp
        cards/Dodge.cpp )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(server ${SRC_LIST} main.cpp)
add_executable(server_test ${SRC_LIST} testbench.cpp)
