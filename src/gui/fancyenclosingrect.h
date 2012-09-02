#ifndef FANCYENCLOSINGRECT_H
#define FANCYENCLOSINGRECT_H

#include <QPainterPath>
#include <QSettings>

QList<int> parseFormattedArray(QString arrayString);

//Just a bucket of methods really...
class FancyEnclosingRect
{
    //static QPainterPath m_modelPath;
public:
    static void paintPaths(QPainter* painter, QList<QPair < QPainterPath,QBrush > > paths);
    static QRegion getStandardBox(QRect r);
    static QPainterPath getStandardPath(QRect r);
    static QList<QPair < QPainterPath,QBrush > > createStyledRects(
            QRect rect,
            QString name,
            bool flatTop=false);
    static QPainterPath
            createStyledRect(
                           QRect rect,
                           int innerWidth,
                           QPoint offset,
                           bool flatTop=false);
    static     QPainterPath createRect(QRect rect, bool flatTop);
    static QColor getColor(QString key);
    static QBrush getBrush(QString key, qreal scale);
    static QRect getRect(QString key);
    static QPoint getPoint(QString key);
    static bool validRect(QRect r);
};

#endif // FANCYENCLOSINGRECT_H
