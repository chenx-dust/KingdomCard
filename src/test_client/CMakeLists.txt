file(GLOB_RECURSE SRC_LIST "*.cpp")
message(STATUS "SRC_LIST: ${SRC_LIST}")

add_executable(kc_test_client ${SRC_LIST})

target_include_directories(kc_test_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(kc_test_client PUBLIC ${PROTO_BINARY_DIR})
target_include_directories(kc_test_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/cppzmq/)
target_include_directories(kc_test_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/protobuf/src/)
target_include_directories(kc_test_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)

target_link_libraries(kc_test_client PRIVATE
        proto-objects
        cppzmq-static
        spdlog::spdlog)
