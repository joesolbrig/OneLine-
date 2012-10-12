#include <QIcon>
#include <QPainter>
#include "constants.h"
#include "icon_delegate.h"
#include "listwithdisplay.h"
#include "edit_controls.h"
#include "appearance_window.h"
#include "multiinputdisplayer.h"
#include "fancyenclosingrect.h"
#include "main.h"


ListWithDisplay::ListWithDisplay()
        : QWidget((QWidget*)gMarginWidget, Qt::FramelessWindowHint ){
    m_previewFrame = 0;
    m_multiInputDisplayer = 0;
    m_miniIconAnimation = 0;
    m_newFrame=0;
    m_miniIconBar = new TextMessageBar(this);
    addViewRef();
    currentViewRef() = new ListWithFrame(this);
    QRect innerR = this->geometry();
    innerR.moveTopLeft(QPoint(0,0));
    currentViewRef()->setGeometry(innerR);
    currentViewRef()->setVisible(true);
    currentViewRef()->raise();
    //m_upDownAnimation = new QPropertyAnimation(this, "animatedHeight");
    m_upDownAnimation = new QPropertyAnimation(this, "baseGeometry");
    connect(m_upDownAnimation, SIGNAL(finished()), this,SLOT(upDownAnimationDone()));
    m_currentlyShowing = false;

    m_timeOutsideBarArea =0;
    setFocusPolicy(Qt::StrongFocus);
    //m_miniIconBar = &m_iconBarOnStack;
    QRect iconBarR = geometry();
    if(currentViewRef()){
        iconBarR = currentViewRef()->geometry();
    }
    iconBarR.setHeight(UI_MINI_ICON_BAR_HEIGHT);
    iconBarR.setTopLeft(QPoint(0,0));
    iconBarR.moveBottom(0);
    m_miniIconBar->setGeometry(iconBarR);
    m_miniIconBar->hide();
    m_autoHideIconBarPosition = 0;
    setMouseTracking(true);
    m_showIconList = true;
    //m_borderWindow = 0;//new BorderWindow();
//    m_borderWindow->lower();
//    m_borderWindow->hide();
    //setBorderPosition();

    m_formHeight = 0;
    setVisible(false);
}

ListWithDisplay::~ListWithDisplay(){
    if(m_miniIconBar){
        delete m_miniIconBar;
    }
}
void ListWithDisplay::addMessage(QString msg){
    m_bottomMessage = msg;
    if(m_miniIconBar){
        m_miniIconBar->addNavText(msg);
    }
}

int ListWithDisplay::getMessageSpaceAvailable(){
    if(m_miniIconBar){
        return m_miniIconBar->getMessageSpaceAvailable();
    } else {
        return 0;
    }
}

int ListWithDisplay::getMessageItemCount(){
    if(m_miniIconBar){
        return m_miniIconBar->getTextButtonCount();
    } else {
        return 0;
    }
}




bool ListWithDisplay::setMultiInput(InputList* il){
    if(!m_multiInputDisplayer){
        m_multiInputDisplayer = new FormDisplayer(il,m_width);
        m_multiInputDisplayer->setChoiceList(baseViewRef());
        raise();
        Q_ASSERT(m_viewStack.length()==1);
        currentViewRef()->hide();;
        m_miniIconBar->hide();
        return true;
    }
    return false;
}

bool ListWithDisplay::endMultiInput(){
    if(m_multiInputDisplayer){
        delete m_multiInputDisplayer;
        m_multiInputDisplayer = 0;
        currentViewRef()->show();
        return true;
    }
    return false;
}
//////-------------------------------------------------

void ListWithDisplay::setHeight(int h){
    qDebug() << "setting height: " << h;
    m_height = h;
    setBaseGeometry();
}

int ListWithDisplay::getItemCount(){
    return currentViewRef()->count();
}

QRect ListWithDisplay::getFocusRect(){
    Q_ASSERT(m_miniIconBar);
    QRect r = currentViewRef()->getSelectedRect();
    if(m_miniIconBar->isVisible()){
        r.moveTop(r.top() + m_miniIconBar->geometry().bottom());
    }
    return r;
}


