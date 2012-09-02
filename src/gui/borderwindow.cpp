#include "constants.h"
#include "borderwindow.h"
#include "QPainter"
#include "QColor"


BorderWindow::BorderWindow() : QWidget()
{
    setWindowFlags(Qt::FramelessWindowHint );
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(UI_BORDER_WINDOW_OPACITY);

    QLinearGradient grad;
    grad.setCoordinateMode(QGradient::StretchToDeviceMode);
    QGradientStops stops;
    stops.append(QGradientStop(0, Qt::black));
    stops.append(QGradientStop(0.7, Qt::darkGray));
    stops.append(QGradientStop(1, Qt::black));
    grad.setStops(stops);
    QBrush brush(grad);
    QMatrix matrix;
    matrix.rotate(30);
    brush.setMatrix(matrix);
    m_brush = brush;
}

void BorderWindow::paintEvent(QPaintEvent * /* event */){
    QPainter painter(this);
    painter.fillRect(rect(),m_brush);
}


