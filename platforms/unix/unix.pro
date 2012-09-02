TEMPLATE = lib
TARGET = platform_unix
CONFIG += plugin \
    qt_warn \
    debug_and_release
VPATH += ../../src/
INCLUDEPATH += ../../src/
INCLUDEPATH += gnome_menus
INCLUDEPATH += /usr/include/glib-2.2/
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/lib/glib-2.0/include
SOURCES = platform_x11_hotkey.cpp \
    platform_base_hotkey.cpp \
    platform_unix.cpp \
    platform_unix_util.cpp \
    process_menus.cpp \
    gnome_menus/menu-util.c \
    gnome_menus/menu-monitor.c \
    gnome_menus/menu-layout.c \
    gnome_menus/gmenu-tree.c \
    gnome_menus/entry-directories.c \
    gnome_menus/desktop-entries.c \
    gnome_menus/canonicalize.c \
    ../../src/plugin_interface.cpp
HEADERS = platform_base.h \
    platform_unix.h \
    platform_base_hotkey.h \
    platform_base_hottrigger.h \
    platform_unix_util.h \
    platform_x11_hotkey.h \
    process_menu.h \
    gnome_menus/menu-util.h \
    gnome_menus/menu-monitor.h \
    gnome_menus/menu-layout.h \
    gnome_menus/gmenu-tree.h \
    gnome_menus/entry-directories.h \
    gnome_menus/desktop-entries.h \
    gnome_menus/canonicalize.h
CONFIG -= embed_manifest_dll
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.2 \
    libgnomeui-2.0
LIBS += -lX11 \
    -lXext \
    -lXrender
DEFINES += SKINS_PATH=$(SKINS_PATH) \
    PLUGINS_PATH=$(PLUGINS_PATH) \
    PLATFORMS_PATH=$(PLATFORMS_PATH)
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/