void ListWithDisplay::setBaseAndOrientation(QPoint base, bool isTopOriented, int width, int screenEdge){
    m_isTopOriented = isTopOriented;
    m_base = base;
    m_width = width;
    m_screenEdge = screenEdge;
    QSize s(width,m_height);
    QRect g = QWidget::geometry();
    g.setSize(s);
    setBaseGeometry(g);
    move(m_base);
    update();
}

void ListWithDisplay::setAnimatedHeight(QRect r){
    int height = r.height();
    int topPosition=0;
    if(!m_isTopOriented)
        { topPosition = m_height - height; }
    QRect area = geometry();
    area.setHeight(height);

    //Adjust for Icon bar
    if(m_miniIconBar && m_miniIconBar->isVisible()){
        QRect iconBarRect(
            QPoint(area.right(),topPosition),
            QSize(m_width, UI_MINI_ICON_BAR_HEIGHT));
        m_miniIconBar->setGeometry(iconBarRect);
        topPosition +=UI_MINI_ICON_BAR_HEIGHT;
        Q_ASSERT(m_miniIconBar->geometry().height() == UI_MINI_ICON_BAR_HEIGHT);
    }
    area.moveTop(topPosition);

    //Get total Width
    int minWidth=0;
    for(int i=0; i< m_viewStack.count();i++){
        QRect r = m_viewStack[i]->geometry();
        minWidth += r.width();
        r.setTop(topPosition + UI_MINI_ICON_BAR_HEIGHT);
        r.setHeight(height -UI_MINI_ICON_BAR_HEIGHT);
        m_viewStack[i]->setGeometry(r);
    }
    if(minWidth>0){
        m_width = minWidth;
    }
    area.setWidth(m_width);

    //Adjust for multi-input and adjust multi-input
    if(m_multiInputDisplayer){
        m_formHeight = m_multiInputDisplayer->sizeOrPaint(0,height).height();
        if(m_miniIconBar && m_miniIconBar->isVisible()){
            m_formHeight += m_miniIconBar->geometry().height();
        }
        area.setHeight(m_formHeight);
    }

    //Adjust for screen size...
    if(area.right() > m_screenEdge){
        area.moveRight(m_screenEdge);
    }

    QWidget::setGeometry(area);
}

void ListWithDisplay::setBaseGeometry(QRect area){
    if(area.isEmpty()){
        area = geometry();
        area.setHeight(m_height);
        area.setWidth(m_width);
    }
    if(currentViewRef()){
        currentViewRef()->resize(m_width, m_height);
    }

    if(m_multiInputDisplayer){
        m_formHeight = m_multiInputDisplayer->sizeOrPaint(0,area.height()).height();
        if(m_miniIconBar && m_miniIconBar->isVisible()){
            m_formHeight += m_miniIconBar->geometry().height();
        }
        area.setHeight(m_formHeight);
    } else {
        QRect innerFrameRect;
        innerFrameRect.setTopLeft(QPoint(0,0));
        innerFrameRect.setHeight(m_height);
        innerFrameRect.setWidth(m_width);
        if(m_miniIconBar && m_miniIconBar->isVisible()){
            //QMutexLocker locker(&m_previewMutex);
            Q_ASSERT(m_miniIconBar->geometry().height() == UI_MINI_ICON_BAR_HEIGHT);
            innerFrameRect.setTop(innerFrameRect.top() + m_miniIconBar->geometry().height());
            m_miniIconBar->show();
            m_miniIconBar->raise();
        }
        baseViewRef()->setGeometry(innerFrameRect);
        //'cause the inside
        m_width = MAX(m_width, baseViewRef()->geometry().width());
    }
    QRect currentRect = area;
    int margin = m_baseGeometry.right();
    for(int i=0; i< m_viewStack.count(); i++){
        margin+= m_viewStack[i]->geometry().width();
    }

    if(m_previewFrame){
        QRect previewRect = m_previewFrame->geometry();
        previewRect.setTop(geometry().top());
        previewRect.setHeight(m_height);
        previewRect.setWidth(m_width);
        //m_previewFrame->setGeometry(previewRect);
        m_previewFrame->adjustSizeToContentsSize();
    }
    currentRect.setWidth(MAX(currentRect.width(),margin));
    if(currentRect.right() > m_screenEdge){
        currentRect.moveRight(m_screenEdge);
    }

    gMarginWidget->gSetAppPos();
    //QWidget::setGeometry(currentRect);
    //setBorderPosition();
    if(!m_multiInputDisplayer && currentViewRef()){
        currentViewRef()->m_layout->getRealList()->updateDisplay();
    }

}


