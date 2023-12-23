cmake_minimum_required(VERSION 3.5)

project(kc_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        client/src/client.cpp
        client/include/client.h
        client/ui/ui_clientwindow.h

        server/ui_serverwindow.h
        server/server.cpp
        server/server
        server/serverwindow.ui

        client/ui/ui_card.h
        client/ui/card.ui

        client/ui/ui_startwindow.h
        client/ui/startwindow.ui

        communicator/communicator.cpp
        communicator/communicator.h

        client/ui/clientwindow.ui

        client/include/start.h
        client/src/start.cpp

        res.qrc communicator/commands.h client/include/card.h client/src/card.cpp utils/utils.cpp utils/utils.h client/include/player.h client/src/player.cpp)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kc_client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET kc_client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(kc_client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(kc_client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(kc_client PUBLIC ${PROTO_BINARY_DIR})
target_include_directories(kc_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/cppzmq/)
target_include_directories(kc_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/protobuf/src/)
target_include_directories(kc_client INTERFACE ${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)

target_link_libraries(kc_client PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        proto-objects
        cppzmq-static
        spdlog::spdlog)

set_target_properties(kc_client PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS kc_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kc_client)
endif()
