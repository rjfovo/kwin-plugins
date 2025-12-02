find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6WindowSystem REQUIRED)

# 获取qmake
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
if(NOT QT_QMAKE_EXECUTABLE)
    message(FATAL_ERROR "qmake is not found.")
endif()

# execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(QT_PLUGINS_DIR)
    message(STATUS "Qt6 plugin directory:" "${QT_PLUGINS_DIR}")
else()
    message(FATAL_ERROR "Qt6 plugin directory cannot be detected.")
endif()

# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

# add_subdirectory(blur)
add_subdirectory(decoration)

# Check if we have the required libraries for roundedwindow
find_path(EFFECTS_H kwineffects.h PATH_SUFFIXES kf6)
find_library(KWIN_EFFECTS NAMES kwineffects PATH_SUFFIXES kf6)
find_library(KWIN_GLUTILS NAMES kwinglutils PATH_SUFFIXES kf6)
find_library(OPENGL NAMES GL)

if (EFFECTS_H AND KWIN_EFFECTS AND KWIN_GLUTILS AND OPENGL)
    message(STATUS "Found KWin effects libraries, building roundedwindow plugin")
    add_subdirectory(roundedwindow)
else()
    message(STATUS "KWin effects libraries not found, skipping roundedwindow plugin")
endif()
