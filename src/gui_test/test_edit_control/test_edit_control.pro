# -------------------------------------------------
# Project created by QtCreator 2010-08-23T16:34:11
# -------------------------------------------------
QT += webkit \
    testlib
INCLUDEPATH += "../../"
INCLUDEPATH += /usr/include/gtk-2.2
INCLUDEPATH += /usr/include/gtk-2.0
INCLUDEPATH += /usr/include/gnome-desktop-2.0/
INCLUDEPATH += /usr/include/glib-2.2/
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/include/libwnck-1.0/
INCLUDEPATH += /usr/include/gnome-vfs-2.0/
TARGET = test_edit_control
TEMPLATE = app
CONFIG += qt_warn \
    debug_and_release
CONFIG -= embed_manifest_dll
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0 \
    gtk+-2.2 \
    libgnomeui-2.0 \
    libwnck-1.0 \
    libnotify

# gnome-desktop-2.2
LIBS += -lX11 \
    -lXext \
    -lXrender
SOURCES += main.cpp \
    edittestmainwindow.cpp \
    myfulltextedit.cpp \
    ../../plugin_interface.cpp \
    ../../list_item.cpp \
    messageiconwidget.cpp \
    listwithdisplay.cpp \
    listwithframe.cpp \
    ../../icon_delegate.cpp \
    iconspreadwindow.cpp \
    individualicon.cpp \
    multiTextDisplay.cpp \
    fancylabel.cpp \
    ../../edit_controls.cpp \
    previewpane.cpp \
    ../../inputLanguage.cpp
HEADERS += edittestmainwindow.h \
    myfulltextedit.h \
    messageiconwidget.h \
    ../../list_item.h \
    listwithdisplay.h \
    ../../information_coalator.h \
    listwithframe.h \
    ../../icon_delegate.h \
    iconspreadwindow.h \
    individualIcon.h \
    ../../constants.h \
    multiTextDisplay.h \
    ../../edit_controls.h \
    fancylabel.h \
    previewpane.h \
    ../../itemWSHash.h \
    ../../inputLanguage.h
FORMS += edittestmainwindow.ui \
    iconspread.ui
OTHER_FILES += 
