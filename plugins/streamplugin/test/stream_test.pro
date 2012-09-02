TEMPLATE = app
CONFIG += debug_and_release \
    console
DEFINES += STANDALONE_CMDLINE=1
INCLUDEPATH += ../ \
    ../../../src/ \
    ../../../src/gui \
    QtXmlPatterns \
    ../../../thirdparty/qoauth \
    ../../../thirdparty/twitterapi \
    ../../../thirdparty/qoath/qoauth/src \
    ../../../thirdparty/qoath/qoauth/include \
    ../../../thirdparty/qjson/src
QT += core
QT += xmlpatterns
QT += webkit
QT += network
QT += xml
LIBS += -L/usr/lib \
    -lqca
FORMS = pin_dialog.ui
HEADERS = ../../../src/plugin_interface.h \
    ../streamplugin.h \
    ../feed_read.h \
    ../../../src/item.h \
    ../feed_reader.h \
    ../twitter_reader.h \
    ../../../src/constants.h \
    ../../../src/gui/previewpane.h \
    ../../../src/itemWSHash.h \
    ../../../src/inputLanguage.h \
    ../../../src/fillInItem.h \
    ../../../src/gui/url_change_receiver.h \
    ../test_stream_window.h \
    ../facebook_reader.h \
    ../remote_item_source.h \
    ../../../src/globals.h \
    ../recursive_json.h \
    ../iconfromurl_reader.h
SOURCES = ../../../src/plugin_interface.cpp \
    ../streamplugin.cpp \
    ../feed_read.cpp \
    main.cpp \
    ../../../src/inputLanguage.cpp \
    ../../../src/gui/previewpane.cpp \
    ../../../src/fillInItem.cpp \
    ../../../src/list_item.cpp \
    ../facebook_read.cpp \
    ../../../src/dataTreeInstances.cpp \
    ../recursive_json.cpp \
    ../iconfromurl_reader.cpp
win32:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqca.a
win32:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqca-ossl.a
win32:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqoauth.a
unix:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqca.so
unix:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqca-ossl.so
unix:LIBS += ../../../thirdparty/qoath/qoauth/lib/libqoauth.so
unix:LIBS += ../../../thirdparty/qjson/lib/libqjson.so
TARGET = stream_plugin_test
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = .
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = .
