#include  <QPainter>
#include <QWidget>
#include <QDebug>

#include "fancyenclosingrect.h"
#include "constants.h"
#include "globals.h"



QRegion FancyEnclosingRect::getStandardBox(QRect rect){
    QRegion r(getStandardPath(rect).toFillPolygon().toPolygon());
    return r;
}

QPainterPath FancyEnclosingRect::getStandardPath(QRect rect){
    QPainterPath p;
    p.addRoundedRect ( rect, UI_RECT_XRADIUS, UI_RECT_YRADIUS, Qt::AbsoluteSize );
    return p;
}

void FancyEnclosingRect::paintPaths(QPainter* painter, QList<QPair < QPainterPath,QBrush > > paths){
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform,true);
    for(int i=paths.count()-1; i>=0;i--){
        painter->fillPath(paths[i].first, paths[i].second );
        //painter->drawPixmap(m_marginRect,m_backgroundImage);
    }
    painter->restore();
}


QList<QPair < QPainterPath,QBrush > > FancyEnclosingRect::createStyledRects(QRect rect,QString name, bool flatTop){
    QString key = UI_RECT_PREFIX + name;

    gStyleSettings->endArray();
    int count = gStyleSettings->beginReadArray(key);

    QList<QPair < QPainterPath,QBrush > > res;

    for(int i=0; i< count; ++i){
        gStyleSettings->setArrayIndex(i);
        int innerWidth = gStyleSettings->value(UI_RECT_INNER_WIDTH_KEY).toInt();
        int outterWidth = gStyleSettings->value(UI_RECT_OUTTER_WIDTH_KEY).toInt();
        QBrush brush = getBrush(UI_RECT_COLOR_KEY, 1);

        //manually "POPing" our indexes back
        gStyleSettings->beginReadArray(key);
        gStyleSettings->setArrayIndex(i);

        QPoint offset = getPoint(UI_RECT_OFFSET_KEY);
        QPair<QPainterPath,QBrush> pair;
        rect.adjust(outterWidth, outterWidth, -outterWidth, -outterWidth);
        const QPainterPath path = createStyledRect(rect,innerWidth,offset, flatTop);
        pair.first = path;
        pair.second= brush;
        res.append(pair);
    }
    gStyleSettings->endArray();
    return res;
}

QPainterPath FancyEnclosingRect::createStyledRect(QRect rect,
               int innerWidth,
               QPoint offset,bool flatTop){

    if(!offset.isNull()){
        rect.adjust(offset.x(), offset.y(), offset.x(), offset.y());
    }
    QRect innerRect = rect;
    QPainterPath path;
    if(!validRect(rect) ){
        return path;
    }

//    path.addRoundedRect (
//            rect, UI_RECT_XRADIUS, UI_RECT_YRADIUS, Qt::AbsoluteSize );
    path.addPath (createRect(rect,flatTop));
    if(innerWidth>=0){
        innerRect.adjust(innerWidth, innerWidth, -innerWidth, -innerWidth);
//        path.addRoundedRect(
//                innerRect, UI_RECT_XRADIUS, UI_RECT_YRADIUS, Qt::AbsoluteSize );
        path.addPath (createRect(innerRect,false));
    }

    path.setFillRule(Qt::OddEvenFill);
    return path;
}

QPainterPath FancyEnclosingRect::createRect(QRect rect, bool flatTop){

    QPainterPath rounded;
    if(flatTop){
        QRect higher = rect;
        higher.setTop(higher.top() - 4*(UI_RECT_XRADIUS + UI_RECT_YRADIUS));
        rounded.addRoundedRect(
            higher, UI_RECT_XRADIUS, UI_RECT_YRADIUS, Qt::AbsoluteSize);

        QRect lower = rect;
        lower.setBottom(lower.bottom()+4*(UI_RECT_XRADIUS + UI_RECT_YRADIUS));
        QPainterPath squared;
        squared.addRect(lower);
        QPainterPath final = squared.intersected(rounded);
        return final;
    } else {
        rounded.addRoundedRect(
            rect, UI_RECT_XRADIUS, UI_RECT_YRADIUS, Qt::AbsoluteSize);
        return rounded;
    }


//    int offset = 10;
//    QPainterPath path;
//    path.moveTo(rect.left(),rect.bottom()-offset);
//    QPoint topLeft(rect.left(),rect.bottom()-offset);
//    QPoint bRight(rect.left()+offset,rect.bottom());
//    QRect r(topLeft,bRight);
//    path.arcTo(r,180,90);
////    path.arcMoveTo(rect.left(),rect.bottom()-offset, rect.left()+offset,rect.bottom(),90);
//    path.lineTo(rect.right()-offset,rect.bottom());
//
//    {QPoint topLeft(rect.right()-offset,rect.bottom());
//    QPoint bRight(rect.right(),rect.bottom()-offset);
//    QRect r(topLeft,bRight);
//    path.arcTo(r,270,90);}
//
////    path.arcMoveTo(rect.right()-offset,rect.bottom(), rect.right(),rect.bottom()-offset,90);
//    if(!flatTop){
//        path.lineTo(rect.right(),rect.top()+offset);
//
//        {QPoint topLeft(rect.right(),rect.top()+offset);
//        QPoint bRight(rect.right()-offset,rect.top());
//        QRect r(topLeft,bRight);
//        path.arcTo(r,0,90);}
//
////        path.arcMoveTo(rect.right(),rect.top()+offset, rect.right()-offset,rect.top(),90);
////        path.lineTo(rect.right()-offset,rect.top());
//        path.lineTo(rect.left()+offset,rect.top());
//        QPoint topLeft(rect.left()+offset,rect.top());
//        QPoint bRight(rect.left(),rect.top()+offset);
//        QRect r(topLeft,bRight);
//        path.arcTo(r,90,90);
////        path.arcMoveTo(rect.left()+offset,rect.top(), rect.left(),rect.top()+offset,90);
////        path.lineTo(rect.left(),rect.top()+offset);
//    } else {
//        path.lineTo(rect.topRight());
//        path.lineTo(rect.topLeft());
//    }
//    path.lineTo(rect.left(),rect.bottom()-offset);
//    path.closeSubpath();
//    return path;
}

