/*
Oneline: Multibrowser
Copyright (C) 2012  Hans Joseph Solbrig

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QPalette>
#include <QLineEdit>
#include <QPixmap>
#include <QBitmap>
#include <QLabel>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTranslator>
#include <QToolTip>


//#include "icon_delegate.h"
//#include "main.h"
//#include "globals.h"
//#include "options.h"
//#include "dsingleapplication.h"
//#include "plugin_interface.h"
#include "edit_controls.h"
#include "icon_delegate.h"

#include "multiTextDisplay.h"
#include "appearance_window.h"
#include "fancyenclosingrect.h"

MyListWidget::MyListWidget(QWidget* parent) :
        QListWidget(parent) {

    m_hoveredIndex =QPersistentModelIndex();
    m_showHotkey=false;
    m_arrowHover= false;

    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAlternatingRowColors(true);
    setAutoScroll(true);
    QRect r = geometry();

    setAutoScrollMargin(r.height()/3);

    connect(this,
            SIGNAL(listItemAction(QString , CatItem )),
            (QObject*)gMainWidget, //
            SLOT(operateOnItem(QString , CatItem )));

    connect(this, SIGNAL(showOptionsMenu(QString, QPoint)),
            (QObject*)gMainWidget, SLOT(listMenuEvent(QString, QPoint)),
            Qt::QueuedConnection);

    setMouseTracking(true);
    m_hoveStart = QTime::currentTime();
    m_hovered= false;
}

void MyListWidget::paintEvent ( QPaintEvent * event ){
    QListWidget::paintEvent(event);

    if(parentWidget() && ((ListWithFrame*)this->parentWidget())->atSidePreview()){
        QPainter painter(this);
        QRect extent = geometry();
        extent.setTopLeft(QPoint(0,0));

        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(10);
        QBrush brush;
        brush.setColor(Qt::transparent);
        extent.adjust(10,10,-10,-10);
        painter.save();
        painter.setPen(pen);
        //painter.setBrush(brush);
        painter.drawRect(extent);
        painter.restore();
        //FancyEnclosingRect::drawStyledRects(&painter,extent,MAIN_LIST_NAME);
    }

}


int MyListWidget::itemsHeight(){
    this->count();
    int s=0;
    for(int i=0; i<count(); i++){
        QListWidgetItem* it = item(i);
        QModelIndex index = indexFromItem(it);
        QRect containingRect = visualRect(index);
        s+= containingRect.height();
    }
    return s;
}

QRect MyListWidget::enclosingRect(int i){
    QListWidgetItem* it = item(i);
    QModelIndex index = indexFromItem(it);
    QRect containingRect = visualRect(index);
    return containingRect;
}

QList<ListItemElement> MyListWidget::getListItemElements(QPersistentModelIndex index,
                                                         QRect containingRect){
    QList<ListItemElement> res;
    ListItemElement pinElement;
    QRect pinRect = IconDelegate::getPinItemRect(containingRect);
    pinElement.m_rect = pinRect;
    pinElement.m_toolMessage = PIN_MESSAGE;
    pinElement.m_actionName = PIN_OPERATION_NAME;
    res.append(pinElement);

    ListItemElement depricateElement;
    QRect depricateRect = IconDelegate::getDepricateItemRect(containingRect);
    depricateElement.m_rect = depricateRect;
    depricateElement.m_toolMessage = DEPRICATE_MESSAGE;
    depricateElement.m_actionName = DEPRICATE_OPERATION_NAME;
    res.append(depricateElement);

    if(index.data(ROLE_HAS_PARENT_ELEMENT).toBool()){
        ListItemElement parentArrowElement;
        QRect parentArrowRect = IconDelegate::getParentArrowRect(containingRect);
        parentArrowElement.m_rect = parentArrowRect;
        parentArrowElement.m_toolMessage = DEPRICATE_MESSAGE;
        parentArrowElement.m_actionName = DEPRICATE_OPERATION_NAME;
        res.append(parentArrowElement);
    }

    if(index.data(ROLE_ITEM_TAG_LEVEL).toInt()>= ((int)CatItem::POSSIBLE_TAG) ){
        ListItemElement childArrowElement;
        QRect childArrowRect = IconDelegate::getChildArrowRect(containingRect);
        childArrowElement.m_rect = childArrowRect;
        childArrowElement.m_toolMessage = DEPRICATE_MESSAGE;
        childArrowElement.m_actionName = DEPRICATE_OPERATION_NAME;
        res.append(childArrowElement);
    } else if(index.data(ROLE_HAS_CHILDREN).toBool()){
        ListItemElement moreItemsElement;
        QRect moreItemsRect = IconDelegate::getDepricateItemRect(containingRect);
        moreItemsElement.m_rect = moreItemsRect;
        moreItemsElement.m_toolMessage = DEPRICATE_MESSAGE;
        moreItemsElement.m_actionName = DEPRICATE_OPERATION_NAME;
        res.append(moreItemsElement);
    }

    return res;

}

void MyListWidget::mouseMoveEvent(QMouseEvent * event ){
    event->ignore();
    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);
    QPoint globPos = QWidget::mapToGlobal(pos);
    QTime now = QTime::currentTime();
    m_arrowHover = false;

    if(m_lastIndex == index){
//        if(m_hoveStart.msecsTo(now )< LIST_HOVER_INTERVAL){
//            return;
//        }
        m_hoveredIndex = index;
        m_hovered=true;
        update(index);

        QRect containingRect = visualRect(index);

        QRect pinRect = IconDelegate::getPinItemRect(containingRect);
        if(pinRect.contains(pos)){
            QToolTip::showText(globPos, PIN_MESSAGE,  this);
            return;
        }

        QRect depRect = IconDelegate::getDepricateItemRect(containingRect);
        if(depRect.contains(pos)){
            QToolTip::showText(globPos, DEPRICATE_MESSAGE, this);
            return;
        }

        QRect childRect = IconDelegate::getChildArrowRect(containingRect);
        if(childRect.contains(pos)){
            QToolTip::showText(globPos, GOTO_CHILDREN_MESSAGE, this);
            m_arrowHover = true;
            return;
        }
        QToolTip::showText(globPos, index.data(ROLE_DESCRIPTION).toString(), this);

    } else {
        if(m_hoveredIndex !=QPersistentModelIndex()){
            QPersistentModelIndex oldIndex = m_hoveredIndex;
            m_hoveredIndex =QPersistentModelIndex();
            update(oldIndex);
        }
        m_hoveStart = now;
        m_lastIndex = index;
    }
    gMainWidget->m_itemChoiceList->fakeMouseMoveEvent(event);
}


void MyListWidget::itemChanged( QListWidgetItem * n , QListWidgetItem * ){
     QModelIndex index = this->indexFromItem(n);
     QString path = index.data(ROLE_ITEM_PATH).toString();
     CatItem clickItem(addPrefix(OPERATION_PREFIX,SELECTION_OPERATION_NAME));
     gMainWidget->operateOnItem(path,clickItem);
}

void MyListWidget::sendMenuEvent(QContextMenuEvent* event){
    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);
    QString path = index.data(ROLE_ITEM_PATH).toString();
//    QPoint globalPos = QWidget::mapToGlobal(pos);
//    emit showOptionsMenu(path, globalPos);
//    QPoint mainWidgetPos = mapTo((QWidget*)gMarginWidget,pos);
    QPoint mainWidgetPos = this->mapToGlobal(pos);
    emit showOptionsMenu(path, mainWidgetPos);

}

void MyListWidget::mousePressEvent( QMouseEvent * event  ) {
    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);

    QString path = index.data(ROLE_ITEM_PATH).toString();
    if(event->button() != Qt::LeftButton){
        QPoint globalPos = QWidget::mapToGlobal(pos);
        emit showOptionsMenu(path,globalPos);
        //emit pressed(index);
        return;

    }

    //Later...
    //if(event->button() == Qt::RightButton){ }

    QRect containingRect = this->geometry();
    containingRect.setHeight(UI_DEFAULT_ITEM_HEIGHT);
    containingRect.moveTop(0);

    QRect pinRect = IconDelegate::getPinItemRect(containingRect);
    QListWidgetItem*  listWidgetItem = item(index.row());

    QPoint localPos = pos;
    localPos.setY(pos.y() % UI_DEFAULT_ITEM_HEIGHT);

    if(pinRect.contains(localPos)){
        CatItem pinItem(addPrefix(OPERATION_PREFIX,PIN_OPERATION_NAME));

        if(!(listWidgetItem->data((ROLE_ITEM_PINNED)).toBool())){
            listWidgetItem->setData((ROLE_ITEM_PINNED),true);
        } else {listWidgetItem->setData((ROLE_ITEM_PINNED),false);}

        //emit listItemAction(path, pinItem);
        //frickin signal not going through
        gMainWidget->operateOnItem(path,pinItem);
        return;
    }

    QRect childRect = IconDelegate::getChildArrowRect(containingRect);
    if(childRect.contains(localPos)){
        listWidgetItem->setSelected(true);
        setCurrentRow(index.row());
        CatItem clickItem(addPrefix(OPERATION_PREFIX,GOTO_CHILD_OPERATION));
        gMainWidget->operateOnItem(path,clickItem);
        return;
    }

    QRect depRect = IconDelegate::getDepricateItemRect(containingRect);
    if(depRect.contains(localPos)){
        CatItem depricateItem(addPrefix(OPERATION_PREFIX,DEPRICATE_OPERATION_NAME));
        //emit listItemAction(path, depricateItem);
        //fickin signals not going through...
        gMainWidget->operateOnItem(path,depricateItem);
        return;
    }
    if(listWidgetItem){
        listWidgetItem->setSelected(true);
        setCurrentRow(index.row());
        CatItem clickItem(addPrefix(OPERATION_PREFIX,SELECTION_OPERATION_NAME));
        gMainWidget->operateOnItem(path,clickItem);
    }

}

void MyListWidget::setCurrentItem(CatItem catItem){
    for(int i=0; i< count();i++){
        QListWidgetItem* listWidgetItem = item(i);
        if((listWidgetItem->data((ROLE_ITEM_PATH)).toString() == catItem.getPath())){
            setCurrentRow(i);
            break;
        }
    }
}

void MyDisplayItem::setText(const QString s) {
    QString t = s;
    QLabel::setText(t);
}


void MyDisplayItem::keyPressEvent(QKeyEvent* key) {
    //This shouldn't directly handle key events..
    //QLineEdit::keyPressEvent(key);
    emit keyPressed(key);
}