void ListWithDisplay::paintEvent ( QPaintEvent * event ){

    QPainter painter(this);
    QRect extent = geometry();
    extent.setTopLeft(QPoint(0,0));

    QWidget::paintEvent(event);
    if(m_multiInputDisplayer){
        QWidget::paintEvent(event);
        painter.fillRect(extent,Qt::lightGray);
        int height = m_miniIconBar->geometry().height();
        m_multiInputDisplayer->sizeOrPaint(&painter,-1, height);
        event->accept();
        //List is painted by message to children...
    }
}

void ListWithDisplay::addMiniIconList(QList<ListItem> items){
    if(!m_miniIconBar){ return; }

    m_miniIconBar->setVisible(true);
    if(items.count()>0){
        m_showIconList = true;
        m_miniIconBar->addItems(items);
        QRect r = m_miniIconBar->geometry();
        r.moveTopLeft(QPoint(0,0));
        r.setHeight(UI_MINI_ICON_BAR_HEIGHT);
        r.setWidth(m_width+4); //margin to prevent an overlying "bleed"
        m_miniIconBar->setGeometry(r);
        m_miniIconBar->raise();
        m_miniIconBar->update();
    } else {
        m_showIconList = true;
        //m_miniIconBar->setVisible(false);
        m_miniIconBar->addItems(items);
    }
    setBaseGeometry();
}

void ListWithDisplay::hideIconList(){
    m_miniIconBar->setVisible(false);
    m_showIconList = false;
    setBaseGeometry();
}


void ListWithDisplay::addSearchList(QList<ListItem> il){
    //customVerbAdjust();
    currentViewRef()->addItemList(il);
    currentViewRef()->setCurrentRow(0);
}

QString ListWithDisplay::currentPreviewUrl(){
    return this->currentViewRef()->getPreviewUrl();

}

void ListWithDisplay::addPreviewWindow(ListItem li, AbstractReceiverWidget* w){
    //m_selectedItem = newSelection;
    currentViewRef()->setPreviewItem(li);
    currentViewRef()->m_layout->m_previewPane->hookURLChange(w);;

}

void ListWithDisplay::endPreview(){
    //m_selectedItem = newSelection;
    currentViewRef()->endPreview();;

}

void ListWithDisplay::setContextItem(CatItem contextItem){
    //m_selectedItem = newSelection;
    currentViewRef()->setContextItem(contextItem);

}

void ListWithDisplay::addChildList(QList<ListItem> il, int newSelection, CatItem contextItem){
    m_selectedItem = newSelection;
    m_newFrame = new ListWithFrame(this, contextItem);
    currentViewRef()->lower();
    m_newFrame->raise();
    m_newFrame->setVisible(true);

    QRect g = currentViewRef()->geometry();
    g.moveLeft(g.right());
    m_newFrame->setGeometry(g);
    m_newFrame->addItemList(il);

    FrameAnimation* an = new FrameAnimation(m_newFrame, currentViewRef(), this, FrameAnimation::BeginLeft);
    an->startAnimation();
}

void ListWithDisplay::updateItem(ListItem item){
    currentViewRef()->updateItem(item);
}

//Same operation, animated differently...
void ListWithDisplay::addParentList(QList<ListItem> il, int newSelection, CatItem contextItem){
    if(m_viewStack.length()>1){
        popSideExplore();
    }

    m_selectedItem = newSelection;
    m_newFrame = new ListWithFrame(this,contextItem);
    m_newFrame->raise();
    m_newFrame->setVisible(true);

    QRect g = currentViewRef()->geometry();
    g.setWidth(m_width);
    g.moveRight(g.left());
    m_newFrame->setGeometry(g);
    m_newFrame->addItemList(il);


    m_newFrame->setVisible(true);
    FrameAnimation* an = new FrameAnimation(m_newFrame, currentViewRef(), this, FrameAnimation::BeginRight);
    m_newFrame->raise();
    an->startAnimation();
}

