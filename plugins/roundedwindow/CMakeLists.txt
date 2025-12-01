find_package(KF6CoreAddons)
find_package(KF6Config)
find_package(KF6WindowSystem)

find_path(EFFECTS_H kwineffects.h PATH_SUFFIXES kf6)

if (EFFECTS_H)
    include_directories(${EFFECTS_H})
else (EFFECTS_H)
    message(STATUS "didnt find kwineffects.h, not building effects")
endif (EFFECTS_H)

find_library(KWIN_EFFECTS NAMES kwineffects PATH_SUFFIXES kf6)
find_library(KWIN_GLUTILS NAMES kwinglutils PATH_SUFFIXES kf6)
find_library(OPENGL NAMES GL)

if (NOT KWIN_EFFECTS)
    message(STATUS "didnt find kwineffects lib, not building effects")
endif (NOT KWIN_EFFECTS)

if (NOT KWIN_GLUTILS)
    message(STATUS "didnt find kwin glutils lib, not building effects")
endif (NOT KWIN_GLUTILS)

if (NOT OPENGL)
    message(STATUS "didnt find opengl, not building effects")
endif (NOT OPENGL)

if (NOT EFFECTS_H OR NOT KWIN_GLUTILS OR NOT KWIN_EFFECTS OR NOT OPENGL)
    message(FATAL_ERROR "cant continue")
endif (NOT EFFECTS_H OR NOT KWIN_GLUTILS OR NOT KWIN_EFFECTS OR NOT OPENGL)

add_library(roundedwindow MODULE
    main.cpp
    roundedwindow.cpp
    resources.qrc
)

target_link_libraries(roundedwindow
    PUBLIC
        Qt6::Core
        Qt6::Gui
    PRIVATE
        KF6::CoreAddons
        KF6::ConfigCore
        KF6::WindowSystem
)

install (TARGETS roundedwindow DESTINATION ${QT_PLUGINS_DIR}/kwin/effects/plugins)
