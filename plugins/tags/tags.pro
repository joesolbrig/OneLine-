TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
INCLUDEPATH += ../../src/
FORMS = 
HEADERS = plugin_interface.h \
    tags.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h
SOURCES = tags.cpp \
    ../../src/plugin_interface.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    ../../src/list_item.cpp
TARGET = tagsPlugin
win32 { 
    CONFIG -= embed_manifest_dll
    LIBS += shell32.lib
    % LIBS += user32.lib
    % LIBS += Gdi32.lib
    % LIBS += comctl32.lib
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
