TEMPLATE = lib
TARGET = platform_gnome
CONFIG += plugin \
    qt_warn \
    debug_and_release
VPATH += ../../src/ \
    ../unix
INCLUDEPATH += ../../src/ \
    ../unix
INCLUDEPATH += /usr/include/gtk-2.2
INCLUDEPATH += /usr/lib/i386-linux-gnu/gtk-2.0/
INCLUDEPATH += /usr/include/gtk-2.0
INCLUDEPATH += /usr/include/cairo
INCLUDEPATH += /usr/include/pango-1.0/
INCLUDEPATH += /usr/include/gnome-desktop-2.0/
INCLUDEPATH += /usr/include/glib-2.2/
INCLUDEPATH += /usr/lib/i386-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/include/libwnck-1.0/
INCLUDEPATH += /usr/include/gio-unix-2.0/
INCLUDEPATH += ../unix/gnome_menus
INCLUDEPATH += /usr/include/gnome-vfs-2.0/
SOURCES = platform_x11_hotkey.cpp \
    platform_base_hotkey.cpp \
    platform_gnome.cpp \
    platform_gnome_util.cpp \
    process_menus.cpp \
    gnome_menus/menu-util.c \
    gnome_menus/menu-monitor.c \
    gnome_menus/menu-layout.c \
    gnome_menus/gmenu-tree.c \
    gnome_menus/entry-directories.c \
    gnome_menus/desktop-entries.c \
    gnome_menus/canonicalize.c \
    ../../src/plugin_interface.cpp \
    ../unix/platform_unix_util.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    hookGnomeWindows.cpp \
    ../../src/globals.cpp
HEADERS = platform_base.h \
    platform_gnome.h \
    platform_base_hotkey.h \
    platform_base_hottrigger.h \
    platform_gnome_util.h \
    platform_x11_hotkey.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h \
    gnomeSignalReceiver.h \
    ../unix/platform_unix_util.h
CONFIG -= embed_manifest_dll
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.2 \
    libgnomeui-2.0 \
    libwnck-1.0

# gnome-desktop-2.2
LIBS += -lX11 \
    -lXext \
    -lXrender
DEFINES += SKINS_PATH=$(SKINS_PATH) \
    PLUGINS_PATH=$(PLUGINS_PATH) \
    PLATFORMS_PATH=$(PLATFORMS_PATH) \
    GNOME_PLUGIN_DEFINE=$DEFINED

# if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = debug/
# if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = release/
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/
