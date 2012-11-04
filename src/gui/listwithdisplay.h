#ifndef LISTWITHDISPLAY_H
#define LISTWITHDISPLAY_H

//#include "messageiconwidget.h"
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
//#include "iconspreadwindow.h"
//#include "../globals.h"

#include "previewpane.h"
#include "borderwindow.h"
#include "textmessagewidget.h"
#include "listinnerlayout.h"
#include "fancyenclosingrect.h"
#include "formdisplayer.h"

class MyListWidget;

class MultiInputDisplayer;

class ListInnerLayout;

#include "listwithframe.h"

//class InputManager;

class FormDisplayer;

class ListWithDisplay : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QRect baseGeometry READ getBaseGeometry WRITE setBaseGeometry)
    Q_PROPERTY(QRect animatedHeight READ getAnimatedHeight WRITE setAnimatedHeight)
    //Q_PROPERTY(int miniBarPosition READ getMiniBarPosition WRITE setMiniBarPosition)

    TextMessageBar* m_miniIconBar;
    //TextMessageBar m_iconBarOnStack;
    int m_autoHideIconBarPosition;
    bool m_showIconList;
    time_t m_timeOutsideBarArea;
    time_t m_timeInsideBarArea;
    QPropertyAnimation* m_miniIconAnimation;
    QRect m_baseGeometry;

    //Subframes...
    //ListWithFrame * m_stdFrame;
    typedef ListWithFrame* ListWithFramePtr;
    QList<ListWithFramePtr> m_viewStack;
    ListWithFrame * m_newFrame;
    QPropertyAnimation * m_upDownAnimation;


    //Size stuff....
    bool m_isTopOriented;
    QPoint m_base;
    int m_width;
    int m_height;
    int m_formHeight;
    int m_screenEdge;
    bool m_currentlyShowing;

    int m_selectedItem;
    QString m_bottomMessage;

    FormDisplayer* m_multiInputDisplayer;
    QMutex m_previewMutex;

public:
    ListWithFrame * m_previewFrame;

    void setGeometry(QRect r=QRect());
    void setFullGeometry(QRect r=QRect());

    bool showingSidePreview() {return (m_previewFrame!=0);}

    ListWithFramePtr& currentViewRef() {
        Q_ASSERT(m_viewStack.count()> 0);
        return m_viewStack[m_viewStack.count()-1];
    }

    ListWithFramePtr& baseViewRef()  {
        Q_ASSERT(m_viewStack.length()>0);
        return m_viewStack[0];
    }

    void addViewRef(){
        m_viewStack.append(0);
    }

    void popViewRef(){
        ListWithFrame* l = m_viewStack.last();
        delete l;
        m_viewStack.pop_back();
        Q_ASSERT(m_viewStack.length()>0);
    }

    ListWithDisplay();
    ~ListWithDisplay();


    //Called by child in kludge...
    void fakeMouseMoveEvent( QMouseEvent * event );

    void addSidePreview(ListItem contextItem, QList<ListItem> il);
    void addSidePreview(ListItem li);
    void removeSidePreview();
    void popSidePreview();
    void addSideExplore(QList<ListItem> il, int select);
    void popSideExplore();
    void collapseSideExplore();

    void setBaseGeometry(QRect r=QRect());
    QRect getBaseGeometry(){
        QRect r = QWidget::geometry();
        if(!m_multiInputDisplayer){
            r.setHeight(m_height);
            r.setWidth(m_width);
        } else {
            r.setHeight(m_formHeight);
            r.setWidth(m_width);
        }

        return r;
    }

    void setAnimatedHeight(QRect r);
    QRect getAnimatedHeight(){
        return geometry();
    }

    QRect getFocusRect();
    bool setMultiInput(InputList* il);
    bool endMultiInput();
    bool atMultiInput(){ return (m_multiInputDisplayer!=0);}
    void paintEvent ( QPaintEvent * event );

    void removeItem(CatItem item){
        if(m_viewStack.length()==0){return;}
        currentViewRef()->removeItem(item);
    }

    void doUpdate(){
        m_miniIconBar->doUpdate();
        if(m_viewStack.length()>0 && currentViewRef()){
            currentViewRef()->doUpdate();
        }
        QWidget::update();
    }

    void setHeight(int h);

    void moveUp(){
        if(m_viewStack.length()==0){return;}
        currentViewRef()->moveUp();
    }

    void moveDown(){
        if(m_viewStack.length()==0){return;}
        currentViewRef()->moveDown();
    }

    void setBaseAndOrientation(QPoint base, bool isTopOriented, int suggestedWidth, int screenEdge);

    void animateToLocation(QRect r);
    void addMiniIconList(QList<ListItem> items);
    void hideIconList();
//    int getIconPositions(){
//        if(!UI_MINI_ICON_ON_MAINFRM){
//            if(m_viewStack.length()==0){return 0;}
//            return currentViewRef()->getIconPositions();
//        }  else {
//            Q_ASSERT(m_miniIconBar);
//            return m_miniIconBar->itemPositions();
//        }
//    }

