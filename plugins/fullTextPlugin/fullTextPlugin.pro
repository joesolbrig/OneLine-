TEMPLATE = lib
QT += network
RECOL_DIR=../../thirdparty/recoll/recoll2/recoll-1.18.1
CONFIG += plugin \
    debug_and_release
SOURCES = fullTextPlugin.cpp \
    recoll_interface.cpp \
    ../../src/platform_util.cpp \
    ../../src/plugin_interface.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    address.cc
HEADERS = fullTextPlugin.h \
    recoll_interface.h \
    address.h \
    ../../src/item.h \
    ../../src/itemWSHash.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h
VPATH += ../../src/ \
    $${RECOL_DIR}\
    $${RECOL_DIR}/utils/ \
    $${RECOL_DIR}/rcldb/ \
    $${RECOL_DIR}/lib/ \
    ../utils/
INCLUDEPATH += ../../src/ \
    $${RECOL_DIR}/ \
    $${RECOL_DIR}/utils/ \
    $${RECOL_DIR}/rcldb/ \
    $${RECOL_DIR}/index/ \
    $${RECOL_DIR}/query/ \
    $${RECOL_DIR}/common/ \
    $${RECOL_DIR}/internfile/ \
    $${RECOL_DIR}/lib/ \
    $${RECOL_DIR}/bincimapmime/ \
    $${RECOL_DIR}/utils/
TARGET = fullTextPlugin
CONFIG += link_pkgconfig
unix { 
    DEFINES += SKINS_PATH=$(SKINS_PATH) \
        PLUGINS_PATH=$(PLUGINS_PATH) \
        PLATFORMS_PATH=$(PLATFORMS_PATH)
    OBJECTS_DIR = .obj
    LIBS += $${RECOL_DIR}/lib/librcl.a \
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
