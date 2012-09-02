#ifndef SCROLLBARMINIMAL_H
#define SCROLLBARMINIMAL_H

#include <QScrollBar>
#include <QProxyStyle>


class MyProxyStyle : public QProxyStyle
 {
   public:
     int styleHint(StyleHint hint, const QStyleOption *option = 0,
                   const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
     {
         if (hint == QStyle::SH_UnderlineShortcut)
             return 0;
         return QProxyStyle::styleHint(hint, option, widget, returnData);
     }
     void QStyleSheetStyle::drawComplexControl(ComplexControl cc,
        const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;


 };


class ScrollBarMinimal : public QScrollBar
{
Q_OBJECT
public:
    explicit ScrollBarMinimal(QObject *parent = 0);
    virtual void paintEvent ( QPaintEvent * );
signals:

public slots:

};

#endif // SCROLLBARMINIMAL_H
