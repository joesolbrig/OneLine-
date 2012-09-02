#ifndef MULTITEXTDISPLAY_H
#define MULTITEXTDISPLAY_H

#include <QWidget>
#include <QHash>
#include <QUrl>
#include <QIcon>

#include <qapplication.h>

#include "constants.h"
#include "fancylabel.h"
#include "expression_icon_window.h"
#include "myfulltextedit.h"

QString titlize(QString str);

class MultiTextDisplay :public QWidget{
private:
    Q_OBJECT
    Q_PROPERTY(QImage BackgroundImage READ getBackgroundImage WRITE setBackgroundImage)
    QImage m_backgroundImage;
    Q_PROPERTY(int IconSize READ getIconSize WRITE setIconSize)
    int m_iconSize;
    Q_PROPERTY(float IconRatio READ getIconRatio WRITE setIconRatio)
    float m_iconRatio;
    Q_PROPERTY(int horizontalMargin READ getRightMargin WRITE setRightMargin)
    int m_horizontalMargin;
    Q_PROPERTY(int margin READ getBottomMargin WRITE setMargin)
    int m_verticalMargin;

    Q_PROPERTY(int backgroundCutHeight READ getBackgroundCutHeight WRITE setBackgroundCutHeight)
    int m_backgroundCutHeight;

    Q_PROPERTY(int imageMidsectionWidth READ getImageMidsectionWidth WRITE setImageMidsectionWidth)
    int m_imageMidsectionWidth;

    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

private:
    MyFullTextEdit m_mainEdit;
    MyDisplayItem m_explanationLabel;

    ExpressionIconWindow m_meaningIconWidget;
    FancyLabel m_backgroundLabel;

    qreal m_explanationFieldAdjust;
    qreal m_explanationTextWidth;


public:

    //We'll assume the text is light so it won't show up if the background's opaque'd
    int invisibleAreaHeight();
    void setFlat(bool flat=true);
    int visibleDesiredHeight();
    int totalDesiredHeight();
    void placeExplanationText();
    void setOpacity(qreal o);
    qreal getOpacity();
    bool takingEditKeys();
    void setVisible(bool visible);
    void setEditVisibility(bool visible);
    bool setKeyPress(QKeyEvent* key);
    void setKeyRelease(QKeyEvent* key);
    explicit MultiTextDisplay(QWidget* parent=0);
    void placeBackgroundLabel();
    void setParentHeight(int rectangleHeight);
    void setGeometry(QRect r);
    void show();
    QRect visibleRect();
    QPoint visibleTopLeft();
    QImage getBackgroundImage() const;
    void setBackgroundImage(QImage s);
    int getIconSize() const;
    void setIconSize(int s);
    float getIconRatio() const;
    void setIconRatio(float s);
    int getTopMargin() const ;
    int getBottomMargin() const;
    void setMargin(int s=0);
    int getLeftMargin() const ;
    int getRightMargin() const ;
    void setRightMargin(int s) ;
    int getBackgroundCutHeight() const ;
    void setBackgroundCutHeight(int s) ;
    int getImageMidsectionWidth() const ;
    void setImageMidsectionWidth(int s) ;
    void setMainPlainText(QString s, bool forceAdjust=false);
    void setFeedbackText(QString s);
    void clearText();
    bool hasText();
    void setMainHtmlText(QString html, QString s );
    QString getMainText();
    QString getMainHtmlText();
    void setExplanationText(QString formattedText, int keyFieldBegin=-1, int keyFieldEnd=-1);
    void adjustForTextChange(bool forceAdjust=false);
    void positionTextEdit();
    void sizeParentHeightsForChildren(bool forceAdjust);
    void setActiveIconPos(int pos);
    void placeIconWidget();
    void setExpressionIcon(QIcon i, int pos);
    int expressionIconCount();
    int activeExpressionIcon();
    void clearExpressionIcons();
    void popIcon();
    void animateIconRight();
    void animateIconLeft();
    bool getAtFull();
signals:
    void rectangleResizingSignal();
    void textChanged(QString s, QString t, int i);
public slots:
    void rectangleResizing();
    void textChangedSlot(QString s, QString t, int i);
};

#endif // MULTITEXTDISPLAY_H
