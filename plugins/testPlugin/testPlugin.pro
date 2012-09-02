TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
INCLUDEPATH += ../../src/
FORMS = 
HEADERS = plugin_interface.h \
    testPlugin.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h \
    ../../src/is_test.h
SOURCES = ../../src/plugin_interface.cpp \
    testPlugin.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp
TARGET = testPlugin
win32 { 
    CONFIG -= embed_manifest_dll
    LIBS += shell32.lib
    % LIBS += user32.lib
    % LIBS += Gdi32.lib
    % LIBS += comctl32.lib
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
