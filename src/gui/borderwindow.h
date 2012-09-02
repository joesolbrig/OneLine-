#ifndef BORDERWINDOW_H
#define BORDERWINDOW_H

#include <QWidget>

class BorderWindow : public QWidget
{
Q_OBJECT

private:
    QBrush m_brush;
protected:
      void paintEvent(QPaintEvent *event);
public:
    explicit BorderWindow();

signals:

public slots:

};

#endif // BORDERWINDOW_H
