#ifndef FANCYCONTEXTMENU_H
#define FANCYCONTEXTMENU_H

#include <QWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QMenu>
#include "itemarglist.h"
#include "constants.h"

class FancyContextMenu;

class MultiItemLabel: public QLabel {
    Q_OBJECT

    QList<CatItem> m_items;
    int m_currentItemIndex;

public:

    MultiItemLabel(FancyContextMenu* parent=0);

    void setItemList(QList<CatItem> items){
        Q_ASSERT(items.count()>0);
        m_items = items;
        m_currentItemIndex=0;
        setItem();
    }

    CatItem& curentItem(){
        return m_items[m_currentItemIndex];
    }

    void paintEvent(QPaintEvent * evt);

    void mousePressEvent( QMouseEvent * event  ){
        if(m_items.count()==1){ return;}

        QPoint pos = event->pos();
        if(downRect().contains(pos) && (m_currentItemIndex<m_items.count()-1)){
            m_currentItemIndex++;
            update();
            emit itemChanged();
        } else  if(upRect().contains(pos) && (m_currentItemIndex >0)){
            m_currentItemIndex--;
            update();
            emit itemChanged();
        }
        setItem();
    }

    void setItem(){
        CatItem item = m_items[m_currentItemIndex];
        QFont f = ListItem(item).getDisplayFont();
        setFont(f);
        setText( tagAs(item.getName(),"b")
                + QString(" priority ") );
        setMargin(0);
        setIndent(0);
        adjustSize();
    }

    QRect upRect(){
        QRect right = downRect();
        right.moveLeft(right.left() - right.height());
        return right;

    }
    QRect downRect(){
        QRect right = geometry();
        right.moveTopLeft(QPoint(0,0));
        right.setLeft(right.right() - right.height());
        return right;
    }

signals:
    void itemChanged();

};


class FancyContextMenu : public QWidget
{
    Q_OBJECT

    QBoxLayout m_layout;
    MultiItemLabel m_topLabel;
    QBoxLayout m_sublayout;
    QSlider m_slider;
    QMenu m_menu;
    CatItem m_priorityItem;
    QHash<QString, CatItem> m_optionItems;
    QPoint m_mousePos;
public:
    FancyContextMenu(CatItem priorityItem, QList<CatItem> actionChildren);
    ~FancyContextMenu();

    void end(){
        hide();
        deleteLater();
    }


signals:
    void operateOnItem(QString, const CatItem );

public slots:
    void sourceChanged();
    void focusOutEvent(QFocusEvent* evt);
    void mousePressEvent(QMouseEvent * event );

    void sliderChanged(int value);
    void optionChosen();
};

#endif // FANCYCONTEXTMENU_H
