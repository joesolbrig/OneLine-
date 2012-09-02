TEMPATE = app
CONFIG += qt_warn \
    debug_and_release \
    qtestlib \
    link_pkgconfig
TARGET = oneline
QT += network
QT += xmlpatterns
QT += xml
QT += webkit
INCLUDEPATH += /usr/include/gtk-2.2
INCLUDEPATH += /usr/include/gtk-2.0
INCLUDEPATH += /usr/include/gnome-desktop-2.0/
INCLUDEPATH += /usr/include/glib-2.2/
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/include/libwnck-1.0/
INCLUDEPATH += /usr/include/gnome-vfs-2.0/
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0 \
    libgnomeui-2.0 \
    libwnck-1.0 \
    libnotify

# Uncomment/comment to enable/disable profiling
# QMAKE_CXXFLAGS_DEBUG += -pg
# QMAKE_LFLAGS_DEBUG += -pg
VPATH += src/ \
    src/util/ \
    src/util/mmap_file/ \
    src/gui/ \
    plugins/fullTextPlugin/ \
    thirdparty/qjson/lib/
INCLUDEPATH += QtXmlPatterns \
    src/gui/ \
    thirdparty/Tokyo_Cabinet/kyotocabinet-1.2.15/ \
    thirdparty/qjson/src
SOURCES = appearance_window.cpp \
    globals.cpp \
    options.cpp \
    catalog.cpp \
    icon_delegate.cpp \
    main.cpp \
    plugin_handler.cpp \
    plugin_interface.cpp \
    platform_util.cpp \
    edit_controls.cpp \
    item.cpp \
    main_window.cpp \
    test.cpp \
    cat_builder.cpp \
    src/cat_store.cpp \
    src/list_item.cpp \
    src/item_rep.cpp \
    src/trie_node.cpp \
    src/gui/multiTextDisplay.cpp \
    src/gui/messageiconwidget.cpp \
    src/gui/listwithframe.cpp \
    src/gui/listwithdisplay.cpp \
    src/gui/fancylabel.cpp \
    src/gui/myfulltextedit.cpp \
    src/itemarglist.cpp \
    src/inputLanguage.cpp \
    src/gui/previewpane.cpp \
    src/gui/fancyHorizontalLabel.cpp \
    src/fillInItem.cpp \
    src/gui/expression_icon_window.cpp \
    src/dataTreeInstances.cpp \
    src/gui/multiinputdisplayer.cpp \
    src/gui/borderwindow.cpp \
    src/gui/textmessagewidget.cpp \
    src/gui/marginwindow.cpp \
    src/gui/listinnerlayout.cpp \
    src/gui/fancycontextmenu.cpp \
    src/gui/fancyenclosingrect.cpp
HEADERS = appearance_window.h \
    catalog.h \
    globals.h \
    icon_delegate.h \
    main.h \
    options.h \
    platform_util.h \
    platform_base.h \
    plugin_interface.h \
    plugin_handler.h \
    edit_controls.h \
    item.h \
    main_window.h \
    test.h \
    src/is_test.h \
    src/test_defines.h \
    src/input_data.h \
    cat_builder.h \
    cat_store.h \
    src/item_rep.h \
    src/util/mmap_file/btree_impl.h \
    src/util/mmap_file/btree.h \
    src/list_item.h \
    src/input_list.h \
    src/constants.h \
    src/trie_node.h \
    src/util/mmap_file/multi_tree.h \
    src/information_coalator.h \
    src/itemWSHash.h \
    src/util/mmap_file/multi_tree_db.h \
    src/util/mmap_file/keyvaluedb.h \
    src/gui/multiTextDisplay.h \
    src/gui/messageiconwidget.h \
    src/gui/listwithframe.h \
    src/gui/listwithdisplay.h \
    src/gui/fancylabel.h \
    src/gui/myfulltextedit.h \
    src/itemarglist.h \
    src/inputLanguage.h \
    src/gui/previewpane.h \
    src/gui/fancyHorizontallabel.h \
    src/fillInItem.h \
    src/gui/url_change_receiver.h \
    src/history_item.h \
    src/gui/expression_icon_window.h \
    src/gui/multiinputdisplayer.h \
    src/gui/borderwindow.h \
    src/gui/textmessagewidget.h \
    src/gui/marginwindow.h \
    src/gui/listinnerlayout.h \
    src/util/mmap_file/mmap_pointer_impl.h \
    src/util/mmap_file/mmap_pointer.h \
    src/gui/fancycontextmenu.h \
    src/gui/fancyenclosingrect.h \
    src/constants_ui.h
ICON = oneline.ico
unix { 
    UI_DIR = src/
    FORMS = src/options.ui
    DEFINES += SKINS_PATH=$(SKINS_PATH) \
        PLUGINS_PATH=$(PLUGINS_PATH) \
        PLATFORMS_PATH=$(PLATFORMS_PATH)
    if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = debug/
    if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = release/
    LIBS += -lkyotocabinet \
        /usr/local/lib/lib/libqjson.so.0 \
        -lstdc++ \
        -lpthread
    LIBS += -lX11 \
        -lXext \
        -lXrender
}
win32 { 
    UI_DIR = ../src/
    FORMS = ../src/options.ui
    RC_FILE = win/oneline.rc
    LIBS += shell32.lib
    if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../debug/
    if(!debug_and_release|build_pass):CONFIG(release, debug|release) { 
        CONFIG += embed_manifest_exe
        DESTDIR = ../release/
    }
}
TRANSLATIONS = tr/oneline_fr.ts \
    tr/oneline_nl.ts \
    tr/oneline_zh.ts
FORMS += 
