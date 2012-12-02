#include "expression_icon_window.h"

#define INACTIVE false
#define ACTIVE true

IconWidget::IconWidget(QWidget *parent ): QWidget(parent,Qt::FramelessWindowHint) {
    setAttribute( Qt::WA_TranslucentBackground);
    this->setMinimumSize(QSize(0,0));
    this->setSizeIncrement(1,1);
}

void IconWidget::setIcon(QIcon ic){
    m_icon = ic;
}

QIcon& IconWidget::getIcon(){
    return m_icon;
}

void IconWidget::paintEvent ( QPaintEvent* ){
    QPainter painter(this);
    QBrush b(Qt::darkRed);
    painter.setBackground(b);

    QPixmap pixmap = m_icon.pixmap(size(),QIcon::Normal,QIcon::On);
//    QSize sz = pixmap.size();
//    QRect r = geometry();
    //QRect newR = r;
    //newR.setSize(sz);
    //newR.moveBottomLeft(r.bottomLeft());
    QPointF pf(0,0);
    painter.drawPixmap(pf, pixmap);
}


//---------------------------------------

ExpressionIconWindow::ExpressionIconWindow(QWidget *parent) :
    QWidget(parent,Qt::FramelessWindowHint) ,
    m_smallIconSize(UI_DEFAULT_MINI_ACTION_ICON_SIZE),
    m_largeIconSize(UI_ACTIVE_ICON_SIZE)
{
    setAttribute(
            Qt::WA_TranslucentBackground);
    reset();

}


void ExpressionIconWindow::reset(){
    m_animate = 0;
    m_activeIcon=0;
    m_wantedBeActiveIcon=0;
    setGeometry(geometry());
//    QRect r = this->geometry();
//    r.setWidth(m_largeIconSize + m_smallIconSize + UI_ICON_WINDOW_RIGHT_MARGIN + 20);
//    r.setHeight(m_largeIconSize);
//    //r.setBottomLeft(geometry().bottomLeft());
//    setGeometry(r);

}


ExpressionIconWindow::~ExpressionIconWindow(){
    for(int i=0; i < m_expressionIcons.count();i++){
        if(m_expressionIcons[i]){
            delete m_expressionIcons[i];
        }
    }
    if(m_animate){
        m_animate->stop();
        delete m_animate;
    }
}

int ExpressionIconWindow::expressionIconCount(){
    return m_expressionIcons.length();
}

int ExpressionIconWindow::activeExpressionIcon(){
    if(!m_animate){
        return m_activeIcon;
    } else{
        return m_wantedBeActiveIcon;
    }
}

void ExpressionIconWindow::clearIcons(){
    for(int i=0; i < m_expressionIcons.count();i++){
        if(m_expressionIcons[i]){
            delete m_expressionIcons[i];
        }
    }
    m_expressionIcons.clear();
    reset();
}

void ExpressionIconWindow::setGeometry(QRect r){
    QPoint bottomLeft = r.bottomLeft();
    r.setHeight(MAX(m_largeIconSize,r.height()));
    r.setWidth(m_largeIconSize + m_smallIconSize*(m_expressionIcons.count()-1)+ UI_ICON_WINDOW_RIGHT_MARGIN+10);
    r.setBottomLeft(bottomLeft);
    QWidget::setGeometry(r);
}

float ExpressionIconWindow::getActiveness(){
    return m_activeness;
}

void ExpressionIconWindow::setActiveIconPos(int pos){
    pos = MIN(1,pos);
    m_activeIcon= pos;
    m_wantedBeActiveIcon = 1-m_activeIcon;
    m_activeness=0;
    formatIcons();
}

void ExpressionIconWindow::setIcon( QIcon ic, int pos){
    if(pos < m_expressionIcons.count()){
        m_expressionIcons[pos]->setIcon(ic);
    } else {
        Q_ASSERT(pos <= m_expressionIcons.count());
        appendIcon(ic);
    }
}