bool ListWithDisplay::testIf_I_CanPreviewItem(CatItem ci){
    ListItem li(ci);
    return (currentViewRef()->m_layout->m_previewPane->setItem(li,true));
}

void ListWithDisplay::trySetPreviewItem(ListItem li){
    if(currentViewRef()->m_layout->m_previewPane->setItem(li,true)){
        addSidePreview(li);
    }
}

void ListWithDisplay::trySetPreviewItem(ListItem contextItem, QList<ListItem> il){
    //m_selectedItem = newSelection;
    addSidePreview(contextItem,il);
}


void ListWithDisplay::addSidePreview(ListItem it){
    if(m_previewFrame){
        QMutexLocker locker(&m_previewMutex);
        m_previewFrame->hide();
        m_previewFrame->lower();
        m_previewFrame->deleteLater();
        m_previewFrame= 0;
    }
    m_previewFrame = new ListWithFrame();

    //Must be done first...
    QRect g = currentViewRef()->geometry();
    QRect gm = this->geometry();
    QPoint localTopRight = gm.topRight();
    //QPoint globTopRight = QWidget::mapToGlobal(localTopRight);
    QPoint globTopRight = (localTopRight);
    g.moveTopLeft(globTopRight);
    m_previewFrame->move(globTopRight);
    m_previewFrame->resize(g.size());

    m_previewFrame->setPreviewItem(it);
    //m_previewFrame->show();
    m_previewFrame->raise();
    //m_previewFrame->adjustSize();
    //m_previewFrame->activateWindow();
    //m_previewFrame->fadeIn();
    //setBaseGeometry();
    //m_previewFrame->hide();
}


void ListWithDisplay::addSidePreview(ListItem contextItem, QList<ListItem> il){
    if(m_previewFrame){
        QMutexLocker locker(&m_previewMutex);
        m_previewFrame->deleteLater();
        m_previewFrame=0;
    }
    m_previewFrame = new ListWithFrame(0, contextItem);

    QRect g = currentViewRef()->geometry();
    QRect gm = this->geometry();
    QPoint localTopRight = gm.topRight();
    //QPoint globTopRight = QWidget::mapToGlobal(localTopRight);
    QPoint globTopRight = (localTopRight);

    g.moveTopLeft(globTopRight);
    m_previewFrame->move(globTopRight);
    m_previewFrame->resize(g.size());
    m_previewFrame->adjustSize();
    m_previewFrame->addItemList(il);
    m_previewFrame->fadeIn();
    m_previewFrame->show();;
    setBaseGeometry();
}

void ListWithDisplay::setPreviewSearch(QString searchStr){
    QMutexLocker locker(&m_previewMutex);
    if(currentViewRef()){
        currentViewRef()->setPreviewSearch(searchStr);
    }
}

void ListWithDisplay::previewLoaded(){
    QMutexLocker locker(&m_previewMutex);
    if(m_previewFrame){
        m_previewFrame->setVisible(true);
        m_previewFrame->fadeIn();
        m_previewFrame->show();;
    }
    setBaseGeometry();
}

void ListWithDisplay::removeSidePreview(){
    QMutexLocker locker(&m_previewMutex);
    if(m_previewFrame){
        m_previewFrame->deleteLater();
        m_previewFrame = 0;
        setBaseGeometry();
    }
}

void ListWithDisplay::popSidePreview(){
    QMutexLocker locker(&m_previewMutex);
    m_previewFrame->deleteLater();
    m_previewFrame = 0;
    setBaseGeometry();
}

void ListWithDisplay::addSideExplore(QList<ListItem> il, int select){
    m_selectedItem = select;
    ListWithFrame* f= new ListWithFrame(this);
    f->raise();
    f->setVisible(true);
    f->addItemList(il);

    QRect g = currentViewRef()->geometry();
    g.moveLeft(g.right());
    f->setGeometry(g);
    m_viewStack.append(f);
    setBaseGeometry();
}

void ListWithDisplay::popSideExplore(){
    Q_ASSERT(!m_viewStack.isEmpty());
    ListWithFrame* f = m_viewStack.last();
    m_viewStack.pop_back();
    f->deleteLater();
    setBaseGeometry();
}

