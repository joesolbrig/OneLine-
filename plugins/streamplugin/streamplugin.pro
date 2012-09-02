TEMPLATE = lib
CONFIG += plugin \
    debug_and_release
CONFIG += oauth
CONFIG += link_pkgconfig
PKGCONFIG += QJson
VPATH += ../../thirdparty/qoath/qoauth/lib/
VPATH += ../../thirdparty/qjson/lib/
INCLUDEPATH += ../../src/ \
    QtXmlPatterns \
    ../../thirdparty/qoauth \
    ../../thirdparty/twitterapi \
    ../../thirdparty/qoath/qoauth/src \
    ../../thirdparty/qoath/qoauth/include \
    ../../thirdparty/qjson/src

# qoauth requires the appropriate qca plugin TOO but loads this **at runtime**
# from the qt directory
LIBS += -L/usr/lib \
    -L/usr/lib/qt4/plugins/crypto \
    -L/usr/local/lib/lib \
    -lqca \
    -lqjson
QT += xmlpatterns
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
    iconfromurl_reader.h
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
win32:LIBS += ../../thirdparty/qoath/qoauth/lib/libqca.a
win32:LIBS += ../../thirdparty/qoath/qoauth/lib/libqca-ossl.a
win32:LIBS += ../../thirdparty/qoath/qoauth/lib/libqoauth.a
win32:LIBS += ../../thirdparty/qjson/lib/libqjson.a
unix:LIBS += ../../thirdparty/qoath/qoauth/lib/libqca.so

# unix:LIBS += ../../thirdparty/qoath/qoauth/lib/libqca-ossl.so
unix:LIBS += /home/hansj/qtsdk-2010.01/qt/plugins/crypto/libqca-ossl.so

# unix:LIBS += libqca-ossl.so
unix:LIBS += ../../thirdparty/qoath/qoauth/lib/libqoauth.so
unix:LIBS += /usr/local/lib/lib/libqjson.so.0
TARGET = streamplugin
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
