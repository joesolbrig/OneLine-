# -------------------------------------------------
# Project created by QtCreator 2009-12-07T22:51:38
# -------------------------------------------------
QT -= gui
TARGET = lbtree
CONFIG += console
CONFIG -= app_bundle

# CONFIG += warn_off
TEMPLATE = app
INCLUDEPATH += ../../../thirdparty/Tokyo_Cabinet/kyotocabinet-1.2.15
SOURCES += main.cpp \
    test.cpp
OTHER_FILES += 
HEADERS += btree.h \
    btree_impl.h \
    multi_tree_db.h \
    tree_struct_test.h \
    mmap_pointer.h \
    mmap_pointer_impl.h
unix:LIBS += -lkyotocabinet \
    -lstdc++ \
    -lpthread
