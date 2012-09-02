#ifndef PREVIEWPANE_H
#define PREVIEWPANE_H

#include <QWidget>

#include <QWebView>

#include "item.h"


//Pretty every file type *needs* to become
//browser-visible.
class PreviewPane : public QWebView
{
    Q_OBJECT
    private:
        CatItem m_item;

    public:
        explicit PreviewPane(QWidget *parent = 0);
        void setItem(CatItem it);



    signals:

    public slots:

};

#endif // PREVIEWPANE_H
