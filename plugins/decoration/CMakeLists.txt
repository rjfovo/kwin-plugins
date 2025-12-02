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
         DESTINATION ${QT_PLUGINS_DIR}/org.kde.kdecoration2)