QBrush FancyEnclosingRect::getBrush(QString key, qreal scale){
    QString colorStr = gStyleSettings->value(UI_RECT_COLOR_KEY).toString();
    gStyleSettings->endArray();
    if(colorStr.startsWith(IMAGE_RECT_PREFIX)){
        QString bitmapPath = colorStr.right(colorStr.length() - IMAGE_RECT_PREFIX.length());
        QImage image(UI_SKIN_DIR + bitmapPath);
        QBrush brush;
        brush.setTextureImage(image);
        return brush;
    } else if(colorStr.startsWith(RECT_GRADIANT_PREFIX)){
        QString key = colorStr;
        QLinearGradient grad;
        grad.setCoordinateMode(QGradient::StretchToDeviceMode);
        QGradientStops stops;

        int count = gStyleSettings->beginReadArray(key);
        gStyleSettings->setArrayIndex(0);
        qreal angle = gStyleSettings->value(UI_RECT_GRADIANT_ANGLE_KEY).toReal();
        int stop = 0;
        for(int i=0; i< count; ++i){
            gStyleSettings->setArrayIndex(i);
            QColor color = getColor(UI_RECT_COLOR_KEY);
            stop = gStyleSettings->value(UI_RECT_GRADIANT_STOP_KEY).toInt();
            stops.append(QGradientStop(stop*scale, color));
        }
        gStyleSettings->endArray();
        grad.setStops(stops);
        if(count>0){
            QBrush brush(grad);
            QMatrix matrix;
            matrix.rotate(angle);
            brush.setMatrix(matrix);
            return brush;
        } else { return QBrush();}
    }
    QColor c = getColor(key);
    QBrush brush(c);
    return brush;
}

QColor FancyEnclosingRect::getColor(QString key){
    QString str = gStyleSettings->value(key).toString();
    //qDebug() << "FancyEnclosingRect::getColor k:v " << key << ":" << str;
    QList<int> values = parseFormattedArray(str);
    if(values.count()==4){
        QColor c(values[0],values[1],values[2],values[3]);
        if(c.isValid()){
            return c;
        }
    } else  if(values.count()==3){
        QColor c(values[0],values[1],values[2]);
        if(c.isValid()){
            return c;
        }
    } else  if(values.count()==1 && values[0] <=Qt::transparent && values[0]>=Qt::color0){
        QColor c((Qt::GlobalColor)values[0]);
        if(c.isValid()){
            return c;
        }
    } else {
        QColor c(str);
        if(c.isValid()){
            return c;
        }
    }
    return QColor();
}



QRect FancyEnclosingRect::getRect(QString key){
    QString str = gStyleSettings->value(key).toString();
    QList<int> values = parseFormattedArray(str);
    if(values.count()==4){
        QRect r(values[0],values[1],values[2],values[3]);
        return r;
    }
    Q_ASSERT(values.count()==0);
    return QRect();
}

QPoint FancyEnclosingRect::getPoint(QString key){
    QString str = gStyleSettings->value(key).toString();
    QList<int> values = parseFormattedArray(str);
    if(values.count()==2){
        QPoint r(values[0],values[1]);
        return r;
    }
    Q_ASSERT(values.count()==0);
    return QPoint();
}


//Really loose format - an anything-seperated array - use a comma anyway
QList<int> parseFormattedArray(QString arrayString){
    int value = -1;
    int sign=1;
    QList<int> res;
    if(!arrayString.contains(',')){ return res;}

    for(int i=0; i<arrayString.count();i++){
        QChar d = arrayString[i];
        if(d =='-' && value==-1){
            sign =-1;
        } else if(d.isDigit() ){
            if(value==-1){
                value = ((arrayString[i].toAscii()) - '0');
            } else {
                value = 10*value + (arrayString[i].toAscii() -('0'));
            }
        } else {
            Q_ASSERT(d==QChar(' ')  || d==QChar(',') || d==QChar('(') || (d==QChar(')')) || d.isSpace());
            if(value!=-1){
                res.append(sign*value);
                value=-1;
                sign=1;
            }
        }
    }
    if(value!=-1){ res.append(sign*value); }
    return res;
}

bool FancyEnclosingRect::validRect(QRect r){
    if(r.height()<=UI_RECT_XRADIUS*2){
        return false;
    }
    if(r.width()<=UI_RECT_YRADIUS*2){
        return false;
    }
    return true;
}