void ExpressionIconWindow::animateLeft(){
    if(m_expressionIcons.count()==1){
        m_activeness=0;
        m_activeIcon=0;
        return;
    }

    if(m_activeIcon<=0){
        Q_ASSERT(m_wantedBeActiveIcon >=0);
        Q_ASSERT(m_activeIcon >=0);
        return;
    }

    m_wantedBeActiveIcon = 0;
    if(!m_animate){
        beginAnimation();
    }


//    if(!m_animate){
//        if(m_activeIcon >0){
//            m_wantedBeActiveIcon = m_activeIcon-1;
//            beginAnimation();
//        }
//    } else{
//        if(m_activeIcon < m_wantedBeActiveIcon){
//            m_activeIcon++;
//            m_wantedBeActiveIcon--;
//            Q_ASSERT(m_wantedBeActiveIcon >=0);
//    } else if(m_wantedBeActiveIcon >0){
//            stretchRectToLeft(m_activeIcon,m_smallIconSize);
//            m_wantedBeActiveIcon--;
//            Q_ASSERT(m_wantedBeActiveIcon >=0);
//            m_activeIcon--;
//        }
//    }
}

void ExpressionIconWindow::animateRight(){
    if(m_expressionIcons.count()==1){
        m_activeness=0;
        m_activeIcon=0;
        return;
    }

    m_wantedBeActiveIcon = 1;
    if(!m_animate){
        beginAnimation();
    }

//    if(!m_animate){
//        m_wantedBeActiveIcon = m_activeIcon+1;
//        beginAnimation();
//    } else {
//        if(m_activeIcon < m_wantedBeActiveIcon ) {
//            if( m_wantedBeActiveIcon < m_expressionIcons.count()-1){
//                m_activeIcon++;
//                m_wantedBeActiveIcon++;
//                //stretchRectToRight(m_activeIcon,m_smallIconSize);
//            }
//        } else {
//            m_wantedBeActiveIcon++;
//            m_activeIcon++;
//        }
//    }
}

void ExpressionIconWindow::beginAnimation(){

    m_activeness = 0;
    m_animate = new QPropertyAnimation(this, "activeness");
    m_animate->setDuration(UI_ICONSWITCH_TIME);
    m_animate->setStartValue(0);
    m_animate->setEndValue(1);
    connect(m_animate, SIGNAL(finished()), this,SLOT(animateToActiveDone()));
    m_animate->start();

}

void ExpressionIconWindow::popIcon(){
    if(!m_animate){
        if(m_expressionIcons.last()){
            delete m_expressionIcons.last();
        }
        m_expressionIcons.pop_back();
        m_activeIcon =MIN(m_expressionIcons.count()-1,m_activeIcon);
        m_wantedBeActiveIcon=m_activeIcon;
        setGeometry(geometry());
    } else {
        //Set to delete later...
        QIcon nullIcon;
        m_expressionIcons[m_expressionIcons.length()-1]->setIcon(nullIcon);
    }
}

void ExpressionIconWindow::appendIcon(QIcon pm){
    //Q_ASSERT(m_animate || (m_activeIcon == m_wantedBeActiveIcon));
    IconWidget* l = new IconWidget(this);
    l->setIcon(pm);
    l->setFocusPolicy(Qt::NoFocus);
    l->setVisible(true);
    l->raise();
    l->setMinimumSize(0,0);
    m_expressionIcons.append(l);
    setGeometry(geometry());
    formatIcons();
}



// -->
void ExpressionIconWindow::setActiveness(float a){
    m_activeness = a;
    //resizeIcons();
    formatIcons();
}

float ExpressionIconWindow::adj(bool isActive) {
    if(isActive) {
        return (m_smallIconSize*(1-m_activeness) +
                m_largeIconSize*(m_activeness) );
    } else {
        return (m_smallIconSize*(m_activeness)+
                m_largeIconSize*(1-m_activeness));
    }
}

void ExpressionIconWindow::formatIcons(){
    if(m_expressionIcons.count()>0){
        stretchRectToRight(0, adj(m_wantedBeActiveIcon ==0));
    }
    if(m_expressionIcons.count()>1){
        stretchRectToLeft(1, adj(m_wantedBeActiveIcon ==1));
    }
//    for(int i=0; i< m_expressionIcons.count(); i++){
//        formatIcon(i);
//    }
}


