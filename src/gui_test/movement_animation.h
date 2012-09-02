#ifndef MOVEMENT_ANIMATION_H
#define MOVEMENT_ANIMATION_H

#include <QFont>
#include <QListWidget>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QTextDocument>
#include <QPainter>
#include <QTextLayout>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QTextStream>
#include "item.h"
#include "list_item.h"
#include "icon_delegate.h"



class MovementAnimation : public QPropertyAnimation {
public:
    enum animationDirection {
        BeginLeft,
        BeginRight
    };
private:
    QPainterPath m_path;
    QWidget* m_target;
    QString m_propertyName;
    float m_position;

public:

    FrameAnimation(QWidget* target, QString propertyName)
        : QPropertyAnimation(target, propertyName) {
        m_target = target;
        m_propertyName = propertyName;
        m_path = QPainterPath();
        connect(this, SIGNAL(finished()), parent,SLOT(animationDone()));
    }

    //Assure circular movement...
    void updateCurrentTime(int currentTime)
    {
        if (m_path.isEmpty()) {
            QPointF from = startValue().toPointF();
            QPointF to = endValue().toPointF();
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

    void startAnimation() {

        QPointF p1 = m_target->getLocation();
        setStartValue(p1);
        QPointF p2 = m_itemToCover->getLocation();
        setEndValue(p2);
        setDuration(STANDARD_MOMENT/SIGNAL_MOMENT_RATIO);
        setLoopCount(1);
        start(QAbstractAnimation::DeleteWhenStopped);
    }

};


#endif // MOVEMENT_ANIMATION_H
