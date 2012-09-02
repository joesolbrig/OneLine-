TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
VPATH += ../../src/
INCLUDEPATH += ../../src/ \
    /usr/include/gtk-2.0/
HEADERS = fileCatalog.h \
    ../../src/directory.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h \
    ../../src/constants.h
SOURCES = fileCatalog.cpp \
    ../../src/platform_util.cpp \
    ../../src/plugin_interface.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    ../../src/list_item.cpp
TARGET = fileCatalog
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0 \
    libgnomeui-2.0 \
    gnome-desktop-2.0
LIBS += -lX11 \
    -lXext \
    -lXrender \
    /usr/lib/libmagic.so
unix:DEFINES += SKINS_PATH=$(SKINS_PATH) \
    PLUGINS_PATH=/usr/lib/oneline/ \
    PLATFORMS_PATH=$(PLATFORMS_PATH)
win32 { 
    CONFIG -= embed_manifest_dll
    LIBS += shell32.lib
    % LIBS += user32.lib
    % LIBS += Gdi32.lib
    % LIBS += comctl32.lib
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
