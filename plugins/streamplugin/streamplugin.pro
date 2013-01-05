TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
CONFIG += oauth
CONFIG += link_pkgconfig
PKGCONFIG += QJson \
    libcrypto \
    qca2

VPATH += /usr/local/lib/
INCLUDEPATH += ../../src/ \
    QtXmlPatterns \
    /usr/include/QtOAuth \
    /usr/include/QtOAuth/include \
    /usr/local/lib/include/qjson \
    /usr/include/qjson/ \
    /usr/local/include/QtCrypto \
    /usr/include/QtCrypto \
    /usr/include/qt4/QtXmlPatterns/

# qoauth requires the appropriate qca plugin TOO but loads this **at runtime**
# from the qt directory
LIBS += -L/usr/lib \
    -L/usr/lib/qt4/plugins/crypto \
    -L/usr/local/lib/lib \
    -lqca \
    -lqjson
QT += xmlpatternslibcrypto
QT += webkit
QT += network
QT += xml
FORMS = 
HEADERS = streamplugin.h \
    feed_reader.h \
    twitter_reader.h \
    remote_item_source.h \
    authenticator.h \
    facebook_reader.h \
    ../../src/constants.h \
    ../../src/item.h \
    ../../src/itemarglist.h \
    ../../src/inputLanguage.h \
    ../../src/fillInItem.h \
    recursive_json.h \
    iconfromurl_reader.h \
    ../../src/itemWSHash.h
SOURCES = ../../src/plugin_interface.cpp \
    streamplugin.cpp \
    feed_read.cpp \
    ../../src/itemarglist.cpp \
    ../../src/inputLanguage.cpp \
    ../../src/fillInItem.cpp \
    facebook_read.cpp \
    ../../src/dataTreeInstances.cpp \
    recursive_json.cpp \
    iconfromurl_reader.cpp

# win32:LIBS += /usr/local/lib/libqca.so
#unix:LIBS += /usr/local/lib/libqca.so
unix:LIBS += /usr/lib/qt4/plugins/crypto/libqca-ossl.so
unix:LIBS += /usr/lib/libqoauth.so
#unix:LIBS += /usr/local/lib/lib/libqjson.so.0
TARGET = streamplugin
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
