cmake_minimum_required(VERSION 3.6)
project(schaken)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

#=====================================================
# TODO: Change directory QT for windows/mac/linux
# Uncomment correct directory, and comment other directories
#=====================================================

#=====================================================
#Linux (tested in classroom):
#=====================================================
#set(Qt5Core_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5Core")
#set(Qt5Widgets_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5Widgets")
#set(Qt5Gui_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5Gui")

#For other linux variants locate local installation directory Qt5; e.g.:
#=====================================================
set(Qt5Core_DIR "/home/joachimverleysen/QT/5.15.2/gcc_64/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "/home/joachimverleysen/QT/5.15.2/gcc_64/lib/cmake/Qt5Widgets")
set(Qt5Gui_DIR "/home/joachimverleysen/QT/5.15.2/gcc_64/lib/cmake/Qt5Gui")


#=====================================================
# Mac
#=====================================================
#set(Qt5Core_DIR "~/Qt/5.9.2/clang_64/lib/cmake/Qt5Core")
#set(Qt5Widgets_DIR "~/Qt/5.9.2/clang_64/lib/cmake/Qt5Widgets")
#set(Qt5Gui_DIR "~/Qt/5.9.2/clang_64/lib/cmake/Qt5Gui")

#=====================================================
# Windows
#=====================================================
#set(Qt5Core_DIR "C:\\Qt\\5.9.3\\mingw53_32\\lib\\cmake\\Qt5Core")
#set(Qt5Widgets_DIR "C:\\Qt\\5.9.3\\mingw53_32\\lib\\cmake\\Qt5Widgets")
#set(Qt5Gui_DIR "C:\\Qt\\5.9.3\\mingw53_32\\lib\\cmake\\Qt5Gui")


find_package( Qt5Gui REQUIRED )
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)


set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp guicode/ChessWindow.cpp guicode/ChessWindow.h guicode/chessboard.cpp guicode/chessboard.h src/game.cpp src/game.h src/SchaakStuk.cpp src/SchaakStuk.h guicode/resources.h src/SchaakGUI.cpp src/SchaakGUI.h guicode/message.cpp guicode/message.h guicode/fileIO.cpp guicode/fileIO.h )
add_executable(schaken ${SOURCE_FILES})

#qt5_use_modules(schaken Core Widgets Gui)
target_link_libraries(schaken Qt5::Core Qt5::Widgets Qt5::Gui)

#include_directories(${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_BUILD_TYPE Debug)
