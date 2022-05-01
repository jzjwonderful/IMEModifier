cmake_minimum_required(VERSION 3.10)

# Remove when sharing with others.
list(APPEND CMAKE_PREFIX_PATH "D:/Qt-Creator/build/Desktop_Qt_6_2_3_MSVC2019_64bit-Debug")

project(InputMethodModifier)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QtCreator REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
set(QtX Qt${QT_VERSION_MAJOR})

add_qtc_plugin(InputMethodModifier
  PLUGIN_DEPENDS
    QtCreator::Core QtCreator::TextEditor
  DEPENDS
    ${QtX}::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    .github/workflows/build_cmake.yml
    .github/workflows/README.md
    README.md
    inputmethodmodifier.cpp
    inputmethodmodifier.h
    inputmethodmodifier_global.h
    inputmethodmodifierconstants.h
)
