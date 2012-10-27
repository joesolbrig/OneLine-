#include <QIcon>
#include <QPainter>
#include "constants.h"
#include "icon_delegate.h"
#include "listwithdisplay.h"
#include "edit_controls.h"
#include "appearance_window.h"
#include "multiinputdisplayer.h"
#include "listinnerlayout.h"

ListInnerLayout::ListInnerLayout(QWidget* parent, CatItem contextItem) :
        QBoxLayout(QBoxLayout::TopToBottom, 0){
    m_parentWidget = parent;

    m_realItemList = new MyListWidget(parent);
    m_realItemList->setObjectName("alternatives");
    m_realItemList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_realItemList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_realItemList->setTextElideMode(Qt::ElideLeft);
    m_realItemList->setAlternatingRowColors(false);

    m_realItemList->setVisible(true);
    m_realItemList->raise();
    QRect parRect = parent->geometry();
    parRect.moveTopLeft(QPoint(0,0));
    m_realItemList->setGeometry(parRect);


    m_previewPane = new PreviewPane(parent);
    m_previewPane->setVisible(false);
    m_previewing = false;
    m_contextItem = contextItem;

    IconDelegate* listDelegate = new IconDelegate(this,contextItem);
    m_realItemList->setItemDelegate(listDelegate);

    setMargin(0);
    setContentsMargins(0,0,0,0);
    //addWidget(m_realItemList,1,Qt::AlignTop | Qt::AlignCenter);
    addWidget(m_realItemList,1);
}

ListInnerLayout::~ListInnerLayout(){
    if(m_previewPane){
        delete m_previewPane;
    }
    if(m_realItemList){
        m_realItemList->deleteLater();
        m_realItemList = 0;
    }
    qDebug() << "ListWithDisplayHolder deleted";
}


void ListInnerLayout::doUpdate(){
    if(m_realItemList){
        m_realItemList->update();
    }
}

void ListInnerLayout::removeCatItem(CatItem item){
    int num = m_realItemList->count();
    for(int i = 0; i < num; i++) {
        QListWidgetItem * widgetItem = m_realItemList->item(i);
        if(widgetItem && widgetItem->data(ROLE_ITEM_PATH).toString() == item.getPath()){
            widgetItem = m_realItemList->takeItem(i);
            Q_ASSERT(widgetItem);
            delete widgetItem;
        }
    }
}

void ListInnerLayout::setListWidth(int w){
    QRect lG = m_realItemList->geometry();
    lG.moveTopLeft(QPoint(0,0));
    lG.setWidth(w);
    m_realItemList->setGeometry(lG);
    m_realItemList->update();
}

void ListInnerLayout::moveUp(){
    if(!m_previewing){
        m_realItemList->moveUp();
    }
}

void ListInnerLayout::moveDown(){
    if(!m_previewing){
        m_realItemList->moveDown();
    }
}


void ListInnerLayout::setPreview(ListItem it){
    removeWidget(m_realItemList);
    m_realItemList->setVisible(false);
    m_realItemList->lower();
    m_previewing = true;
    Q_ASSERT(m_previewPane);
    Q_ASSERT(m_parentWidget);
    //m_previewPane->setGeometry(QRect(QPoint(0,0), QSize(400,400)));
    //m_previewPane->setMinimumSize(QSize(400,400));
    m_previewPane->setFocusPolicy(Qt::StrongFocus);
    m_previewPane->setFocus();
    m_previewPane->activateWindow();
    m_previewPane->setItem(it);

    m_previewPane->setVisible(true);
    m_previewPane->show();
    m_previewPane->raise();
    //insertWidget(0, m_previewPane, 1);
    clear();
    addWidget(m_previewPane,1);
    update();
    m_previewPane->raise();
    m_previewPane->update();
    m_parentWidget->update();



    //m_previewPane->setWindowOpacity(1);
}

void ListInnerLayout::endPreview(int i){
    if(m_previewPane){
        m_previewPane->setVisible(false);
        m_previewPane->setFocusPolicy(Qt::NoFocus);
    }
    if(!m_previewing ){return;}
    m_previewing = false;

    m_realItemList->setVisible(true);
    m_realItemList->raise();
    m_realItemList->show();
    clear();
    addWidget(m_realItemList, 1);
    if(i>=0){
        m_realItemList->setCurrentRow(i);
    }
}


