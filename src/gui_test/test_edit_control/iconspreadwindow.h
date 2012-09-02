#ifndef ICONSPREADWINDOW_H
#define ICONSPREADWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <gtk/gtk.h>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScreen>
#include <QAnimationGroup>

#include "individualIcon.h"
#include "messageiconwidget.h"

inline double length(QPointF a, QPointF b){
    QPointF c = a -b;
    double xx = c.x();
    double yy = c.y();
    return sqrt( (xx)*(xx) + (yy)*(yy));

};

inline bool inDiagonalQuadrant(QPointF loc, QPointF o, int quandrant){
    QPointF offset = loc -o;
    if(offset.y()==0){
        return quandrant%2==1 && (quandrant-2)*offset.y() > 0;
    }

    int r = abs(offset.x())/abs(offset.y());

    if(quandrant == 0){
        return (r <= 1  && offset.x() > 0);
    }

    if(quandrant == 1){
        return (r > 1  && offset.y() > 0);
    }

    if(quandrant == 2){
        return (r <= 1  && offset.x() < 0);
    }

    if(quandrant == 3){
        return (r > 1  && offset.y() < 0);
    }
    Q_ASSERT(false);
    return false;


};

class ItemPositioner {
    QRect m_screenArea;
    int m_itemsPerRectangle;
    float m_lastRowFraction;
    int m_totalDepth;

public:
    ItemPositioner(QRect area, int ipr){
        m_itemsPerRectangle = ipr;
        m_screenArea = area;
    }

    void setIPR(int ipr){
        m_itemsPerRectangle = ipr;
    }

    void positionItems(QList<IndividualIcon*>& itms){
        int cellCount = itms.count()/m_itemsPerRectangle;
        m_totalDepth = log2(cellCount);
        m_lastRowFraction = getLastRowFraction(itms.count());

        int i =-1;
        for(int index=0; index<cellCount;index++){
            addCell(itms,i);
        }
    }

    float getLastRowFraction(int count){
        return (count - (2^m_totalDepth))/(2^(m_totalDepth));
    }

    void addCell(QList<IndividualIcon*>& itms,int i){
        int d = log2(i);
        int relInd = i - (2^d);
        Q_ASSERT(relInd >=0 && relInd < (2^(d+1)));
        float cellScale = 2^(-d);

        i = (d == m_totalDepth) ? i: i/m_lastRowFraction;

        QSizeF cellSize(m_screenArea.width()*cellScale,m_screenArea.height()*cellScale);
        float cellRight = m_screenArea.right()*(1- cellScale/2);
        float cellLeft = cellRight - cellSize.width();
        float cellBottom = relInd*cellScale;
        float cellTop = cellSize.height() + cellBottom;
        QPointF tl(cellLeft,cellTop);
        QRectF r(tl,cellSize);
        fillCell(itms, i*m_itemsPerRectangle, r);

    }

    void fillCell(QList<IndividualIcon*>& itms, int begin, QRectF location){
        float a = m_screenArea.width() * m_screenArea.height();
        int w = sqrt(m_itemsPerRectangle) * (m_screenArea.width()/a)+0.99;
        int ht = sqrt(m_itemsPerRectangle) * (m_screenArea.height()/a)+0.99;

        Q_ASSERT(w*ht >= m_itemsPerRectangle);
        Q_ASSERT(w*(ht-1) <= m_itemsPerRectangle);

        // Carefull - we're arranging our icons in a rectangular grid with
        // with the missing places on the left & right edges,
        // then each row vertically centered
        //

        // (1 - colRatio)*h*w +  colmnRatio*h*(w-1) = m_itemsPerRectangle;
        // (1 - colRatio)*h*w + colmnRatio*ht*(w) - columnRation*ht
        //                                        = m_itemsPerRectangle;
        // ht*w - columnRation*h = m_itemsPerRectangle;
        float colRatio = w - (m_itemsPerRectangle/ht);
        int h;

        float widthIncrement = location.width()/w;
        for(int i=0; i < m_itemsPerRectangle;i++){
            h =ht;
            int col = i % (h);
            //use one less row on edges to fill non/rectangular sets
            h -= (abs(w/2 -col+0.1) < colRatio/2)? 1:0;
            float heightIncrement = location.height()/h;
            int row = i/(h);
            QPoint pos(col*widthIncrement, heightIncrement*row);

            QRectF r(pos.x() + widthIncrement/2, pos.y() + heightIncrement/2, widthIncrement, heightIncrement);
            itms[i+begin]->setBoundingRect(r);
        }

    }

protected:
    void allocateArea(int itemCount);

};


