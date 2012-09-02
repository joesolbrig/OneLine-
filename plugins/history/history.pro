TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
VPATH += ../../src/
INCLUDEPATH += ../../src/
FORMS = 
HEADERS = history.h \
    ../../src/item.h \
    ../../src/constants.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h
SOURCES = history.cpp \
    ../../src/plugin_interface.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp
TARGET = history
win32 { 
    CONFIG -= embed_manifest_dll
    LIBS += shell32.lib
    % LIBS += user32.lib
    % LIBS += Gdi32.lib
    % LIBS += comctl32.lib
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
