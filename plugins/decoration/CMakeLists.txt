project (cutefishdecoration)
set(CMAKE_CXX_STANDARD 11)

find_package(KF6CoreAddons REQUIRED)
find_package(KF6Config REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(KDecoration3 REQUIRED)
find_package(Qt6 CONFIG REQUIRED COMPONENTS Gui Widgets Core)

set (decoration_SRCS
    decoration.cpp
    x11shadow.cpp
    button.cpp
    resources.qrc
)

add_library (cutefishdecoration MODULE
    ${decoration_SRCS}
)

# KWin6/KF6 按"插件文件名"推导插件 Id（KPluginMetaData::pluginId()）。
# 文件名必须与 cutefishos.json 里的 Id(org.cutefish.decoration) 一致，
# 否则 KWin6 的 KPluginMetaData::findPluginById("org.kde.kdecoration3", ...) 永远找不到。
# 用 PREFIX "" 去掉 CMake 默认的 lib 前缀，产物名为 org.cutefish.decoration.so
# （与系统自带的 org.kde.breeze.so / org.kde.kwin.aurorae.so 命名一致）。
set_target_properties(cutefishdecoration PROPERTIES
    OUTPUT_NAME "org.cutefish.decoration"
    PREFIX ""
)

target_link_libraries (cutefishdecoration
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::CoreAddons
        KF6::WindowSystem

    PRIVATE
        KDecoration3::KDecoration
        Qt6::CorePrivate
        Qt6::GuiPrivate
)

install (TARGETS cutefishdecoration
         DESTINATION ${QT_PLUGINS_DIR}/org.kde.kdecoration3)
