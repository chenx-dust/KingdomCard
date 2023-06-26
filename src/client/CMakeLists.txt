set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

qt_standard_project_setup()

set(SRC_LIST main.cpp
        ui/MainWindow.cpp
        ui/MainWindow.ui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(client ${SRC_LIST})

target_link_libraries(client
        PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )
