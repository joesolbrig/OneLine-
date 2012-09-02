#ifndef PREVIEWPANE_H
#define PREVIEWPANE_H

#include <QWidget>

#include <QWebView>
#include <QPropertyAnimation>
#include <QLabel>
#include <QMessageBox>
#include <QWebInspector>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "globals.h"
#include "item.h"
#include "url_change_receiver.h"


//Pretty every file type *needs* to become
//browser-visible.
class PreviewPane : public QWebView
{
    Q_OBJECT
    private:
        CatItem m_item;
        QLabel m_imageLabel;
        static QWebInspector* mp_inspectorPage;

        QNetworkReply* m_reply;


    protected:
        void mousePressEvent(QMouseEvent *e){
            emit mouseFocusGained();
            QWebView::mousePressEvent(e);
        }

        void mouseMoveEvent(QMouseEvent *e){
            emit mouseHoverGained(true);
            QWebView::mouseMoveEvent(e);
        }

        void focusOutEvent(QFocusEvent * ev){
            emit mouseHoverGained(false);
            QWebView::focusOutEvent(ev);
        }

        enum {
            UNKNOWN_HTML,
            CUSTOM_HTML
        } m_loadType;

    public:

        //should kill popups but doesn't
        //QWebView* createWindow (QWebPage::WebWindowType ){ return 0;}

        explicit PreviewPane(QWidget *parent=0);

        void paintEvent ( QPaintEvent * event );

        bool setItem(CatItem it, bool justTesting=false);
        void setGeometry(QRect r);
        void hookURLChange(AbstractReceiverWidget *w){
            connect(this, SIGNAL( urlChanged( const QUrl &)),w, SLOT(urlChanged( const QUrl &)));
            connect(this, SIGNAL( mouseFocusGained()),w, SLOT(previewWindowGainedFocus()));
        }
        void hookHover(AbstractReceiverWidget *w){
            connect(this, SIGNAL( mouseHoverGained(bool)),w, SLOT(previewWindowHovered(bool)));
        }

        void show();
        CatItem getItem(){
            return m_item;
        }

        void parentKeyPress(QKeyEvent* key){
            QWebView::keyPressEvent(key);
        }

        void parentKeyRelease(QKeyEvent* key){
            QWebView::keyReleaseEvent(key);
        }

        void startGettingUrlHtml(QUrl url);

//        void setGeometry(const QRect &r){
//            m_imageLabel.setGeometry(r);
//            QWebView::setGeometry(r);
//
//        }

        void setGeometry(int x, int y, int w, int h){
            m_imageLabel.setGeometry(x, y, w, h);
            QWebView::setGeometry(x, y, w, h);

        }

        void fadeIn(){
            if(!this->isVisible()){
                this->setWindowOpacity(0);
                setVisible(true);
                raise();
                QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
                animation->setDuration(UI_FADEIN_TIME/4);
                QEasingCurve ec(QEasingCurve::OutCirc);
                //ec.setAmplitude(0.5);
                animation->setEasingCurve(ec);
                animation->setStartValue(0);
                animation->setEndValue(0.5);
                animation->start();
            }
        }

        void fadeOut(){
            Q_ASSERT(false);

            if(this->isVisible()){
                QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
                animation->setDuration(UI_FADEIN_TIME);
                animation->setStartValue(1);
                animation->setEndValue(0);
                animation->start();
            }

        }

        void setReaderScript(bool setOnload=false);
        void searchForString(QString searchStr);
    signals:
        void mouseFocusGained();
        void mouseHoverGained(bool);

    public slots:
        void urlLoaded();
        void finishLoading(bool);
        void readyToShow();
        void loadingStart();
        void progressLoading(int);
        void debugOut(QString);

};

#endif // PREVIEWPANE_H