void ListInnerLayout::clear(){
    int num = m_realItemList->count();
    for(int i = 0; i < num; i++) {
        QListWidgetItem * item = m_realItemList->takeItem(0);
        if (item != 0) {delete item;}
    }
}

void ListInnerLayout::setCurrentRow(int r){
    //m_realItemList->setCurrentRow(r,QItemSelectionModel::Select);,QItemSelectionModel::Current
    m_realItemList->setCurrentRow(r);
}

void ListInnerLayout::setCurrentItem(CatItem item){
    m_realItemList->setCurrentItem(item);
}

QListWidgetItem* ListInnerLayout::getItem(int i){
    return m_realItemList->item( i);
}

int ListInnerLayout::itemCount(){
    return m_realItemList->count();
}

void ListInnerLayout::parentKeyPress(QKeyEvent* key){
    if(!m_previewing){
        m_realItemList->parentKeyPress(key);
    } else {
        m_previewPane->parentKeyPress(key);
    }

}

void ListInnerLayout::parentKeyRelease(QKeyEvent* key){
    if(!m_previewing){
        m_realItemList->parentKeyRelease(key);
    } else {
        m_previewPane->parentKeyRelease(key);
    }
}

int ListInnerLayout::sizeHintForRow(int row){
    return m_realItemList->sizeHintForRow(row);
}

void ListInnerLayout::addItem(ListItem itm){
    endPreview(0);
    m_realItemList->addItem(
            menuItFromListItem(itm, charsWidth()));
}

void ListInnerLayout::updateItem(ListItem catItem){
    int num = m_realItemList->count();
    for(int i = 0; i < num; ++i) {
        QListWidgetItem * widgetItem = m_realItemList->item(i);
        if (widgetItem->data(ROLE_ITEM_PATH) == catItem.getPath()) {
            *widgetItem = *menuItFromListItem(catItem, charsWidth());
            return;
        }
    }
    m_realItemList->addItem(
            menuItFromListItem(catItem, charsWidth()));
}

void ListInnerLayout::addItemList(QList<ListItem> il){

    endPreview(0);
    IconDelegate* listDelegate = new IconDelegate(this);
    if(il.size() > 0){
        listDelegate->setDelegateFont(il[0].getDisplayFont());
    } else {
        listDelegate->setDelegateFont(m_realItemList->font());
    }

    m_realItemList->setItemDelegate(listDelegate);

    int num = m_realItemList->count();
    for(int i = 0; i < num; ++i) {
        QListWidgetItem * item = m_realItemList->takeItem(0);
        if (item != NULL) {delete item;}
    }

    int w = this->charsWidth();
    for(int i=0;i < il.count(); i++){
        ListItem li =il[i];
        li.setCharsAvailable(w);
        m_realItemList->addItem( menuItFromListItem(li, w));
    }
    m_realItemList->raise();
}

//QListWidgetItem* ListWithDisplayHolder::menuItFromListItem(ListItem ir, int charsAllowed){
//
//}

QListWidgetItem* ListInnerLayout::currentItem() const{
    return m_realItemList->currentItem();
}

QListWidgetItem* ListInnerLayout::currentOrHoverItem() const{
    return m_realItemList->currentOrHoverItem();
}

QListWidgetItem* ListInnerLayout::hoverItem() const{
    return m_realItemList->hoverItem();
}

int ListInnerLayout::charsWidth(){
    QFontMetrics fm =  m_realItemList->fontMetrics();
    float avDirCharWidth = fm.width("/mmmm/")/8;
    qDebug() <<  "avDirCharWidth: " << avDirCharWidth;
    qDebug() << "list width" << m_realItemList->geometry().width();
    qDebug() << "parent width" << this->m_parentWidget->geometry().width();
    int w = m_realItemList->geometry().width() -  (UI_STANDARD_ICON_WIDTH +UI_STANDARD_ICON_BORDER);

    int charsAvail = w/ avDirCharWidth;
    qDebug() << "got chars width: " << charsAvail;
    return charsAvail;
}

int ListInnerLayout::charsHeight(){
    QFontMetrics fm =  m_realItemList->fontMetrics();
    float avDirCharHeight = fm.height();
    QRect r = m_realItemList->geometry();
    int rowsAvail = r.height()/avDirCharHeight;
    return rowsAvail;
}

