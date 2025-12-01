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
add_subdirectory(roundedwindow)
