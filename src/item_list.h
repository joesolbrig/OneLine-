#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#ifndef EDIT_CONTROLS_H
#define EDIT_CONTROLS_H
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QScrollBar>
#include <QBoxLayout>
#include "item.h"
#include "edit_controls.h"

class ItemList : public QBoxLayout {
private:
    MyDisplayItem* top_display;
    QCharListWidget* item_list;
    MyDisplayItem* bottom_item;

public:

    ItemList(QWidget parent,QString name):QBoxLayout(Qt::Horizontal,parent){
        top_display = new MyDisplayItem;
        addWidget(top_display);
        item_list = new QCharListWidget;
        addWidget(item_list);
        bottom_item = new MyDisplayItem;
        addWidget(bottom_item);
    }

    int count(){
        return item_list->count();
    }
    QListWidgetItem * currentItem(){
        return item_list->currentItem();
    }

    void setCurrentRow(int i){
        item_list->setCurrentRow(i);
    }

    void addItem(QListWidgetItem * item){
        item_list->addItem(item);
    }

    QListWidgetItem * takeItem(int i){
        item_list->takeItem(i);
    }

    void parentKeyPress(QKeyEvent* key){
        item_list->parentKeyPress(key);
    }

    QAbstractItemDelegate* itemDelegate(){
        item_list->itemDelegate();
    }

}


#endif // ITEM_LIST_H
