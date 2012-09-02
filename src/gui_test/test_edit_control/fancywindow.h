#ifndef FANCYWINDOW_H
#define FANCYWINDOW_H

#include <QWidget>

#include "myfulltextedit.h"

#include "iconspreadwindow.h"
#include "fancylabel.h"



class FancyWindow : public QWidget
{
Q_OBJECT

//We allow only 0-8 numbered positions
Q_PROPERTY(int screenPosition READ getScreenPosition WRITE setScreenPosition)

public:
    QSize sizeHint() const;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

    void calculateWindowSize();


private:
    //Window positioning states:
    QPoint dragPosition;
    QRect visibleRect;

    //Window interaction states:
    bool m_showIconSpread;
    bool m_showInactiveText;


    QImage m_itemIcon;
    QImage m_actionIcon;
    QLabel m_visibleBackground;

    MyFullTextEdit m_input;
    QLabel m_output;
    MyFullTextEdit m_inactiveLongText;
    IconSpreadWindow m_iconSpread;


signals:

public slots:
    void controlsResized();

};

#endif // FANCYWINDOW_H
