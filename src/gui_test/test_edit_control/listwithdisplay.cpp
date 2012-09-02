#include <QIcon>
#include "constants.h"
#include "icon_delegate.h"
#include "listwithdisplay.h"

QListWidgetItem * ListWithDisplayHolder::menuItFromListItem(ListItem ir, int charsAllowed){
        QString fullPath = ir.getPath();
        QString adjPath = QDir::toNativeSeparators(fullPath);
        //QFileInfo fileInfo(fullPath);
        QIcon icon = ir.getActualIcon();
        QListWidgetItem * menuItem = new QListWidgetItem(icon,adjPath);
        qlonglong itemId = ir.getItemId();
        menuItem->setData(ROLE_FULL, ir.getFormattedPath(charsAllowed));
        menuItem->setData(ROLE_SHORT, ir.formattedName());
        menuItem->setData(ROLE_ICON, icon);
        menuItem->setData(ROLE_ID, itemId);
        menuItem->setData(ROLE_DESCRIPTION, ir.getDescription());
        menuItem->setData(ROLE_MATCH_TYPE, ir.getMatchType());
        if(ir.hasChildren()){
            menuItem->setData(ROLE_HAS_CHILDREN, 1);
        } else {
            menuItem->setData(ROLE_HAS_CHILDREN, 0);
        }
        menuItem->setData(ROLE_ITEM_TAG_LEVEL, (int)ir.getTagLevel());
        menuItem->setToolTip(ir.getDescription());
        return menuItem;
}


//Same operation, animated differently...
void ListWithDisplay::addChildList(QList<ListItem> il){
    m_newFrame = new ListWithFrame(this);
    m_newFrame->addItemList(il);
    QRect g = m_stdFrame->geometry();
    g.moveLeft(g.right());
    m_newFrame->setGeometry(g);
//
    m_newFrame->setVisible(true);
//    m_newFrame->raise();
//

    FrameAnimation* an = new FrameAnimation(m_newFrame, m_stdFrame, this, FrameAnimation::BeginLeft);
    an->startAnimation();
    //animationDone();
}

//Same operation, animated differently...
void ListWithDisplay::addParentList(QList<ListItem> il){
    m_newFrame = new ListWithFrame(this);
    m_newFrame->addItemList(il);
    QRect g = m_stdFrame->geometry();
    g.moveRight(g.left());
    m_newFrame->setGeometry(g);

    m_newFrame->setVisible(true);
//    m_stdFrame->lower();
//    m_newFrame->raise();
    FrameAnimation* an = new FrameAnimation(m_newFrame, m_stdFrame, this, FrameAnimation::BeginRight);
    an->startAnimation();
    //animationDone();
}
