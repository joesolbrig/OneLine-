#ifndef URL_CHANGE_RECEIVER_H
#define URL_CHANGE_RECEIVER_H

#include <QWidget>
#include <QMessageBox>
#include <QUrl>

class AbstractReceiverWidget : public QWidget {
    Q_OBJECT
public:
    AbstractReceiverWidget(QWidget* p, QFlags<Qt::WindowType> f=QFlags<Qt::WindowType>()): QWidget(p, f) {}
public slots:
    virtual void urlChanged(const QUrl &){ }
    virtual void previewWindowGainedFocus(){ }

};



#endif // URL_CHANGE_RECEIVER_H
