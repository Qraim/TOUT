cmake_minimum_required(VERSION 3.25)
project(Tout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Tout main.cpp bdd.cpp bdd.h tubesimple.cpp tubesimple.h tout.cpp tout.h tableau.h tableau.cpp pcdim.cpp pcdim.h gag.cpp gag.h MainWidow.cpp MainWidow.h)
target_link_libraries(Tout
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