//    int getIconTextLength(){
//        if(!UI_MINI_ICON_ON_MAINFRM){
//            if(m_viewStack.length()==0){return 0;}
//            return currentViewRef()->m_layout->m_typeSorterBar->getTextCharacters();
//        } else {
//            return m_miniIconBar->getTextCharacters();
//        }
//    }

    void testShow(){
        if(m_currentlyShowing){
            setVisible(true);
        }
    }

    void showAnimated();

    void hideAnimated(){
        m_currentlyShowing = false;
        QRect r = this->geometry();
        r.setHeight(1);
        if(m_isTopOriented){
            r.moveTopLeft(m_base);
        } else {
            r.moveBottomLeft(m_base);
        }
        animateToLocation(r);
    }

    void delayedAnimateTic(){}


    void addMessage(QString msg);
    int getMessageSpaceAvailable();
    int getMessageItemCount();
    int getItemCount();
    void clear(){
        if(m_viewStack.length()==0){return;}
        if(currentViewRef()){
            currentViewRef()->clear();
        }
    }
    int getCurrentRow(){
        if(m_viewStack.length()==0){return -1;}
        if(atMultiInput()){return -1;}
        if(currentViewRef()){
            return currentViewRef()->getCurrentRow();
        }
        return -1;

    }
    void setCurrentRow(int row, QItemSelectionModel::SelectionFlags command);

    QListWidgetItem *currentItem() {
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->currentItem();
    }

    QListWidgetItem *currentOrHoverItem() {
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->currentOrHoverItem();
    }

    QListWidgetItem *hoverItem() {
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->hoverItem();
    }

    virtual int sizeHintForRow(int row){
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->sizeHintForRow(row);
    }

    QListWidgetItem* getItem(int i){
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->getItem(i);
    }

    void setCurrentRow(int r){
        if(m_viewStack.length()==0){return;}
        if(currentViewRef()){
            currentViewRef()->setCurrentRow(r);
        }
    }

    void setCurrentItem(CatItem item){
        if(m_viewStack.length()==0){return ;}
        currentViewRef()->setCurrentItem(item);;

    }

    int count(){
        if(m_viewStack.length()==0){return 0;}
        return currentViewRef()->count();
    }

    void parentKeyPress(QKeyEvent* k){
        if(m_viewStack.length()==0){return;}
        return currentViewRef()->parentKeyPress(k);
    }

    void parentKeyRelease(QKeyEvent* k){
        if(m_viewStack.length()==0){return;}
        return currentViewRef()->parentKeyRelease(k);
    }

    void addPreviewWindow(ListItem li, AbstractReceiverWidget* w);
    void endPreview();
    void setContextItem(CatItem contextItem);
    bool testIf_I_CanPreviewItem(CatItem ci);
    void trySetPreviewItem(ListItem li);
    void trySetPreviewItem(ListItem contextItem, QList<ListItem> il);


    QString currentPreviewUrl();

    void updateItem(ListItem item);


    //Same actions below, animated differently...
    void addChildList(QList<ListItem> il, int newSelection, CatItem contextItem=CatItem());
    void addSearchList(QList<ListItem> il);
    void addParentList(QList<ListItem> il, int newSelection, CatItem contextItem);

    void addSidePreview(QList<ListItem> li, int newSelection);

    void keyPressEvent(QKeyEvent* key) {
            emit listKeyPressed(key);
            key->ignore();
    }


    void mouseDoubleClickEvent( QMouseEvent * ) {
            QKeyEvent key(QEvent::KeyPress, Qt::Key_Enter, NULL);
            emit listKeyPressed(&key);
    }

    void focusOutEvent ( QFocusEvent * evt) {
        emit listLostFocus(evt);
    }


signals:
    void listKeyPressed(QKeyEvent*);
    void listLostFocus(QFocusEvent* evt);


public slots:
    void raise(){
        QWidget::raise();
        if(m_newFrame!=0){
            m_newFrame->raise();
        }
    }
    void iconBarAnimateDone(){
        Q_ASSERT(m_miniIconAnimation);
        delete m_miniIconAnimation;
        m_miniIconAnimation = 0;
    }


    void upDownAnimationDone(){
        if(!m_currentlyShowing){
            setVisible(false);
        }
        //showMiniIconBar(UI_MINI_ICON_BAR_HEIGHT);
        setBaseGeometry();
    }

    void sideAnimationDone();

    void previewLoaded();
    void setPreviewSearch(QString searchStr);


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
        connect(this, SIGNAL(finished()), parent,SLOT(sideAnimationDone()));
    }

    void updateCurrentTime(int currentTime){
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
        setDuration(STANDARD_ANIMATION_DURACTION);
        setLoopCount(1);
        start(QAbstractAnimation::DeleteWhenStopped);
    }

};


#endif // LISTWITHDISPLAY_H
