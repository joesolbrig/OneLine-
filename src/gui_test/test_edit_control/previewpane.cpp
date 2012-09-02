#include "previewpane.h"

PreviewPane::PreviewPane(QWidget *parent) : QWebView(parent), m_item() {
    //this->setWindowFlags(Qt::FramelessWindowHint);
}

void PreviewPane::setItem(CatItem it) {
    //this->setWindowFlags(Qt::FramelessWindowHint);
    QUrl url = it.previewUrl();
    qDebug() << "setting url:" << url.toString();
    setUrl(url);
    load(url);
    show();
    update();
    m_item = it;
}


