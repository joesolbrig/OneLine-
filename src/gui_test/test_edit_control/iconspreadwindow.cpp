#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include "individualIcon.h"
#include "iconspreadwindow.h"


IconSpreadWindow::IconSpreadWindow(QWidget* parent):
        QWidget(parent), m_itemsPerCell(DEFAULT_ITEMS_PER_CELL), m_iconPositioner(this->rect(), m_itemsPerCell){
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);

    m_itemsPerCell = DEFAULT_ITEMS_PER_CELL;
    setEnabled(true);
    setVisible(true);
    m_IconSpreadAnimation = new IconSpreadAnimation();
    m_selectedItemIndex = 0;
    Q_ASSERT(parent);



}

void IconSpreadWindow::addItem(ListItem li){
    IndividualIcon* ii = new IndividualIcon(this,li);
    IndividIconAnimation* ia = new IndividIconAnimation(ii);
    m_IconSpreadAnimation->insertAnimation(0, ia);
}

void IconSpreadWindow::startAnimation(){
    for(int i=0; i<m_icons.count();i++){
        IndividIconAnimation* ia = new IndividIconAnimation(m_icons[i]);
        m_IconSpreadAnimation->addAnimation(ia);
    }
    m_IconSpreadAnimation->beginAnimation();
}


void IconSpreadWindow::removeItem(ListItem li){
    int j=-1;
    for(int i =0; i < m_icons.count(); i++){
        if(m_icons[i]->getItem()==li){
            j=i;

        }
    }
    if(j>-1){ m_icons.removeAt(j); }
}


void IconSpreadWindow::arrowMove(int direction){
    int closest=-1;
    float bestDistance = 1000000;
    QPointF point = m_icons[m_selectedItemIndex]->pos();
    for(int i=0; i<m_icons.count();i++){
        if(!inDiagonalQuadrant(point, m_icons[i]->pos(), direction )){
            continue;
        }
        float d = length(m_icons[i]->pos(), point);

        if(d <= bestDistance){
            closest = i;
            bestDistance = d;
        }
    }
    if(closest > 0 && (m_selectedItemIndex !=  closest)){
        m_selectedItemIndex = closest;
        m_icons[closest]->setSelected(true);
    }
    
    

}

void IconSpreadWindow::endMove(){

}

void IconSpreadWindow::startMove(){

}


void IconSpreadWindow::incrementItemsPerCell(){
    m_itemsPerCell++;
    m_iconPositioner.setIPR(m_itemsPerCell);
    m_iconPositioner.positionItems(m_icons);
    m_IconSpreadAnimation->stop();
    m_IconSpreadAnimation->start();
}

void IconSpreadWindow::decrementItemsPerCell(){
    m_itemsPerCell--;
    m_iconPositioner.setIPR(m_itemsPerCell);
    m_iconPositioner.positionItems(m_icons);
    m_IconSpreadAnimation->stop();
    m_IconSpreadAnimation->start();
}
