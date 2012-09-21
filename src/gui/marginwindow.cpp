
#include "marginwindow.h"
#include "appearance_window.h"
#include "main.h"

MarginWindow::MarginWindow(QWidget *, PlatformBase* plat, bool rescue):
        QWidget(0,Qt::FramelessWindowHint), m_mainWindow(this,plat,rescue),
        m_backgroundImage(UI_BORDER_TEXTURE_FILE)
{
    gMarginWidget = this;
    gMainWidget = &m_mainWindow;
    gMainWidget->setObjectName("main");
    setAttribute(Qt::WA_TranslucentBackground);

    activateWindow();
    show();
    gMainWidget->raise();
}

void MarginWindow::paintEvent(QPaintEvent * /* event */){
    QPainter painter(this);
    //--------------
    FancyEnclosingRect::paintPaths(&painter, m_pPaths);

//    for(int i=0; i<MIN(m_pPaths.count(),1);i++){
//        //painter.fillPath(m_pPaths[i].first, m_pPaths[i].second );
//        painter.save();
//        painter.setClipPath(m_pPaths[i].first);
//        painter.drawPixmap(m_marginRect,m_backgroundImage);
//        painter.restore();
//    }
    //painter.fillPath(m_previewArrow.first, m_previewArrow.second ); //
    painter.setBrush(Qt::transparent);
    QPen p(Qt::darkGray);
    p.setWidthF(1.5);
    p.setCosmetic(true);
    p.setJoinStyle(Qt::RoundJoin);
    p.setCapStyle(Qt::RoundCap);
    painter.setPen(p);
    painter.setRenderHint(QPainter::Antialiasing,true);

    painter.fillPath(m_previewArrowInside,Qt::white);
    painter.drawPath(m_previewArrow.first);

}

void MarginWindow::resizeEvent(QResizeEvent * /* event */)
 {
//    QRect r = rect();
//    r.adjust(10,10,-10,-10);
//    QRegion reg(r);
//    setMask(reg);
 }

QSize MarginWindow::sizeHint()
{
    QRect r = rect();
    r.adjust(10,10,-10,-10);
    return r.size();
}


void MarginWindow::gSetAppPos(int vOrient, int hOrient){
    if(hOrient!=-1){ m_hOrient = hOrient;}
    if(vOrient!=-1){ m_vOrient = vOrient; }
    if(!gMainWidget){ return; }

    QRect scr = gGlobalArea();

    gMainWidget->adjustSize();
    QRect editRect = gMainWidget->geometry();
    editRect.moveTopLeft(QPoint(0,0));
    QRect listRect = listWindowRect();

    QRect outSideRect = editRect;
    outSideRect.setWidth(MAX(outSideRect.width(), listRect.width())
        + UI_BORDER_WINDOW_LEFT_MARGIN + UI_BORDER_WINDOW_RIGHT_MARGIN);

    int invisibleMargin = gMainWidget->invisibleAreaHeight();

    outSideRect.setWidth((outSideRect.width()/2)*2);

    listRect.setWidth(outSideRect.width()-(
            UI_BORDER_WINDOW_LEFT_MARGIN+UI_BORDER_WINDOW_RIGHT_MARGIN));

    //Margin for sub-windows
    listRect.setWidth((listRect.width()/2)*2);

    m_marginRect = outSideRect;
    m_marginRect.setWidth(editRect.width() + UI_BORDER_WINDOW_LEFT_MARGIN + UI_BORDER_WINDOW_RIGHT_MARGIN);
    if(!listRect.isEmpty()){
        outSideRect.setHeight((editRect.height() + listRect.height()) - invisibleMargin);
        m_marginRect.moveTopLeft(QPoint(0,0));
        m_marginRect.setHeight(outSideRect.height());;
    } else {
        m_marginRect.moveTopLeft(QPoint(0,invisibleMargin));
    }

    QRect previewRect = previewWindowRect();

    if(m_vOrient ==0){
        outSideRect.moveTop(scr.top());
        editRect.moveTop(0);
        listRect.moveTop(editRect.bottom());
        previewRect.setBottom(outSideRect.height() - UI_BORDER_WINDOW_BOTTOM_MARGIN);
    } else if(m_vOrient ==1){
        //move BOTH vertical **AND** horizontal - we're OK with this
        outSideRect.moveCenter(scr.center());
        editRect.moveCenter(QPoint(outSideRect.width()/2,outSideRect.height()/2));
        listRect.moveBottom(editRect.top());
        previewRect.setBottom(outSideRect.height()/2 + previewRect.height()/2);
    } else {
        Q_ASSERT(m_vOrient ==2);
        outSideRect.moveBottom(scr.bottom());
        editRect.moveBottom(outSideRect.height());
        listRect.moveBottom(editRect.top() + invisibleMargin);
        previewRect.moveTop(UI_BORDER_WINDOW_TOP_MARGIN);
    }

    if(m_hOrient == 0){
        outSideRect.moveLeft(scr.left());
    } else if(m_hOrient == 1) {
        outSideRect.moveLeft((scr.center().x() - editRect.width()/2) - UI_BORDER_WINDOW_LEFT_MARGIN);
    } else {
        Q_ASSERT(m_hOrient==2);
        outSideRect.moveRight(scr.right());
    }

    editRect.moveLeft(UI_BORDER_WINDOW_LEFT_MARGIN);
    listRect.moveLeft(UI_BORDER_WINDOW_LEFT_MARGIN);
    //listRect.moveLeft((m_marginRect.width() - listRect.width())/2);
    listRect.setWidth(m_marginRect.width()-(
            UI_BORDER_WINDOW_LEFT_MARGIN+UI_BORDER_WINDOW_RIGHT_MARGIN));

    if(outSideRect.right() > scr.right()){
        outSideRect.moveRight(scr.right());
    }
    if(!previewRect.isEmpty()){
        outSideRect.setRight(MAX(outSideRect.right() + previewRect.width(),
        outSideRect.left()  + UI_BORDER_WINDOW_LEFT_MARGIN +
            listRect.width() + previewRect.width()));
        previewRect.moveLeft(listRect.right()+UI_BORDER_WINDOW_LEFT_MARGIN+UI_PREVIEW_RIGHT_OFFSET);
    }

    if(outSideRect.right() > scr.right()){
        outSideRect.setRight(scr.right());
        previewRect.setRight(scr.right());
    }

    move(outSideRect.topLeft());


    setGeometry(outSideRect);
    if(!gMainWidget){return;}
    gMainWidget->move(editRect.topLeft());
    if(gMainWidget->m_itemChoiceList!=0){
        gMainWidget->m_itemChoiceList->raise();
        gMainWidget->m_itemChoiceList->setGeometry(listRect);
        if(gMainWidget->m_itemChoiceList->m_previewFrame && previewRect.height()>0){
            gMainWidget->m_itemChoiceList->m_previewFrame->setGeometry(previewRect);
            gMainWidget->m_itemChoiceList->m_previewFrame->raise();
        }
    }
    gMainWidget->raise();
    m_pPaths = FancyEnclosingRect::createStyledRects(m_marginRect,BORDER_WIND_NAME,false);
    createPreviewArrow(previewRect);
    //update();
}

