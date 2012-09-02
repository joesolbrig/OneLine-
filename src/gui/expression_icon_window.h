#ifndef EXPRESSION_ICON_WINDOW_H
#define EXPRESSION_ICON_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include "item.h"
#include "list_item.h"

class IconWidget : public QWidget{


    QIcon m_icon;

public:
    IconWidget(QWidget *parent = 0);
    void setIcon(QIcon ic);
    QIcon& getIcon();
    void paintEvent ( QPaintEvent* );

};


class ExpressionIconWindow : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(float activeness READ getActiveness WRITE setActiveness)
        float m_activeness;

        QList<IconWidget*> m_expressionIcons;
        int m_activeIcon;
        int m_wantedBeActiveIcon;

        const int m_smallIconSize;
        const int m_largeIconSize;
        QPropertyAnimation* m_animate;

    public:
        explicit ExpressionIconWindow(QWidget *parent = 0);
        void reset();
        ~ExpressionIconWindow();
        int expressionIconCount();
        int activeExpressionIcon();
        void clearIcons();
        void setGeometry(QRect r);
        float getActiveness();
        void setActiveIconPos(int pos);
        void setIcon( QIcon ic, int pos);
        void animateLeft();
        void animateRight();
        void beginAnimation();
        void popIcon();
        void appendIcon(QIcon pm);
        void formatIcons();
    protected:

        // -->
        void setActiveness(float a);
        void formatIcon(int index);
        void stretchRectToRight(int pos, float amount);
        void stretchRectToLeft(int pos, float amount);
        void leftFormatIcon(int pos);
        void rightFormatIcon(int pos);
        void resizeIcons();
        float adj(bool isActive);
    public slots:
        void animateToActiveDone();
};

#endif // EXPRESSION_ICON_WINDOW_H