void ExpressionIconWindow::stretchRectToRight(int pos, float amount) {
    //qDebug() << "stretch right" << amount;
    QRect r;
    r.setWidth(amount);
    r.setHeight(amount);
    r.moveBottomLeft(QPoint(m_smallIconSize*(pos),
                            geometry().height()));
    IconWidget* l = m_expressionIcons[pos];
    //qDebug() << "stretchRectToRigh setGeomtry r" << r;
    l->setGeometry(r);
    l->updateGeometry();
}

// <--
void ExpressionIconWindow::stretchRectToLeft(int pos, float amount){

    QRect r;
    r.setWidth(amount);
    r.setHeight(amount);
    r.moveBottomRight(
            QPoint(m_smallIconSize*(pos)+m_largeIconSize,
                   geometry().height())); //m_largeIconSize
    IconWidget* l = m_expressionIcons[pos];
    qDebug() << "stretchRectToLeft setGeomtry r" << r;
    l->setGeometry(r);
    l->updateGeometry();

}



void ExpressionIconWindow::formatIcon(int ){//index
//    if(index < m_activeIcon && index < m_wantedBeActiveIcon){
//        stretchRectToRight(index, m_smallIconSize);
//    } else if(index > m_activeIcon && index > m_wantedBeActiveIcon){
//        stretchRectToLeft(index, m_smallIconSize);
//    } else if(index == m_activeIcon){
//        if(m_activeness==0){
//            stretchRectToRight(index,m_largeIconSize);
//        } else if(m_activeIcon  < m_wantedBeActiveIcon){
//            stretchRectToRight(index,adj(false));
//        } else {
//            stretchRectToLeft(index,adj(false));
//        }
//    } else if( index ==m_wantedBeActiveIcon){
//        if(m_activeIcon  < m_activeIcon){
//            stretchRectToLeft(index,adj(true ));
//        } else {
//            stretchRectToRight(index,adj(true));
//        }
//    }
}



void ExpressionIconWindow::leftFormatIcon(int pos) {
    QRect r;
    r.setWidth(m_smallIconSize);
    r.setHeight(m_smallIconSize);
    r.moveBottomLeft(QPoint(m_smallIconSize*(pos),m_largeIconSize)); //m_largeIconSize
    IconWidget* l = m_expressionIcons[pos];
    qDebug() << "setGeomtry r" << r;
    l->setGeometry(r);
    l->updateGeometry();

}

// <--
void ExpressionIconWindow::rightFormatIcon(int pos){

    QRect r;
    r.setWidth(m_smallIconSize);
    r.setHeight(m_smallIconSize);
    r.moveBottomLeft(
            QPoint(m_smallIconSize*(pos)+m_largeIconSize,
                  m_largeIconSize)); // m_largeIconSize
    IconWidget* l = m_expressionIcons[pos];
    qDebug() << "setGeomtry r" << r;
    l->setGeometry(r);
    l->updateGeometry();

}

void ExpressionIconWindow::resizeIcons(){
    if(m_wantedBeActiveIcon == m_activeIcon+1){
        stretchRectToRight(
                m_activeIcon, adj(0));
        stretchRectToLeft(
                m_wantedBeActiveIcon, adj(1));
    } else if(m_wantedBeActiveIcon == m_activeIcon-1){
        stretchRectToRight(m_wantedBeActiveIcon,
                adj(1));
        stretchRectToLeft(m_activeIcon,
                adj(0));
    }
}


void ExpressionIconWindow::animateToActiveDone(){
    if(m_activeIcon==(m_expressionIcons.length()-1)
        && m_expressionIcons[m_expressionIcons.length()-1]->getIcon().isNull()) {
        popIcon();
    }
    qDebug() << "animateToActiveDone...";
    m_activeIcon = m_wantedBeActiveIcon;
    m_activeness=0;
    delete m_animate;
    m_animate = 0;


}
