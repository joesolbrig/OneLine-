# -------------------------------------------------
# Project created by QtCreator 2010-11-13T21:59:18
# -------------------------------------------------
QT += testlib

# QT -= gui
TARGET = CustomActionTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
VPATH += ..
INCLUDEPATH += ..
SOURCES += langTestmain.cpp \
    inputlanguagetester.cpp \
    ../plugin_interface.cpp \
    ../itemarglist.cpp \
    ../inputLanguage.cpp
HEADERS += inputlanguagetester.h \
    ../itemWSHash.h \
    ../constants.h \
    ../itemarglist.h \
    ../itemWSHash.h \
    ../inputLanguage.h \
    ../input_data.h
