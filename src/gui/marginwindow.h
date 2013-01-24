#ifndef MARGINWINDOW_H
#define MARGINWINDOW_H

#include <QWidget>
#include <QBrush>
#include <QPainterPath>
#include "appearance_window.h"

class PlatformBase;

class MarginWindow : public QWidget
{

    QWidget* mainUserWindow();

    int m_vOrient;
    int m_hOrient;

    QRect m_marginRect;
    QList<QPair < QPainterPath,QBrush > > m_pPaths;

    QPair < QPainterPath,QBrush >  m_previewArrow;
    QPainterPath m_previewArrowInside;

    MainUserWindow m_mainWindow;
    QPixmap m_backgroundImage;

protected:
    void paintEvent(QPaintEvent * );
    void resizeEvent(QResizeEvent * /* event */);
    QSize  sizeHint();
public:
    MarginWindow(QWidget *parent, PlatformBase*);
    ~MarginWindow() { QApplication::instance()->quit();}
    void gSetAppPos(int vOrient=-1, int hOrient=-1);
    void createPreviewArrow(QRect previewRect);
    QRect listWindowRect();
    QRect previewWindowRect();


};

#endif // MARGINWINDOW_H
