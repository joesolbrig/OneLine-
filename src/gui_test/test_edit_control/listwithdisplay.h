#ifndef LISTWITHDISPLAY_H
#define LISTWITHDISPLAY_H

#include <QFont>
#include <QListWidget>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QTextDocument>
#include <QPainter>
#include <QTextLayout>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QTextStream>
#include "item.h"
#include "list_item.h"
#include "icon_delegate.h"
#include "edit_controls.h"



class ListWithDisplayHolder : public QBoxLayout {
    Q_OBJECT
private:
    QLabel* topLabel;
    QLabel* bottomLabel;
    MyListWidget* m_realItemList;

public:
    ListWithDisplayHolder(QWidget* parent) :
            QBoxLayout(QBoxLayout::TopToBottom, parent){
        topLabel = new QLabel(parent);
        topLabel->setText("Hi");
        addWidget(topLabel);
        m_realItemList = new MyListWidget(parent);
        addWidget(m_realItemList);
        bottomLabel = new QLabel(parent);
        bottomLabel ->setText("Ho");


        m_realItemList->setObjectName("itemChoiceList");
        m_realItemList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_realItemList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_realItemList->setTextElideMode(Qt::ElideLeft);
        //m_realItemList->setWindowFlags(Qt::Tool );
        m_realItemList->setVisible(true);


        IconDelegate* listDelegate = new IconDelegate(this);
        m_realItemList->setItemDelegate(listDelegate);

        addWidget(bottomLabel);
    }

    ~ListWithDisplayHolder(){
        delete topLabel;
        delete bottomLabel;
        delete m_realItemList;
    }

    void addTopicMessage(QString msg){
        topLabel->setText(msg);

    }

    void setVisible(){


    }

    void addNavigationMessage(QString msg){
        bottomLabel->setText(msg);

    }
    virtual int sizeHintForRow(int row){
        return m_realItemList->sizeHintForRow(row);
    }

    void addItem(ListItem itm){
        m_realItemList->addItem(
                menuItFromListItem(itm, charsWidth()));
    }

    void addItemList(QList<ListItem> il){


        IconDelegate* listDelegate = new IconDelegate(this);
        if(il.size() > 0){
            listDelegate->setDelegateFont(il[0].getDisplayFont());
        } else {
            listDelegate->setDelegateFont(qApp->font());
        }

        m_realItemList->setItemDelegate(listDelegate);



        int num = m_realItemList->count();
        for(int i = 0; i < num; ++i) {
            QListWidgetItem * item = m_realItemList->takeItem(0);
            if (item != NULL) {delete m_realItemList->takeItem(0);}
        }

        int h = charsHeight();
        for(int i=0;i < il.count(); i++){
            m_realItemList->addItem( menuItFromListItem(il[i], h));
        }
    }

    QListWidgetItem * menuItFromListItem(ListItem ir, int charsAllowed);

    QListWidgetItem *currentItem() const{
        return m_realItemList->currentItem();

    }

    int charsWidth(){
        QFontMetrics fm =  m_realItemList->fontMetrics();
        float avDirCharWidth = fm.width("/mmmm/")/6;
        QRect r = m_realItemList->geometry();
        int charsAvail = r.width()/ avDirCharWidth;
        return charsAvail;
    }

    int charsHeight(){
        QFontMetrics fm =  m_realItemList->fontMetrics();
        float avDirCharHeight = fm.height();
        QRect r = m_realItemList->geometry();
        int rowsAvail = r.height()/avDirCharHeight;
        return rowsAvail;
    }


};


#include "listwithframe.h"

class InputManager;


class ListWithDisplay : public QWidget {
    Q_OBJECT
private:
    ListWithFrame * m_stdFrame;
    ListWithFrame * m_newFrame;
    InputManager* inputManager;


public:

    ListWithDisplay(QWidget* parent) : QWidget(parent){
        m_stdFrame = new ListWithFrame(this);
    }

    void animateToLocation(QRect r){
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(UI_LIST_RESIZE_TIME);
        QRect startR = this->geometry();
        animation->setStartValue(startR);
        animation->setEndValue(r);
        animation->start();
    }

    void addTopicMessage(QString msg){
        m_stdFrame->addTopicMessage(msg);
    }
    void addNavigationMessage(QString msg){
        m_stdFrame->addNavigationMessage(msg);

    }

    void setCurrentRow(int row, QItemSelectionModel::SelectionFlags command);
    QListWidgetItem *currentItem() const{
        return m_stdFrame->currentItem();

    }

    bool isActiveWindow(){
        return QWidget::isActiveWindow();
    }

    void setVisible(bool v=true){
        QWidget::setVisible(v);
    }



    virtual int sizeHintForRow(int row){
        return m_stdFrame->sizeHintForRow(row);
    }


    //Same operation, animated differently...
    void addChildList(QList<ListItem> il);

    void addParentList(QList<ListItem> il);
    void addRawList(QList<ListItem> il){
        m_stdFrame->addItemList(il);

    }


public slots:

    void animationDone(){

        ListWithFrame * tempFrame = m_stdFrame;
        Q_ASSERT(m_newFrame);
        m_stdFrame = m_newFrame;
        m_stdFrame->setVisible(true);

        tempFrame->setVisible(false);
        tempFrame->deleteLater();
    }

};

class FrameAnimation : public QPropertyAnimation {
public:
    enum animationDirection {
        BeginLeft,
        BeginRight
    };
private:
    QPainterPath m_path;
    ListWithFrame* m_target;
    ListWithFrame* m_itemToCover;
    int m_deflection;
    animationDirection m_ad;

public:

    FrameAnimation(ListWithFrame* target, ListWithFrame* itemToCover, QObject* parent, animationDirection ad)
        : QPropertyAnimation(target, "location") {
        m_target = (ListWithFrame*)target;
        m_path = QPainterPath();
        m_itemToCover = itemToCover;
        m_ad = ad;
        connect(this, SIGNAL(finished()), parent,SLOT(animationDone()));
    }

    //Assure circular movement...
    void updateCurrentTime(int currentTime)
    {
        if (m_path.isEmpty()) {
            QPointF from = startValue().toPointF();
            QPointF to = endValue().toPointF();
            m_path.moveTo(from);
            m_path.lineTo(to);
        }
        int dura = duration();
        const qreal progress = ((dura == 0) ? 1 : ((((currentTime - 1) % dura) + 1) / qreal(dura)));

        qreal easedProgress = easingCurve().valueForProgress(progress);
        QPointF pt = m_path.pointAtPercent(easedProgress);
        updateCurrentValue(pt);
        //emit valueChanged(pt);
    }

    void startAnimation() {

        QPointF p1 = m_target->getLocation();
        setStartValue(p1);
        QPointF p2 = m_itemToCover->getLocation();
        setEndValue(p2);
        setDuration(STANDARD_MOMENT/SIGNAL_MOMENT_RATIO);
        setLoopCount(1);
        start(QAbstractAnimation::DeleteWhenStopped);
    }

};


#endif // LISTWITHDISPLAY_H