void ListWithDisplay::collapseSideExplore(){
    Q_ASSERT(!m_viewStack.isEmpty());
    ListWithFrame* f = m_viewStack.last();
    QRect g = f->geometry();
    g.setTopLeft(QPoint(0,0));
    f->setGeometry(g);
    for(int i=m_viewStack.count()-2; i>=0; i--){
        ListWithFrame* frm = m_viewStack[i];
        frm->deleteLater();
    }
    m_viewStack.clear();
    m_viewStack.append(f);
    setBaseGeometry();
}

void ListWithDisplay::sideAnimationDone(){
    if(!m_newFrame){ return;} //triage for double-done animation

    ListWithFrame * tempFrame = currentViewRef();
    currentViewRef() = m_newFrame;
    currentViewRef()->setVisible(true);
    if(m_selectedItem>=0){
        currentViewRef()->setCurrentRow(m_selectedItem);
    }
    Q_ASSERT(tempFrame);

    tempFrame->setVisible(false);
    tempFrame->deleteLater();
    if(m_multiInputDisplayer){
        m_multiInputDisplayer->setChoiceList(baseViewRef());
    }
    m_newFrame = 0;

    setBaseGeometry();

    //Must be last
    currentViewRef()->raise();
}

void ListWithDisplay::showAnimated(){
    //if(m_shouldShow){ return; }

    QRect newRect = this->geometry();

    if(m_multiInputDisplayer){
        newRect.setSize(m_multiInputDisplayer->sizeOrPaint());
        if(m_miniIconBar){
            newRect.setHeight(newRect.height()
                + m_miniIconBar->geometry().height());
        }

    } else {
        newRect.setWidth(m_width);
        newRect.setHeight(m_height);
        QRect childRect = currentViewRef()->geometry();
        childRect.setWidth(m_width);
        childRect.setHeight(m_height);
        currentViewRef()->setGeometry(childRect);
    }

    QRect flat = newRect;
    flat.setHeight(1);

    if(m_isTopOriented){
        newRect.moveTopLeft(m_base);
        flat.moveTopLeft(m_base);
    } else {
        newRect.moveBottomLeft(m_base);
        flat.moveBottomLeft(m_base);
    }

    if(!m_currentlyShowing){
        QWidget::setGeometry(flat);
        m_currentlyShowing = true;
    }
    setVisible(true);

    if(newRect!=this->geometry() ){
        animateToLocation(newRect);
    }
}

void ListWithDisplay::animateToLocation(QRect r){
    qDebug() << "animateToLocation begin" << r;
    m_upDownAnimation->setDuration(UI_LIST_RESIZE_TIME);
    int startHeight = geometry().height();
    if(r.height() == startHeight && r.width() == m_width){
        return;
    }

    //resizes main windows
    //setHeight(startHeight);
    m_width = r.width();
    setBaseGeometry();
    m_upDownAnimation->setStartValue(geometry());
    m_upDownAnimation->setEndValue(r);
    m_upDownAnimation->start();
}




//Called by child in kludge...
//void ListWithDisplay::fakeMouseMoveEvent( QMouseEvent * event ){
//    if(!UI_MINI_ICON_ON_MAINFRM){ return;}
//    Q_ASSERT(m_miniIconBar);
//    QPoint pos = event->pos();
//    event->ignore();
//
//    time_t now = appGlobalTime();
//
//    if(pos.y() <= UI_MINI_ICON_BAR_HEIGHT && m_showIconList){
//        if(m_miniIconAnimation){
//            return;
//        }
//        //int desirePosition = m_autoHideIconBar->desiredHeight();
//        int desirePosition = UI_MINI_ICON_BAR_HEIGHT;
//        m_miniIconBar->show();
//        m_miniIconBar->raise();
//        if(getMiniBarPosition()>desirePosition-0.1){
//            return;
//        }
//        //if(now - m_timeInsidBarArea > )
//        showMiniIconBar(desirePosition);
//
//    } else {
//        if(m_timeOutsideBarArea==0){
//            m_timeOutsideBarArea = now;
//            return;
//        }
//        if(getMiniBarPosition()==0){
//            return;
//        }
//        if(true){ //now - m_timeOutsideBarArea > STANDARD_MOMENT*2
//            fadeMiniIconBar();
//        }
//    }
//}

void ListWithDisplay::fakeMouseMoveEvent( QMouseEvent * ){

}



