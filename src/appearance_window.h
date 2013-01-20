#ifndef APPEARANCE_WINDOW_H
#define APPEARANCE_WINDOW_H
/*
Oneline: Multibrowser
Copyright (C) 2012  Hans Joseph Solbrig

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QString>
#include <QBuffer>
#include <QScrollBar>
#include "plugin_handler.h"
#include "platform_util.h"
#include "edit_controls.h"
#include "catalog.h"
#include "icon_delegate.h"
//#include "globals.h"
#include "list_item.h"
#include "cat_builder.h"

#include "listwithframe.h"
#include "listwithdisplay.h"
#include "multiTextDisplay.h"

#include "itemarglist.h"
#include "previewpane.h"
#include "fancycontextmenu.h"

class InputList;
class QLineEditMenu;
class MessageIconWidget;
class MyListWidget;
class MultiTextDisplay;
//
//class TextMeasurer {
//private:
//    double m_length;
//public:
//    TextMeasurer(int length){
//        m_length = length;
//    }
//
//
//    virtual void addString(QString s){
//        m_length -= s.length();
//    }
//
//    virtual bool hasMoreSpace(QString s){
//        return ((m_length - s.length()) >=0);
//    }
//
//};
//

QIcon makeStandardIcon(QString name);
QIcon IconFromFile(QString name);
bool isImageFile(QFileInfo info);

class AbstractReceiverWidget;

class MainUserWindow : public AbstractReceiverWidget
{
        Q_OBJECT

public:

    MainUserWindow(QWidget *parent, PlatformBase*, bool rescue );
    ~MainUserWindow();
	//Application states
public:

    //
    bool st_AlwaysShow;
    bool st_AutoHideBar;

    //Toggle State variables ...
    bool st_ShowingSidePreview;
    bool st_ShowCustomFieldWindow;

    bool st_showItemList;
    bool st_ShowTaskBar;
    bool st_ListFilled;
    bool st_SearchResultsChanged;
    bool st_UserChoiceChanged;
    bool st_TakeItemFromList;
    bool st_ContextMenuOpen;
    bool st_Visible;
    bool st_OptionsOpen;
    bool st_EdittingCustomField;
    bool st_KeyFromEdit;
    bool st_updatingDisplay;
    bool st_tabChanged;

    bool st_altKeyDown;
    bool st_shiftKeyDown;

    bool st_customFieldTakesKey;
    bool st_previewHovered;

    QString m_backgroundSearchKeys;
    QString m_lastExtendText;

    int m_backgroundSearchKeywordCount;

    int m_vOrient;
    int m_hOrient;

    int m_savedPosition;
    QString m_desiredParentPath;
    QString m_positionBeingUpdated;

    time_t m_lastStateChange;
    time_t m_lastTimeItemsSeen;

    int m_oldSearchLevel;

    unsigned long m_timeCount;

    QString m_customFieldURL;


    enum ItemOrigin {
        FROM_SEARCH,
        FROM_CHILD,
        FROM_PARENT,
        AS_SIDEVIEW

    } m_itemOrigin;


    CatBuilder* m_builder;
    CatBuilder* m_searcher;

    QRect m_listRect;

    //Main UI Widgets...
    QPushButton *m_optsButton;
    QPushButton *m_closeButton;
    QRubberBand *m_rubberBander;

    MessageIconWidget* m_taskBarControl;

    FancyContextMenu* m_contextMenu;
    //
    //QTimer* updateExtendTimer;
    //QTimer* dropTimer;
    QTimer* m_choicesUnderstoodTimer;

    //Appearance...
    QPoint m_moveStartPoint;
    PlatformBase *platform;

    QHash<QString, QIcon> m_iconByMimetypeCache;
    QHash<QString, QIcon> m_iconByPathCache;
    
    int m_searchLevel;


public:
    //QLabel * licon;
    MultiTextDisplay* m_inputDisplay;
    InputList m_inputList;
    ListWithDisplay* m_itemChoiceList;

    ListItem m_centerPreviewingItem;
    ListItem m_rightPreviewingItem;

    void move(const QPoint &p);
    int invisibleAreaHeight();
    int desiredListWidth();

    QRect listRect(){
        if(!st_showItemList || !m_itemChoiceList){
            return QRect();
        }
        return m_itemChoiceList->getBaseGeometry();
    }

    QRect focusRect(){
        if(!st_showItemList){
            return QRect();
        }
        return m_itemChoiceList->getFocusRect();
    }

    QRect previewRect(){
        if(!st_showItemList){
            return QRect();
        }

        if(!m_itemChoiceList->m_previewFrame){
            return QRect();
        }
        return m_itemChoiceList->getBaseGeometry();
    }

    QSize sizeHint() const{
        QSize sz = AbstractReceiverWidget::sizeHint();
        int baseHeight = this->geometry().height();
        baseHeight = MAX(baseHeight , this->m_inputDisplay->totalDesiredHeight());
        sz.setHeight(baseHeight);
        return sz;
    }



    void clearModifierKeys();
    bool bottomOriented(){return m_vOrient!=0;}
    //QRect moveRectToAbutWindow(QRect itemRect);
    void addIconsToList(QList<ListItem>& li);
    QIcon getSpecificIcon(CatItem & item);
    QIcon getGenericIcon(CatItem & item);

    void applySkin(QString);
    void contextMenuEvent(QContextMenuEvent *event);
    void closeEvent(QCloseEvent *event);
    void showApp(bool now = true);
    void hideApp(bool now = false);
    void fadeIn();
    void fadeOut();

    bool setHotkey(int, int);
    virtual void showitemList();
    virtual void searchOnInput(int* beginPos=0);
    virtual void updateDisplay();
    virtual void hideItemList();
    QPoint setPosition(bool fromLocation=false);
    void setPositionFromSize(QSize size);
    void setAppearance();
    void savePosition() {
        SET_UI_VERTICAL_ORIENTATION(m_vOrient);
        SET_UI_HORIZONTAL_ORIENTATION(m_hOrient);
        gSettings->sync();
    }
    void processKey();
    bool backgroundSearch();
    bool foregroundSearch();
    bool searchFurtherItems();
    bool tryShowPreview();
    bool tryHidePreview();
    void hidePreview();
    void trySetFocus();
    void tryHideMenu();
    virtual bool refreshExtendCatalog(bool forceLoad=false, bool forceExtend=false);
    void extendSelectedItem();

    void startTasks();
    void startTimers();
    void preloadIcons();

    void fillList();
    void addMiniIcons();
    void moveListToSelectedItem();
    CatItem getSelectedItemFromList();

    void setItem();
    void setPathMessage();
    void setExpressionIcons();
    bool showCustomFieldWindow(CustomFieldInfo::FieldTransition fi = CustomFieldInfo::UNDETERMINED);
    void showTaskBar();
    bool processControlKey(QKeyEvent* controlKey);
    bool event ( QEvent * event );
    bool keyPressed(QKeyEvent* key);
    bool keyReleased(QKeyEvent* key);
    //void textChanged(QString newText, QString curWord, int cursorLocation);
    QList<ListItem> relativeInformationFormatting(QList<CatItem> inItems);
    QList<CatItem> getItemContextOptions(CatItem item);


    void addListItems(QList<CatItem> inItems);
    void appendListItems(QList<CatItem> inItems);
    void getListDimension(int & charsAvail, int & columnAvail);
    void loadCatalog(bool refreshCatalog);
    void loadSources();

    bool takeAllText();
    bool tryExecuteCurrentItem();
    void executeItemByHotkey(QKeyEvent* hotkey);
    void activateItemByHotkey(QKeyEvent* hotkey);

    void setItemsSeen();

    void tabAction(int k, QKeyEvent* controlKey);
    bool arrowUpDown(int k, QKeyEvent* controlKey);
    bool expandInto(int k, QKeyEvent* controlKey);
    bool expandInto(CatItem& item, bool setSide=false);
    bool goBackFrom(int k, QKeyEvent* controlKey);
    void popGuiState();

    QString userKeys(){return this->m_inputList.getUserKeys();}

    bool shouldProcessControlKey(QKeyEvent* key);

    void fillBuilderInfo(CatBuilder* builder);


public slots:
    void previewWindowGainedFocus();
    void previewWindowHovered(bool hovered);
    void textChanged(QString newText, QString /* curWord */, int cursorLocation);
    void choiceListKeyEvent(QKeyEvent*);
    void rebuildCatalog();
    void extendCatalog();
    void deleteAndZeroCatBuilder();
    void deleteAndZeroCatSearch();

    void keyPressEvent(QKeyEvent*);

    void menuOptions();
    void onHotKey();
    void choiceUnderstoodTimeout();
    void setAlwaysShow(bool);
    void setAlwaysTop(bool);
    void focusInEvent(QFocusEvent* evt);
    void focusOutEvent(QFocusEvent* evt);
    void setOpaqueness(int val);
    void menuEvent(QContextMenuEvent*);
    void listMenuEvent(QString itemPath, QPoint p);
    void inputKeyPressEvent(QKeyEvent* key);
    void inputMethodEvent(QInputMethodEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void catalogBuilt(CatBuilder*);
    void backgroundSearchDone(CatBuilder* searcher, QString searchString);

    void rectangleResizing();
    void miniIconClicked(ListItem, bool);
    void urlChanged(const QUrl &);


    void itemsUpdatedFromBackground(QList<CatItem>);

    void currentItemChangedSlot(QListWidgetItem *, QListWidgetItem * );
    void operateOnItem(QString path, const CatItem operationItem);

};

#endif