class IconSpreadWindow : public QWidget
{
private:
    Q_OBJECT

    QGraphicsScene* m_scene;
    QGraphicsView* m_view;

    int m_itemsPerCell;
    ItemPositioner m_iconPositioner;
    QList<IndividualIcon*> m_icons;

    IconSpreadAnimation* m_IconSpreadAnimation;

    int m_selectedItemIndex;


public:
    IconSpreadWindow(QWidget *parent = 0);

    ~IconSpreadWindow(){

    }

    void arrowMove(int direction);
    void endMove();
    void startMove();

    void incrementItemsPerCell();

    void decrementItemsPerCell();


    void addItems(QList<ListItem> its){
        qDebug() << " ---- clearing ----- ";
        for(int i=0; i < its.count(); i++){ m_scene->removeItem(m_icons[i]); }

        for(int i=0; i < its.count(); i++){
            ListItem li[i];
            addItem(li[i]);
        }
        repositionItems(m_itemsPerCell);
    }

    IndividualIcon* selectedItem(){
        for(int i=0; i < m_icons.size();i++){
            if(m_icons[i]->isSelected()){
                return m_icons[i];
            }
        }


    }

    void addItem(ListItem li);

    void removeItem(ListItem li);

    void repositionItems(int itemsPerCell){
        m_iconPositioner.setIPR(itemsPerCell);
        m_iconPositioner.positionItems(m_icons);
        startAnimation();


    }

    ListItem itemAtLocation(QPointF point){

        int closest=-1;
        float bestDistance = 1000000;
        for(int i=0; i<m_icons.count();i++){
            float d = length(m_icons[i]->pos(), point);
            if(d <= bestDistance){
                closest = i;
                bestDistance = d;
            }
        }
        return m_icons[closest]->getItem();
    }

    void startAnimation();

signals:
    void itemSelected(ListItem it);


public slots:
    void iconHovered(){


    }
    void iconCaptured(){

    }

    void iconReleased(){

    }

};

class IconSpreadAnimation : public QAnimationGroup {

public:
    IconSpreadAnimation(){}

    void beginAnimation(){
        for(int i=0; i< animationCount();i++){
            animationAt(i)->start();
        }

    }

    void stopAnimation(){
        for(int i=0; i< animationCount();i++){
            animationAt(i)->stop();
        }
    }

    void updateCurrentTime(int ) {
    }

    int duration() const {
        return STANDARD_MOMENT/SIGNAL_MOMENT_RATIO;

    }



};

class IndividIconAnimation  : public QPropertyAnimation {
    IndividualIcon* m_icon;
    QPainterPath m_path;

public:

    IndividIconAnimation(QObject *target)
        : QPropertyAnimation(target, "pos") {
        m_icon = (IndividualIcon*)target;
        m_path = QPainterPath();

        setDuration(STANDARD_MOMENT/SIGNAL_MOMENT_RATIO);

        //connect(this, SIGNAL(finished()), m_target,SLOT(animationDone()));
    }

    void updateCurrentTime(int currentTime) {
        if (m_path.isEmpty()) {
            QPointF from = m_icon->pos();
            QPointF to = m_icon->getDesiredPosition();
            m_path.moveTo(from);
            m_path.lineTo(to);
        }
        int dura = duration();
        const qreal progress = ((dura == 0) ? 1 : ((((currentTime - 1) % dura) + 1) / qreal(dura)));

        qreal easedProgress = easingCurve().valueForProgress(progress);
        QPointF pt = m_path.pointAtPercent(easedProgress);
        updateCurrentValue(pt);
        //emit valueChanged(pt);
    }


};

#endif // ICONSPREADWINDOW_H
