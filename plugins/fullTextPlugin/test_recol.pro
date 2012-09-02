# -------------------------------------------------
# Project created created by HJS manually jan 18, 2009
# -------------------------------------------------

#QT -= gui
VPATH +=  ../../thirdparty/recoll/recoll-1.13.01/ \
    ../../thirdparty/recoll/recoll-1.13.01/utils/ \
    ../../thirdparty/recoll/recoll-1.13.01/rcldb/ \
    ../../thirdparty/recoll/recoll-1.13.01/lib/ \
    ../../thirdparty/recoll/recoll-1.13.01/Bincimapmime/ \
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

TEMPLATE = app
TARGET = test_recol
SOURCES += main.cpp \
    test.cpp \
    recoll_interface.cpp \
    ../../src/plugin_interface.cpp

HEADERS += recoll_interface.h \
    test.h

CONFIG += console
CONFIG += link_pkgconfig

UNAME = $$system(uname -s)
contains( UNAME, [lL]inux ) {
    LIBS -= -liconv
    LIBS += -ldl \
        -lX11
}
contains( UNAME, SunOS ):LIBS += -ldl


unix { 
    OBJECTS_DIR = .obj
    LIBS += ../../thirdparty/recoll/recoll-1.13.01/lib/librcl.a \
        $(BSTATIC) \
        -L/usr/local/lib \
        -lxapian \
        -lz \
        $(BDYNAMIC) \
        -lz
    POST_TARGETDEPS = ../../thirdparty/recoll/recoll-1.13.01/lib/librcl.a
}

