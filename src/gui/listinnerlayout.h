#ifndef LISTINNERLAYOUT_H
#define LISTINNERLAYOUT_H


//#include "messageiconwidget.h"
#include <QFont>
#include <QListWidget>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QTextDocument>
#include <QPainter>
#include <QTextLayout>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QTextStream>
#include "item.h"
#include "list_item.h"
#include "icon_delegate.h"
//#include "iconspreadwindow.h"
#include "../globals.h"

#include "previewpane.h"
#include "borderwindow.h"
#include "textmessagewidget.h"


class MyListWidget;

class MultiInputDisplayer;

class ListInnerLayout : public QBoxLayout {
    Q_OBJECT
private:
    QWidget* m_parentWidget;
    bool m_previewing;
    QString m_bottomMessage;
    QWidget* m_fadeWidget;

public:
    CatItem m_contextItem;
    MyListWidget* m_realItemList;
    PreviewPane* m_previewPane;

    ListInnerLayout(QWidget* parent, CatItem contextItem=CatItem()) ;

    ~ListInnerLayout();

    MyListWidget* getRealList(){return m_realItemList;}

    virtual void doUpdate();

    void moveUp();
    void moveDown();


    void clear();
    void setCurrentRow(int r);
    void setCurrentItem(CatItem item);

    QListWidgetItem* getItem(int i);
    int itemCount();

    void parentKeyPress(QKeyEvent* key);
    void parentKeyRelease(QKeyEvent* key);

    void setPreview(ListItem li);
    void endPreview(int i);

    virtual int sizeHintForRow(int row);

    void addItem(ListItem itm);
    void updateItem(ListItem itm);

    void addItemList(QList<ListItem> il);

    QListWidgetItem * menuItFromListItem(ListItem ir, int charsAllowed);

    QListWidgetItem *currentItem() const;
    QListWidgetItem *currentOrHoverItem() const;
    QListWidgetItem *hoverItem() const;
    void removeCatItem(CatItem item);

    int charsWidth();

    int charsHeight();

    void setListWidth(int w);

};



#endif // LISTINNERLAYOUT_H
