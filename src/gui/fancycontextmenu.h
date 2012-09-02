#ifndef FANCYCONTEXTMENU_H
#define FANCYCONTEXTMENU_H

#include <QWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QMenu>
#include "itemarglist.h"
#include "constants.h"

class FancyContextMenu : public QWidget
{
    Q_OBJECT

    QBoxLayout m_layout;
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
    void mousePressEvent(QMouseEvent *e){
        m_mousePos = e->pos();
        if(geometry().contains(m_mousePos)){ return; }
        end();

        QWidget::mousePressEvent(e);
    }

signals:
    void operateOnItem(QString, const CatItem );

public slots:

    void sliderChanged(int value);
    void optionChosen();
};

#endif // FANCYCONTEXTMENU_H
