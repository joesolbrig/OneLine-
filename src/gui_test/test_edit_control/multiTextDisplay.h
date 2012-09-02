#ifndef MULTITEXTDISPLAY_H
#define MULTITEXTDISPLAY_H

#include <QWidget>
#include <QHash>
#include <QUrl>
#include <QIcon>

#include <myfulltextedit.h>
#include <qapplication.h>

#include "constants.h"
#include "multiTextDisplay.h"
#include "fancylabel.h"


class MultiTextDisplay :public QWidget{
    Q_OBJECT
    Q_PROPERTY(QImage BackgroundImage READ getBackgroundImage WRITE setBackgroundImage)
    QImage m_backgroundImage;
    Q_PROPERTY(int IconSize READ getIconSize WRITE setIconSize)
    int m_iconSize;
    Q_PROPERTY(int leftMargin READ getLeftMargin WRITE setLeftMargin)
    int m_leftMargin;
    Q_PROPERTY(int topMargin READ getTopMargin WRITE setTopMargin)
    int m_topMargin;

    Q_PROPERTY(int backgroundCutHeight READ getBackgroundCutHeight WRITE setBackgroundCutHeight)
    int m_backgroundCutHeight;

    Q_PROPERTY(int imageMidsectionWidth READ getImageMidsectionWidth WRITE setImageMidsectionWidth)
    int m_imageMidsectionWidth;

public:

    explicit MultiTextDisplay(QWidget* p=0): QWidget(p,Qt::FramelessWindowHint),
            m_currentEdit(this),  m_explanationText(this), m_editBelow(this), m_backgroundLabel(this){
        setAttribute(Qt::WA_TranslucentBackground);
        QFont f = QWidget::font();
        f.setPointSize(UI_BASE_FONT_SIZE);
        QWidget::setFont(f);

        //Put this in our larger window eventually...
        m_iconSize = UI_MAIN_ICON_SIZE;
        m_leftMargin = UI_LEFT_MARGIN;
        m_topMargin = UI_TOP_MARGIN;
        m_imageMidsectionWidth = UI_IMAGE_MIDSECTION_WIDTH;
        m_backgroundCutHeight = UI_BACKGROUND_CUT_HEIGHT;
        m_backgroundImage = QImage("../GreenLongTextBox.png");

//        QFile file("oneline.qss");
//        Q_ASSERT(file.exists());
//        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            Q_ASSERT(false);
//        }
//        QString styleSheet = QLatin1String(file.readAll());
//        qDebug() << styleSheet;
//        qApp->setStyleSheet(styleSheet);

        m_backgroundLabel.setupImage(
                getBackgroundImage(),
                getBackgroundCutHeight(),getImageMidsectionWidth());

        m_explanationText.hide();
        m_editBelow.hide();;

        m_currentEdit.setObjectName("EditLine");
        m_currentEdit.show();
        m_currentEdit.raise();
        m_currentEdit.activateWindow();

        connect(&m_currentEdit, SIGNAL(rectangleResizing()), this,SLOT(rectangleResizing()));
        adjustForTextChange();

    }

    QImage getBackgroundImage() const {
        return m_backgroundImage;
    }

    void setBackgroundImage(QImage s) {
        m_backgroundImage = s;
    }

    int getIconSize() const {
        return m_iconSize;
    }

    void setIconSize(int s) {
        m_iconSize = s;
    }

    int getTopMargin() const {
        return m_topMargin;
    }

    void setTopMargin(int s) {
        m_topMargin = s;
    }

    int getLeftMargin() const {
        return m_leftMargin;
    }

    void setLeftMargin(int s) {
        m_leftMargin = s;
    }

    int getBackgroundCutHeight() const {
        return m_backgroundCutHeight;
    }

    void setBackgroundCutHeight(int s) {
        m_backgroundCutHeight = s;
    }


    int getImageMidsectionWidth() const {
        return m_imageMidsectionWidth;
    }

    void setImageMidsectionWidth(int s) {
        m_imageMidsectionWidth = s;
    }



    void setMainText(QString s){
        m_currentEdit.setText(s);
        adjustForTextChange();
    }


    void setExplanationText(QString s){
        m_explanationText.setText(s);
        m_explanationText.show();
        m_explanationText.raise();
        adjustForTextChange();
    }

    void moveTextBelow(){
        //EndAnimateToBelow(); //Just in case
        QString s = m_currentEdit.text();
        m_editBelow.setPlainText(s);
        m_currentEdit.setText("");
        adjustForTextChange();
        //BeginAnimateToBelow();

    }

    void clearTextBelow(){
        m_editBelow.setText("");
        adjustForTextChange();
    }

    QString textBelow(){
        return m_editBelow.toPlainText();
    }

    void adjustForTextChange();
//    void BeginAnimateToBelow();
//    void EndAnimateToBelow();


    void setIcon(QIcon i){
        m_mainIconLabel.setPixmap(i.pixmap(getIconSize()));
    }

    int desireHeight(){
        int h = m_currentEdit.height();
        h = MAX(h, m_backgroundLabel.minimumHeight());
        return h;
    }


public slots:
    void rectangleResizing(){
        adjustForTextChange();
    }

private:
    MyFullTextEdit m_currentEdit;
    MyDisplayItem m_explanationText;
    QTextEdit m_editBelow;

    QLabel m_mainIconLabel;
    QLabel m_actionIconLabel;
    FancyLabel m_backgroundLabel;



};

#endif // MULTITEXTDISPLAY_H