QListWidgetItem * ListInnerLayout::menuItFromListItem(ListItem ir, int charsAllowed){
        QString fullPath = ir.getPath();
        QString adjPath = QDir::toNativeSeparators(fullPath);
        //QFileInfo fileInfo(fullPath);
        QIcon icon = ir.getActualIcon();
        QListWidgetItem * menuItem = new QListWidgetItem(icon,adjPath);
        Q_ASSERT(menuItem);
        qlonglong itemId = ir.getItemId();


//        if(!ir.getPreviewHtml().isEmpty()){
//            QFont font = ir.getDisplayFont();
//            font.setPixelSize(font.pointSize()-2);
//            QFontMetrics fm(font);
//            int textLength = fm.averageCharWidth() *ir.getPreviewHtml().count();
//            if( textLength < gMainWidget->geometry().width()*5){
//                menuItem->setData(ROLE_LONG_BODY_TEXT, ir.getPreviewHtml());
//            }
//        }

        int c = charsAllowed;

        QString formattedPath;
        QString formattedPathWithAlt;
        if(!m_contextItem.isEmpty() && m_contextItem.onlyChildrenRelated() &&
                m_contextItem.hasLabel(FILE_CATALOG_PLUGIN_STR)){
            formattedPath = ir.getAltDescription();
            formattedPathWithAlt = ir.getAltDescription();
        } else {
            formattedPath = ir.getFormattedPath(c,true);
            formattedPathWithAlt = ir.getFormattedPath(c,true, true);
        }
        menuItem->setData(ROLE_FULL, formattedPath);
        menuItem->setData(ROLE_FULL_W_ALT, formattedPathWithAlt);
        QString formattedSummary = summarizeMessage(ir);
        menuItem->setData(ROLE_SUMMARY, formattedSummary);
        menuItem->setData(ROLE_SHORT, ir.getName());
        menuItem->setData(ROLE_FORMATTED_SHORT, ir.formattedName());
        menuItem->setData(ROLE_ICON, icon);
        menuItem->setData(ROLE_ID, itemId);
        menuItem->setData(ROLE_DESCRIPTION, ir.getDescription());
        menuItem->setData(ROLE_MATCH_TYPE, ir.getMatchType());
        menuItem->setData(ROLE_ITEM_PATH, fullPath);
        menuItem->setData(ROLE_ITEM_HEIGHT_FACTOR, 1);
        menuItem->setData(ROLE_ITEM_OFFSET, 0);
        menuItem->setData(ROLE_INTERACTION_TYPE, (int)ir.getInterationType());
        menuItem->setData(ROLE_USE_LONG_NAME, (int)ir.getUseLongName());
        menuItem->setData(ROLE_USE_LONG_DESCRIPTION, (int)ir.getUseLongDescription());
        menuItem->setData(ROLE_LONG_TEXT, ir.getLongText());
        menuItem->setData(ROLE_HAS_CHILDREN, ir.canNavigateTo());
        menuItem->setData(ROLE_ITEM_TAG_LEVEL, (int)ir.getTagLevel());
        QString pinStr = ir.getPinnedString();
        bool pinned = ir.isPinned() && (pinStr.isEmpty() || gMainWidget->userKeys().startsWith(pinStr));
        menuItem->setData(ROLE_ITEM_PINNED, pinned);
        menuItem->setData(ROLE_ITEM_NOT_PINABLE, (ir.getCustomValue(ITEM_NOTPINABLE_KEY)));
        menuItem->setData(ROLE_USE_ICON, (!ir.getIsVerbArg()));
        menuItem->setData(ROLE_ITEM_TYPE, (int)ir.getItemType());
        menuItem->setData(ROLE_HAS_PARENT_ELEMENT, false);
        menuItem->setData(ROLE_TOP_MARGIN, ir.getListMargin());
        QDateTime t;
        int secs = (MAX(ir.getCreationTime(), ir.getModificationTime()));
        t.fromTime_t(secs);
        QString timeStr = contextualTimeString(t);
        menuItem->setData(ROLE_TIME_STRING, timeStr);
        menuItem->setToolTip(ir.getDescription());
        return menuItem;
}
