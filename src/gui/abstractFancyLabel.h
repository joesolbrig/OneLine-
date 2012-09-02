#ifndef ABSTRACTFANCYLABEL_H
#define ABSTRACTFANCYLABEL_H

#endif // ABSTRACTFANCYLABEL_H

#include "QImage"
#include "QPaintEvent"
#include "QWidget"
#include "constants.h"
#include "QPropertyAnimation"


class BaseLabel;


class AbstractFancyLabel
{
    BaseLabel* d;

public:

    virtual void setImageHeight(int height)=0;

    virtual int getImageHeight()=0;

    virtual int baseHeight()=0;

    virtual void setOpacity(qreal o)=0;

    virtual qreal getOpacity()=0;

    virtual void animateToOpacity(qreal l)=0;

    virtual void showConnectingRegion(bool show=true)=0;

    virtual void setExplanPopoutArea(float left, float right, float base)=0;

    virtual void setMainEditArea(float left, float right, float base)=0;

    protected:
        virtual void paintEvent(QPaintEvent *event)=0;

};



class RectangleLabel : public AbstractFancyLabel, public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
    qreal m_opacity;

public:
    explicit RectangleLabel(QWidget *parent = 0);

    void setImageHeight(int height){
        m_currentHeight = height;
    }

    int getImageHeight(){
        return m_currentHeight;
    }

    int baseHeight(){
        return m_currentHeight;
    }

    void setOpacity(qreal o){
        m_opacity = o;
    }

    qreal getOpacity(){
        return m_opacity;
    }

    void animateToOpacity(qreal l){
        Q_ASSERT(l>=0 && (l <=1));
        QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
        animation->setDuration(UI_TEXTEDIT_FADE_TIME);
        qreal f = this->getOpacity();
        animation->setStartValue(f);
        animation->setEndValue(l);
        animation->start();
    }

    void showConnectingRegion(bool show=true){
        m_illustrativeRegion = QPainterPath();

        if(!show || m_editRegionRight.isNull() || m_explanRegionRight.isNull())
            { return; }

        m_illustrativeRegion.moveTo(m_explanRegionRight);

        const QPointF controlPtEditRight(m_editRegionRight.x(),m_editRegionRight.y());
        const QPointF controlPtExplanRight(m_explanRegionRight.x(),m_explanRegionRight.y());

        m_illustrativeRegion.cubicTo(
                controlPtEditRight,
                controlPtExplanRight,
                m_editRegionRight);
        //m_illustrativeRegion.lineTo(m_illRegionTopRight);
        m_illustrativeRegion.lineTo(m_editRegionLeft);

        const QPointF controlPtEditLeft(m_editRegionLeft.x(),m_editRegionLeft.y());
        const QPointF controlPtExplanLeft(m_explanRegionLeft.x(),m_explanRegionLeft.y());


        m_illustrativeRegion.cubicTo(
                controlPtExplanLeft,
                controlPtEditLeft,
        m_explanRegionLeft);
        m_illustrativeRegion.lineTo(m_explanRegionRight);

        m_illustrativeRegion.closeSubpath();

        return;

    }

    void setExplanPopoutArea(float left, float right, float base) {
        Q_ASSERT(left <=right);
        m_explanRegionLeft = QPointF(left,base);
        m_explanRegionRight = QPointF(right,base);
    }

    void setMainEditArea(float left, float right, float base) {
        Q_ASSERT(left <=right);
        m_editRegionLeft = QPointF(left,base);
        m_editRegionRight = QPointF(right,base);
    }

    private:

        int m_currentHeight;


        //for illustration polygon
        QPainterPath m_illustrativeRegion;
        QPointF m_editRegionRight;
        QPointF m_editRegionLeft;
        QPointF m_explanRegionRight;
        QPointF m_explanRegionLeft;


    protected:
        void paintEvent(QPaintEvent *event);


    signals:

    public slots:

};

