#ifndef FANCYLABEL_H
#define FANCYLABEL_H

#include <QWidget>
#include <QLabel>

#include "item.h"

#define SECTION_HEIGHT 5


class FancyLabel : public QWidget
{
Q_OBJECT
public:
    explicit FancyLabel(QWidget *parent = 0);

    void setupImage(QImage image, int cutHeight, int sectionHeight = SECTION_HEIGHT){
        Q_ASSERT((sectionHeight/2)*2 == sectionHeight);
        Q_ASSERT(cutHeight < image.height());
        Q_ASSERT(cutHeight > 0);

        m_originalImage = image;
        m_currentHeight = image.height();

        m_cutHeight = cutHeight;
        m_sectionHeight = sectionHeight;

        createImageSections();
        resize(image.size());
    }

    void setCurrentHeight(int height){
        m_currentHeight = MAX(height, m_originalImage.height());
        this->resize(m_originalImage.width(), m_currentHeight);
        update();
    }

    int baseHeight(){
        return m_originalImage.height();
    }

    private:

        QImage m_originalImage;
        QImage m_top;
        QImage m_middle;
        QImage m_bottom;

        int m_currentHeight;
        int m_cutHeight;

        int m_sectionHeight;

    protected:
        void paintEvent(QPaintEvent *event);

        void setCutHeight(int newHeight){
            m_cutHeight = newHeight;

        }
        void createImageSections();

    signals:

    public slots:

};


//class FancyLabel : public QLabel {
//    Q_OBJECT
//public:
//    QSize sizeHint() const;
//    FancyLabel(QWidget* parent);
//
//private:
//    QImage top;
//    QImage section;
//    QImage bottom;
//
//    int m_currentHeight;
//    int m_cutHeight;
//protected:
//    void paintEvent(QPaintEvent *event);
//    void setTotalHeight(int newHeight){
//    }
//    void setCutHeight(int newHeight);
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


#endif // FANCYLABEL_H
