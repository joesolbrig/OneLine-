# -------------------------------------------------
# Project created by QtCreator 2010-05-16T17:04:06
# -------------------------------------------------
# QT -= gui
TARGET = gnome_menu_test
CONFIG += console
CONFIG -= app_bundle
CONFIG += debug_and_release \
    console
TEMPLATE = app
INCLUDEPATH += ../
INCLUDEPATH += ../../../../src/
INCLUDEPATH += /usr/include/glib-2.2/
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/lib/glib-2.0/include
SOURCES += main.cpp \
    ../menu-util.c \
    ../menu-monitor.c \
    ../menu-layout.c \
    ../gmenu-tree.c \
    ../entry-directories.c \
    ../desktop-entries.c \
    ../canonicalize.c \
    ../../../../src/plugin_interface.cpp \
    ../../process_menus.cpp
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.2 \
    libgnomeui-2.0
LIBS += -lX11 \
    -lXext \
    -lXrender
HEADERS += ../../../../src/item.h \
    ../../process_menu.h
