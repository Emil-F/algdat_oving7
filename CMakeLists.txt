cmake_minimum_required(VERSION 4.2.2)
project(oving7)

set(CMAKE_CXX_STANDARD 26)

find_package(Qt6 REQUIRED COMPONENTS Widgets QuickWidgets Location Positioning)

add_executable(oving7 main.cpp
        main.cpp
        init.h
        init.cpp
        algorithm.h
        algorithm.cpp
        GeoMap.cpp
        GeoMap.h
)

target_link_libraries(oving7 PRIVATE
        Qt6::Widgets
        Qt6::QuickWidgets
        Qt6::Location
        Qt6::Positioning
)

qt_add_resources(oving7 map_resources
        PREFIX "/"
        FILES MapView.qml
)