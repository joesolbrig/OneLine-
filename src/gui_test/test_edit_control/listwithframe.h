#ifndef LISTWITHFRAME_H
#define LISTWITHFRAME_H

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

class ListWithDisplayHolder;

class ListWithFrame : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QPointF location READ getLocation WRITE setLocation)
private:
    ListWithDisplayHolder * m_layout;
public:
    ListWithFrame(QWidget* parent) ;
    QListWidgetItem *currentItem() const;
    void addItem(ListItem itm);

    void addItemList(QList<ListItem> il);

    virtual int sizeHintForRow(int row);
    

    void setLocation(QPointF p){
        QRect g = this->geometry();
        QPoint pp(p.x(),p.y());
        g.moveTopLeft(pp);
        this->setGeometry(g);
    }

    QPointF getLocation(){
        QRect g = this->geometry();
        QPointF pp(g.x(),g.y());
        return pp;
    }

    void addTopicMessage(QString msg);
    void addNavigationMessage(QString msg);
};

#endif // LISTWITHFRAME_H
