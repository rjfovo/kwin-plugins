cmake_minimum_required(VERSION 3.16)
project(cutefish_roundeffect LANGUAGES CXX)

# 手动链接方案：绕开 Qt6/KWin CMake find 在部分 Debian 环境下的 versionless target 冲突。
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_EXECUTABLE /usr/lib/qt6/libexec/moc)

find_library(KWIN_LIB NAMES kwin REQUIRED)
find_library(KF6_COREADDONS NAMES KF6CoreAddons REQUIRED)
find_library(KF6_CONFIG NAMES KF6ConfigCore REQUIRED)
find_library(KF6_WINDOWSYSTEM NAMES KF6WindowSystem REQUIRED)
find_library(KF6_GUIADDONS NAMES KF6GuiAddons REQUIRED)
find_library(QT6_CORE NAMES Qt6Core REQUIRED)
find_library(QT6_GUI NAMES Qt6Gui REQUIRED)
find_library(QT6_QUICK NAMES Qt6Quick REQUIRED)
find_library(QT6_QML NAMES Qt6Qml REQUIRED)

file(GLOB KF6_INC /usr/include/KF6/*)
file(GLOB QT6_MODULE_INC /usr/include/x86_64-linux-gnu/qt6/Qt*)

add_library(cutefish_roundeffect MODULE main.cpp roundeffect.cpp roundeffect.h)

target_include_directories(cutefish_roundeffect PRIVATE
    /usr/include
    /usr/include/kwin
    /usr/include/x86_64-linux-gnu/qt6
    ${KF6_INC}
    ${QT6_MODULE_INC}
)

target_link_libraries(cutefish_roundeffect PRIVATE
    ${KWIN_LIB}
    ${KF6_COREADDONS} ${KF6_CONFIG} ${KF6_WINDOWSYSTEM} ${KF6_GUIADDONS}
    ${QT6_CORE} ${QT6_GUI} ${QT6_QUICK} ${QT6_QML}
)

set_target_properties(cutefish_roundeffect PROPERTIES
    PREFIX ""
    OUTPUT_NAME "cutefish_roundeffect"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

install(TARGETS cutefish_roundeffect
        DESTINATION lib/x86_64-linux-gnu/qt6/plugins/kwin/effects/plugins)
