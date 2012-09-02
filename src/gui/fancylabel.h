#ifndef FANCYLABEL_H
#define FANCYLABEL_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>

#include "globals.h"
#include "item.h"

#define SECTION_HEIGHT 5

//#include "abstractFancyLabel.h"

class FancyLabel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
    qreal m_opacity;
    bool m_flat;

public:
    explicit FancyLabel(QWidget *parent = 0);

    void setFlat(bool flat=true){
        m_flat =flat;
    }

    void setupImage(QImage image, int cutHeight, int sectionHeight = SECTION_HEIGHT){
        Q_ASSERT((sectionHeight/2)*2 == sectionHeight);
        if(cutHeight > image.height()/2){
            cutHeight = image.height()/2;
        }
        Q_ASSERT(cutHeight > 0);

        m_originalImage = image;
        m_currentHeight = image.height();

        m_cutHeight = cutHeight;
        m_sectionHeight = sectionHeight;

        createImageSections();
        resize(image.size());
        m_opacity = 1;

        m_illustrativeRegion.setFillRule(Qt::WindingFill);

    }

    void setCurrentHeight(int height){
        m_currentHeight = MAX(height, m_originalImage.height());
        this->resize(m_originalImage.width(), m_currentHeight);
        update();
    }

    int currentHeight(){
        return m_currentHeight;
    }

    int baseHeight() const {
        return m_originalImage.height();
    }

    int baseWidth(){
        return m_originalImage.width();
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
    void move(const QPoint p);
    void move(int x, int y){
        QPoint p(x,y);
        move(p);
    }

    void setGen();

    private:

        QImage m_originalImage;
        QImage m_top;
        QImage m_middle;
        QImage m_bottom;

        int m_currentHeight;
        int m_cutHeight;

        int m_sectionHeight;

        //for illustration polygon
        QPainterPath m_illustrativeRegion;
        QPointF m_editRegionRight;
        QPointF m_editRegionLeft;
        QPointF m_explanRegionRight;
        QPointF m_explanRegionLeft;

        bool m_generated;
        QList<QPair < QPainterPath,QBrush > > m_pPaths;


    protected:
        void paintEvent(QPaintEvent *event);

        void setCutHeight(int newHeight){
            m_cutHeight = newHeight;
        }

        void createImageSections();
        void setGeometry(QRect r);
        void resize(QSize sz);
        void resize(int x, int y){
            QSize s(x,y);
            resize(s);
        }

    signals:

    public slots:

};




#endif // FANCYLABEL_H
