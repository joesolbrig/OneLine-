#ifndef EDIT_CONTROLS_H
#define EDIT_CONTROLS_H
#include <QKeyEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QScrollBar>
#include "item.h"
#include "globals.h"

struct ListItemElement {
    QRect m_rect;
    QString m_toolMessage;
    QString m_actionName;
    QIcon m_iconDetailIcon;

};

class MyListWidget : public QListWidget
{
    Q_OBJECT

    QPersistentModelIndex m_lastIndex;
    QTime m_hoveStart;
    bool m_showHotkey;
    bool m_hovered;

public:
    QPersistentModelIndex m_hoveredIndex;
    bool m_arrowHover;

    MyListWidget(QWidget* parent);

    void paintEvent ( QPaintEvent * event );

    void updateDisplay(){
        QListWidget::update();

    }


    QListWidgetItem* currentOrHoverItem() {
        QTime now = QTime::currentTime();
        if(m_hovered && m_lastIndex.isValid() && (m_hoveStart.msecsTo(now)  > LIST_HOVER_START_INTERVAL)){
            return itemFromIndex(m_lastIndex);
        } else {
            return (QListWidget::currentItem());
        }
    }

    QListWidgetItem* hoverItem() {
        if(m_hovered){
            QTime now = QTime::currentTime();
            if(m_lastIndex.isValid() && (m_hoveStart.msecsTo(now)  > LIST_HOVER_START_INTERVAL)){
                return itemFromIndex(m_lastIndex);
            }
        }
        return 0;
    }

    void setGeometry(QRect r){
        QListWidget::setGeometry(r);
        setAutoScrollMargin(r.height()/3);
    }

    void setGeometry(int x, int y, int w, int h){
        QListWidget::setGeometry(x, y, w, h);
        setAutoScrollMargin(h/3);
    }

    void keyPressEvent(QKeyEvent* key) {
        emit listKeyPressed(key);
        key->ignore();
    }

    void parentKeyPress(QKeyEvent* key){
        QListWidget::keyPressEvent(key);
    }

    void parentKeyRelease(QKeyEvent* key){
        QListWidget::keyReleaseEvent(key);
    }

    void mouseDoubleClickEvent( QMouseEvent * ) {
            QKeyEvent key(QEvent::KeyPress, Qt::Key_Enter, NULL);
            emit listKeyPressed(&key);
    }

    void mousePressEvent( QMouseEvent * event  );
    void mouseMoveEvent( QMouseEvent * event  );


    void focusOutEvent ( QFocusEvent * evt) {
        emit focusOut(evt);
    }

    void setCurrentRow(int row){
        QListWidget::setCurrentRow(row);
        m_lastIndex = currentIndex();
        m_hovered=false;
    }

    void moveUp(){
        setCurrentRow(currentRow()-1);
    }

    void moveDown(){
        setCurrentRow(currentRow()+1);
    }

    void setCurrentItem(CatItem item);


    QList<ListItemElement> getListItemElements(QPersistentModelIndex index,
                                               QRect containingRect);

    int itemsHeight();
    QRect enclosingRect(int);
    void contextMenuEvent( QContextMenuEvent * event ){
        sendMenuEvent(event);
    }

public slots:
    void itemChanged( QListWidgetItem * n , QListWidgetItem * );
    void sendMenuEvent(QContextMenuEvent*);

signals:
    void listKeyPressed(QKeyEvent*);
    void listMouseDoubleClicked(QMouseEvent * event );
    void listItemAction(QString path, CatItem a );
    void showOptionsMenu(QString path, QPoint p);
    void focusOut(QFocusEvent* evt);


};

#endif // EDIT_CONTROLS_H
