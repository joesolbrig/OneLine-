#include "listwithdisplay.h"
#include "edit_controls.h"
#include "fancyenclosingrect.h"

ListWithFrame::ListWithFrame(QWidget* parent,CatItem contextItem)
        : QWidget((parent==0 ? ((QWidget*)gMarginWidget): parent)){
    if(parent){
        m_floatingAtSide = false;
        QRect r = parent->geometry();
        QRect tr = geometry();
        tr.setWidth(r.width());
        QWidget::setGeometry(tr);
    } else {
        //Qt::FramelessWindowHint | Qt::Tool
        m_floatingAtSide = true;
        setWindowFlags(Qt::FramelessWindowHint );
    }
    setAttribute(Qt::WA_TranslucentBackground);
    m_layout = new ListInnerLayout(this, contextItem);
    m_faderWidget = 0;
    m_hovered = false;
    m_lastHover = 0;
    setLayout(m_layout);
    update();
}

ListWithFrame::~ListWithFrame(){
    //m_layout->clear();
    qDebug() << "ListWithFrame deleted";
    delete m_layout;
    m_layout = 0;
}

void ListWithFrame::paintEvent ( QPaintEvent * event ){
    QWidget::paintEvent(event);

//    if(atSidePreview()){
//        QPainter painter(this);
//        QRect extent = geometry();
//        extent.setTopLeft(QPoint(0,0));
//
//        QPen pen;
//        pen.setColor(Qt::black);
//        pen.setWidth(1);
//        extent.adjust(1,1,-1,-1);
//        painter.save();
//        painter.setPen(pen);
//        painter.drawRect(extent);
//        painter.restore();
//        //FancyEnclosingRect::drawStyledRects(&painter,extent,MAIN_LIST_NAME);
//    }

}


void ListWithFrame::adjustSizeToContentsSize(){

    int oldHeight = geometry().height();
    int viewHeight = oldHeight;

    if(getPreviewUrl().isEmpty()){
        int viewHeight = m_layout->getRealList()->itemsHeight();//viewport()->geometry().height();
        viewHeight = MAX(5, MIN(oldHeight, viewHeight));
    } else if(m_layout->m_previewPane) {
        viewHeight = m_layout->m_previewPane->height();//viewport()->geometry().height();
        viewHeight = MIN(1000, MAX(oldHeight, viewHeight));
    }
    resize(width(),viewHeight);

}


void ListWithFrame::doUpdate(){
    if(m_layout){
        m_layout->doUpdate();
    }
    QWidget::update();
}

void ListWithFrame::moveUp(){
    m_layout->moveUp();
}

void ListWithFrame::moveDown(){
    m_layout->moveDown();
}


QListWidgetItem * ListWithFrame::currentItem() const{
    return m_layout->currentItem();
}

QListWidgetItem * ListWithFrame::currentOrHoverItem() const{
    return m_layout->currentOrHoverItem();
}

QListWidgetItem * ListWithFrame::hoverItem() const{
    return m_layout->hoverItem();
}


void ListWithFrame::updateItem(ListItem itm){
    m_layout->updateItem(itm);
}

void ListWithFrame::removeItem(CatItem item){
    //TODO:
    m_layout->removeCatItem(item);
}

void ListWithFrame::setFadedness(double fadedness){
    setWindowOpacity(fadedness);
    //m_layout->m_previewPane->raise();

}



void ListWithFrame::addItemList(QList<ListItem> il){
    raise();
    setVisible(true);
    m_layout->addItemList(il);
    updateGeometry();
}

void ListWithFrame::setPreviewSearch(QString searchStr){
    m_layout->m_previewPane->searchForString(searchStr);

}


void ListWithFrame::setPreviewItem(ListItem li){
    m_layout->setPreview(li);
    m_layout->update();
}

void ListWithFrame::endPreview(){
    m_layout->endPreview(-1);
}

void ListWithFrame::setContextItem(CatItem contextItem){
    m_layout->m_contextItem = contextItem;
}

QString ListWithFrame::getPreviewUrl(){
    if(m_layout && m_layout->m_previewPane)
        {return m_layout->m_previewPane->url().toString();}
    return QString();
}

bool ListWithFrame::atSidePreview(){
    return m_floatingAtSide;
}


void ListWithFrame::parentKeyPress(QKeyEvent* key){
    Q_ASSERT(m_layout);
    m_layout->parentKeyPress(key);
}

void ListWithFrame::parentKeyRelease(QKeyEvent* key){
    Q_ASSERT(m_layout);
    m_layout->parentKeyRelease(key);
}

QListWidgetItem* ListWithFrame::getItem(int i){
    return m_layout->getItem( i);
}


int ListWithFrame::count(){
    return m_layout->itemCount();
}

void ListWithFrame::clear(){
    if(m_layout){
        m_layout->clear();
    }
}

int ListWithFrame::sizeHintForRow(int row){
    return m_layout->sizeHintForRow(row);
}

int ListWithFrame::getCurrentRow(){
    if(m_layout->getRealList()){
        return m_layout->getRealList()->currentRow();
    } else { return -1;}
}

int ListWithFrame::getRowCount(){
    if(m_layout->getRealList()){
        return m_layout->getRealList()->count();
    } else { return 0;}
}

QRect ListWithFrame::getSelectedRect(){
//    QListWidgetItem* indexItem  = m_layout->getRealList()-> item(getCurrentRow());
//    return m_layout->getRealList()->visualItemRect(indexItem);
    int i = getCurrentRow();
    if(i>=0){
        return m_layout->getRealList()->enclosingRect(i);
    } else { return QRect();}
}

void ListWithFrame::setCurrentRow(int r){
    m_layout->setCurrentRow(r);
}

void ListWithFrame::setCurrentItem(CatItem item){
    m_layout->setCurrentItem(item);
}

//void ListWithFrame::addBottomMessage(QString msg){
//    if(m_layout ){
//        m_layout->addNavigationMessage(msg);
//    }
//}

//void setGmtry(QRect r);
void ListWithFrame::setGeometry(QRect r){
    if(r.isEmpty()){ r = geometry(); }

    QWidget::setGeometry(r);
    m_layout->setListWidth(r.width());
    QRect g = r;
    //QPoint globPos = QWidget::mapToGlobal(r.topLeft());
    g.moveTopLeft(QPoint(0,0));
    if(m_layout->m_previewPane ){
        m_layout->m_previewPane->setGeometry(g);
        if(m_layout->m_previewPane->isVisible()){
            m_layout->m_previewPane->raise();
        }
    }

//    if(m_faderWidget){
//        m_faderWidget->setGeometry(g);
//        m_faderWidget->raise();
//    }
    updateGeometry();

}