void MarginWindow::createPreviewArrow(QRect previewRect){
    QRect focusRect = gMainWidget->focusRect();

    if(previewRect.isEmpty()){
        m_previewArrow = QPair < QPainterPath,QBrush >();
        return;
    }

    qreal rightHeight = MIN( previewRect.height()*(UI_PREVIEW_ARROW_RIGHT_RATIO), focusRect.height()*3);
    rightHeight = MAX( rightHeight, 20);

    QRect left = focusRect;
    int leftBorder = m_marginRect.right()-UI_BORDER_WINDOW_RIGHT_MARGIN*2;// left.right();
    int leftCenter = left.center().y();

    QPointF centerLeft(leftBorder, leftCenter);

    QRect rightRect = previewRect;
    rightRect.adjust(-1,-1,1,1);
    int rightBorder = rightRect.left();
    //int rightCenter = right.center().y();

    QPointF topRight(rightBorder, leftCenter-rightHeight/2);
    QPointF bottomRight(rightBorder, leftCenter+rightHeight/2);

    QPainterPath path;
    QPainterPath littlePath;
    path.moveTo(centerLeft);
    littlePath.moveTo(centerLeft);
    path.lineTo(topRight);
    littlePath.lineTo(topRight);
    if(gMainWidget->m_itemChoiceList &&
       gMainWidget->m_itemChoiceList->m_previewFrame &&
        ((gMainWidget->m_itemChoiceList->m_previewFrame->m_layout->m_previewPane
            && gMainWidget->m_itemChoiceList->m_previewFrame->m_layout->m_previewPane->isVisible()) ||
        (gMainWidget->m_itemChoiceList->m_previewFrame->m_layout->m_realItemList &&
            gMainWidget->m_itemChoiceList->m_previewFrame->m_layout->m_realItemList->isVisible()))){
        path.lineTo(rightRect.topLeft());
        path.lineTo(rightRect.topRight());
        path.lineTo(rightRect.bottomRight());
        path.lineTo(rightRect.bottomLeft());
    }
    path.lineTo(bottomRight);
    littlePath.lineTo(bottomRight);
    path.lineTo(centerLeft);
    littlePath.lineTo(centerLeft);
    path.closeSubpath();
    littlePath.closeSubpath();
    m_previewArrowInside = littlePath;
    m_previewArrow.first = path;
    QBrush b(Qt::black);
    m_previewArrow.second = b;

}

QWidget* MarginWindow::mainUserWindow(){
    return gMainWidget;
}

QRect MarginWindow::listWindowRect(){
    Q_ASSERT(gMainWidget);
    return gMainWidget->listRect();
}

QRect MarginWindow::previewWindowRect(){
    Q_ASSERT(gMainWidget);
    return gMainWidget->previewRect();

}

