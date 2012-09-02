#ifndef TEST_STREAM_WINDOW_H
#define TEST_STREAM_WINDOW_H


#include <qdebug.h>

#include "url_change_receiver.h"

class ReceiverWidget : public AbstractReceiverWidget {
    Q_OBJECT
public:
    ReceiverWidget(QWidget* p): AbstractReceiverWidget(p){}

public slots:
    void urlChanged(const QUrl &url){
        qDebug() << url.toString();
    }

};

#endif // TEST_STREAM_WINDOW_H
