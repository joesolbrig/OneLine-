TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
VPATH += ../../src/ \
    ../../thirdparty/recoll/recoll-1.13.01/ \
    ../../thirdparty/recoll/recoll-1.13.01/utils/ \
    ../../thirdparty/recoll/recoll-1.13.01/rcldb/ \
    ../../thirdparty/recoll/recoll-1.13.01/lib/ \
    ../utils/
INCLUDEPATH += ../../src/ \
    ../../thirdparty/recoll/recoll-1.13.01/ \
    ../../thirdparty/recoll/recoll-1.13.01/utils/ \
    ../../thirdparty/recoll/recoll-1.13.01/rcldb/ \
    ../../thirdparty/recoll/recoll-1.13.01/index/ \
    ../../thirdparty/recoll/recoll-1.13.01/query/ \
    ../../thirdparty/recoll/recoll-1.13.01/common/ \
    ../../thirdparty/recoll/recoll-1.13.01/internfile/ \
    ../../thirdparty/recoll/recoll-1.13.01/lib/ \
    ../../thirdparty/recoll/recoll-1.13.01/bincimapmime/ \
    ../../thirdparty/recoll/recoll-1.13.01/utils/
HEADERS = fullTextPlugin.h \
    recoll_interface.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h \
    address.h
SOURCES = fullTextPlugin.cpp \
    recoll_interface.cpp \
    ../../src/platform_util.cpp \
    ../../src/plugin_interface.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    address.cc
TARGET = fullTextPlugin
CONFIG += link_pkgconfig
unix { 
    DEFINES += SKINS_PATH=$(SKINS_PATH) \
        PLUGINS_PATH=$(PLUGINS_PATH) \
        PLATFORMS_PATH=$(PLATFORMS_PATH)
    OBJECTS_DIR = .obj
    LIBS += ../../thirdparty/recoll/librcl.a \
        $(BSTATIC) \
        -L/usr/local/lib \
        -lxapian \
        -lz \
        $(BDYNAMIC) \
        -lz
    POST_TARGETDEPS = ../../thirdparty/recoll/librcl.a
}
win32 { 
    CONFIG -= embed_manifest_dll
    LIBS += shell32.lib
    % LIBS += user32.lib
    % LIBS += Gdi32.lib
    % LIBS += comctl32.lib
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
