#ifndef FancyHorizontalLabel_H
#define FancyHorizontalLabel_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>

#include "item.h"
#include "globals.h"

#define SECTION_WIDTH 20


class FancyHorizontalLabel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
    qreal m_opacity;

public:
    explicit FancyHorizontalLabel(QWidget *parent = 0);

    void setupImage(QImage image, int cutWidth, int sectionWidth = SECTION_WIDTH){
        Q_ASSERT((sectionWidth/2)*2 == sectionWidth);
        Q_ASSERT(cutWidth < image.width());
        Q_ASSERT(cutWidth > 0);

        m_originalImage = image;
        m_currentWidth = image.width();

        m_cutWidth = cutWidth;
        m_sectionWidth = sectionWidth;

        createImageSections();
        resize(image.size());
        m_opacity = 1;
    }

    void setCurrentWidth(int Width){
        m_currentWidth = MAX(Width, m_originalImage.width());
        this->resize(m_originalImage.width(), m_currentWidth);
        update();
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



    private:

        QImage m_originalImage;
        QImage m_left;
        QImage m_middle;
        QImage m_right;

        int m_currentWidth;
        int m_cutWidth;

        int m_sectionWidth;

    protected:
        void paintEvent(QPaintEvent *event);

        void setCutWidth(int newWidth){
            m_cutWidth = newWidth;

        }
        void createImageSections();

    signals:

    public slots:

};


//class FancyHorizontalLabel : public QLabel {
//    Q_OBJECT
//public:
//    QSize sizeHint() const;
//    FancyHorizontalLabel(QWidget* parent);
//
//private:
//    QImage top;
//    QImage section;
//    QImage bottom;
//
//    int m_currentWidth;
//    int m_cutWidth;
//protected:
//    void paintEvent(QPaintEvent *event);
//    void setTotalWidth(int newWidth){
//    }
//    void setCutWidth(int newWidth);
//    void setImage(QImage i);
//    void setAlphaImage(QImage i);
//
//signals:
//
//public slots:
//    void controlsResized();
//
//
//};


#endif // FancyHorizontalLabel_H
