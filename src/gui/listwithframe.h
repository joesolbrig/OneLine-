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

class ListInnerLayout;

class ListWithFrame : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QPointF location READ getLocation WRITE setLocation)
    Q_PROPERTY(double fadedness READ getFadedness WRITE setFadedness)
    QWidget* m_faderWidget;
    bool m_floatingAtSide;


public:
    void paintEvent( QPaintEvent * event );

    ListInnerLayout * m_layout;

    //Drive-by functionality!
    bool m_hovered;
    time_t m_lastHover;
    void leaveEvent( QEvent * ) {
        m_hovered=false;
        m_lastHover = appGlobalTime();
    }
    void mouseMoveEvent(QMouseEvent *  ){
        m_hovered = true;
    }

    CatItem organzingItem();

    double getFadedness(){
//        if(!m_faderWidget){
//            return 0;
//        } else {
//            return (m_faderWidget->windowOpacity());
//        }
        return (windowOpacity());
    }

    void setFadedness(double fadedness);

    void fadeIn(){
        setFadedness(0);
        setVisible(true);
        QPropertyAnimation* an = new QPropertyAnimation(this,"fadedness");
        an->setDuration(1200);
        an->setStartValue(0);
        an->setEndValue(1);
        an->start();

        connect(an, SIGNAL(finished()), an,SLOT(deleteLater()));
        show();
        //raise();
    }
    void adjustSizeToContentsSize();

    ListWithFrame(QWidget* parent=0, CatItem contextItem=CatItem());
    ~ListWithFrame();
    QListWidgetItem *currentItem() const;
    QListWidgetItem *currentOrHoverItem() const;
    QListWidgetItem *hoverItem() const;
    virtual void doUpdate();
    int count();
    void clear();
    void updateItem(ListItem itm);

    void addItemList(QList<ListItem> il);
    void removeItem(CatItem item);

    void setPreviewSearch(QString searchStr);
    void setPreviewItem(ListItem li);
    void endPreview();
    void setContextItem(CatItem contextItem);
    QString getPreviewUrl();
    bool atSidePreview();

    void parentKeyPress(QKeyEvent* key);
    void parentKeyRelease(QKeyEvent* key);

    virtual int sizeHintForRow(int row);
    
    void moveUp();
    void moveDown();

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

    QListWidgetItem* getItem(int i);

    int getCurrentRow();
    int getRowCount();
    QRect getSelectedRect();
    void setCurrentRow(int r);
    void setCurrentItem(CatItem item);

    //void addBottomMessage(QString msg);

    void setGeometry(QRect r);


};

#endif // LISTWITHFRAME_H
