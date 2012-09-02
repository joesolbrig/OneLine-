/*
Oneline: Multi-Browser and Application Launcher
Copyright (C) 2012 Hans Solbrig

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


//#include "iconspreadwindow.h"

#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QPalette>
#include <QLineEdit>
#include <QPixmap>
#include <QBitmap>
#include <QLabel>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTranslator>

#include "icon_delegate.h"
#include "list_item.h"
#include "input_list.h"
#include "appearance_window.h"
#include "catalog.h"
#include "cat_builder.h"
#include "globals.h"
#include "main.h"
#include "options.h"
#include "is_test.h"
#include "fancyHorizontallabel.h"
#include "messageiconwidget.h"


MainUserWindow::MainUserWindow(QWidget *parent,  PlatformBase * plat, bool rescue)
:  AbstractReceiverWidget(parent, Qt::FramelessWindowHint )  { //| Qt::Tool
    gMainWidget = this;
    gMarginWidget = (MarginWindow*)parent;
    m_lastTimeItemsSeen=0;

    //nothing should use Qt::WindowType_Mask;

    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_InputMethodEnabled);
    //setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0);

    platform = plat;

    m_itemOrigin = FROM_SEARCH;
    m_lastExtendText="";
    m_choicesUnderstoodTimer = 0;
    m_builder = 0;
    m_searcher=0;
    m_rubberBander =0;
    m_contextMenu=0;
    m_inputList.setAppState(MAIN_MENU_APP_STATE);
    m_savedPosition = -1;
    m_oldSearchLevel = -1;

    Catalog::curFilterStr = "";
    st_ContextMenuOpen = false;
    st_OptionsOpen = false;
    st_AlwaysShow = false;
    st_showItemList = false;
    st_TakeItemFromList = false;
    st_SearchResultsChanged = false;
    st_UserChoiceChanged = false;
    st_ListFilled = false;
    st_ShowTaskBar = false;
    st_EdittingCustomField = false;
    st_ShowingSidePreview = false;
    st_updatingDisplay= false;
    st_customFieldTakesKey=false;
    st_tabChanged= false;
    st_altKeyDown=false;
    st_shiftKeyDown=false;
    st_ShowCustomFieldWindow=false;
    st_previewHovered=false;

    setObjectName("main");

    setFocusPolicy(Qt::StrongFocus);

    gSettings = new QSettings(USER_APP_INI, QSettings::IniFormat);
    QString stylePath = USER_APP_DIR + STYLE_DIR + STYLE_INI_FILE;
    gStyleSettings = new QSettings(stylePath, QSettings::IniFormat);

    platform->registerToGetActiveWindows(this);
    setAppearance();

    m_taskBarControl = new MessageIconWidget(this, ListItem());
    m_taskBarControl->setVisible(false);

    //* no parent = top level window!
    m_itemChoiceList = new ListWithDisplay();
    m_itemChoiceList->setObjectName("itemChoiceList");
    m_itemChoiceList->setVisible(false);

    connect(m_itemChoiceList, SIGNAL(listKeyPressed(QKeyEvent*)),
            this, SLOT(choiceListKeyEvent(QKeyEvent*)));

    m_inputDisplay = new MultiTextDisplay(this);
    m_inputDisplay->setObjectName("input");
    m_inputDisplay->setFeedbackText(UI_MAIN_GREETING);

    connect(m_inputDisplay, SIGNAL(rectangleResizingSignal()),
            (MainUserWindow*)this, SLOT(rectangleResizing()));
    connect(m_inputDisplay, SIGNAL(textChanged(QString, QString, int)),
            (MainUserWindow*)this, SLOT(textChanged(QString, QString, int)));

    m_optsButton = new QPushButton(m_inputDisplay);
    m_optsButton->setObjectName("opsButton");
    m_optsButton->setToolTip(tr("Options"));
    connect(m_optsButton, SIGNAL(pressed()),
            (MainUserWindow*)this, SLOT(menuOptions()));

    m_closeButton = new QPushButton(m_inputDisplay);
    m_closeButton->setObjectName("closeButton");
    m_closeButton->setToolTip(tr("Close"));
    connect(m_closeButton, SIGNAL(pressed()), qApp, SLOT(quit()));
    m_closeButton->raise();

    bool showAppFirstTime = false;

    m_vOrient = UI_VERTICAL_ORIENTATION;
    m_hOrient = UI_HORIZONTAL_ORIENTATION;
    m_inputDisplay->move(QPoint(0,0));
    setGeometry(m_inputDisplay->geometry());

    //gSetAppPos(m_vOrient,m_hOrient);
    setPosition();

    // Set the general options
    setAlwaysShow(gSettings->value("GenOps/alwaysshow", false).toBool());
    setAlwaysTop(gSettings->value("GenOps/alwaystop", false).toBool());

    // Set the hotkey
#ifdef Q_WS_WIN
    int curMeta = gSettings->value("GenOps/hotkeyModifier", Qt::AltModifier).toInt();
#endif
#ifdef Q_WS_X11
    int curMeta = gSettings->value("GenOps/hotkeyModifier", Qt::ControlModifier).toInt();
#endif
    int curAction = gSettings->value("GenOps/hotkeyAction", Qt::Key_Space).toInt();
    if (!setHotkey(curMeta, curAction)) {
        QMessageBox::warning(this, tr("Oneline"), tr("The hotkey you have chosen is already in use. Please select another from preferences."));
        rescue = true;
    }

    adjustSize();
    if (showAppFirstTime || rescue || true){
        st_Visible = true;
        showApp();
    }
    else {
        st_Visible = false;
        hideApp();
    }

    Q_ASSERT(gMainWidget->takeAllText());
}

//
MainUserWindow::~MainUserWindow() {
    if(m_builder) {
        m_builder->wait(100000);
        if(m_builder){
            m_builder->quit();
        }

    }
    if(m_searcher){
        m_searcher->wait(100000);
        if(m_searcher){
            m_searcher->quit();
        }
    }

    m_itemChoiceList->clear();

    if(m_choicesUnderstoodTimer){
        delete m_choicesUnderstoodTimer;
    }
    if (platform){
        delete platform;
    }
    CatBuilder::destroyCatalog();
    QApplication::instance()->quit();

}

void MainUserWindow::move(const QPoint &p){
    QWidget::move(p);
    m_inputDisplay->move(QPoint(0,0));
    m_inputDisplay->raise();

}
int MainUserWindow::invisibleAreaHeight(){
    int height = m_inputDisplay->invisibleAreaHeight();
    Q_ASSERT(height>=0);
    return height;
}


int MainUserWindow::desiredListWidth(){
    return geometry().width() - (
                UI_BORDER_WINDOW_LEFT_MARGIN+UI_BORDER_WINDOW_RIGHT_MARGIN);
}


bool MainUserWindow::takeAllText(){
    return  !m_inputDisplay->takingEditKeys();
}

void MainUserWindow::fadeIn(){
    QPropertyAnimation *animation = new QPropertyAnimation(m_inputDisplay, "opacity");
    animation->setDuration(UI_FADEIN_TIME);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

void MainUserWindow::fadeOut(){
    QPropertyAnimation *animation = new QPropertyAnimation(m_inputDisplay, "opacity");
    animation->setDuration(UI_FADEIN_TIME);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
}

bool MainUserWindow::setHotkey(int meta, int key) {
	QKeySequence keys = QKeySequence(meta + key);
    return platform->SetHotkey(keys, gMainWidget, SLOT(onHotKey()));
}

void MainUserWindow::rectangleResizing(){

    //int primary = qApp->desktop()->primaryScreen();
    //QRect scr = qApp->desktop()->availableGeometry(primary);
    setFocus();

}

void MainUserWindow::setAppearance(){
    QHash<QString, QList<QString> >& dirs = *gDirs;
    QString key = (dirs["defSkin"])[0];
    QString dir = gSettings->value("GenOps/skin", key).toString();
    // Load the style sheet
    if (QFile::exists(dir + "/style.qss")) {
        QFile file(dir + "/style.qss");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString styleSheet = QLatin1String(file.readAll());
            // This is causing the ::destroyed connect errors
            qApp->setStyleSheet(styleSheet);
            file.close();
        }
    }
}

QPoint MainUserWindow::setPosition(bool fromLocation) {
    Q_ASSERT(m_inputDisplay);

    QRect appArea = gAppArea();
    QRect scrnArea = gScreenArea();
    if(fromLocation){
        Q_ASSERT(m_rubberBander);
        QPoint centerPt = m_rubberBander->geometry().center();
        qDebug() << "centerPt" << centerPt;
        qDebug() << "scr" << appArea;
        int newV_Orient = MIN((centerPt.y()*3)/(MAX(scrnArea.height(),1)),2);
        int newH_Orient = MIN((centerPt.x()*3)/(MAX(scrnArea.width(),1)),2);
        qDebug() << "newV_Orient" << newV_Orient;
        qDebug() << "newH_Orient" << newH_Orient;
        if(newV_Orient == m_vOrient && newH_Orient == m_hOrient ){
            return this->pos();
        }
        m_vOrient = newV_Orient;
        m_hOrient = newH_Orient;
    }

    Q_ASSERT(m_vOrient >-1 && (m_vOrient <3));
    Q_ASSERT(m_hOrient >-1 && (m_hOrient <3));
    m_inputDisplay->adjustForTextChange(true);


    m_itemChoiceList->setHeight(scrnArea.height()/LIST_SCREEN_RATIO);
    if(m_vOrient ==0){
        QPoint p(geometry().left()+UI_TOP_MARGIN, geometry().bottom());
        m_itemChoiceList->setBaseAndOrientation(p,true, this->width() - UI_TOP_MARGIN*2,appArea.right());
    } else {
        QPoint p(geometry().left()+ UI_TOP_MARGIN, geometry().top());
        m_itemChoiceList->setBaseAndOrientation(p,false, this->width() - UI_TOP_MARGIN*2, appArea.right());
    }
    m_optsButton->setGeometry(OPT_BUTTON_RATIO*width(),m_inputDisplay->visibleTopLeft().y(),
                              OPT_BUTTON_WIDTH,OPT_BUTTON_HEIGHT);

    m_closeButton->setGeometry(CLOSE_BUTTON_RATIO*width(),+ m_inputDisplay->visibleTopLeft().y(),
                              CLOSE_BUTTON_WIDTH,OPT_BUTTON_HEIGHT);
    return this->pos();
}

void MainUserWindow::setPositionFromSize(QSize size){

    QRect rect = this->geometry();
    //int winTop = this->pos().y();
    rect.setSize(size);

    QRect appArea=gAppArea();

    if(m_hOrient == 0){
        rect.moveLeft(appArea.left());
    } else if(m_hOrient == 1) {
        rect.moveCenter(appArea.center());
    } else {
        Q_ASSERT(m_hOrient==2);
        rect.moveRight(appArea.right());
    }

    if(m_vOrient ==0){
        rect.moveTop(appArea.top());
    } else if(m_vOrient ==1){
        //move BOTH vertical **AND** horizontal - we're OK with this
        rect.moveCenter(appArea.center());
    } else {
        Q_ASSERT(m_vOrient ==2);
        rect.moveBottom(appArea.bottom());
    }
    qDebug() << "****Move LWinto rect: " << rect;
    setGeometry(rect);
    gSetAppPos(m_vOrient,m_hOrient);

    if(m_vOrient ==0){
        QPoint p(geometry().left()+UI_TOP_MARGIN, geometry().bottom());
        m_itemChoiceList->setBaseAndOrientation(p,true, desiredListWidth(),appArea.right());
    } else {
        QPoint p(geometry().left()+ UI_TOP_MARGIN, geometry().top());
        m_itemChoiceList->setBaseAndOrientation(p,false, desiredListWidth(), appArea.right());
    }
}


void MainUserWindow::setOpaqueness(int val)
{
    double max = (double) val;
    max /= 100.0;
    gMainWidget->setWindowOpacity(max);
    //platform->SetAlphaOpacity(max);
}


void MainUserWindow::setAlwaysShow(bool alwaysShow) {
    st_AlwaysShow = alwaysShow;
    if (!isVisible()  && alwaysShow){
            showApp();
    }
}

void MainUserWindow::setAlwaysTop(bool alwaysTop) {
    if (alwaysTop)	{
        setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint);
    }
    else if ((windowFlags() & Qt::WindowStaysOnTopHint) != 0) {
        setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
}

void MainUserWindow::closeEvent(QCloseEvent *event) {
	//	gSettings->setValue("Display/pos", relativePosition());
    savePosition();
    gSettings->sync();
    gStyleSettings->sync();

    CatBuilder::storeCatalog();

    // Delete the platform (to unbind hotkeys) right away
    // else XUngrabKey segfaults if done later

    if (platform){
        delete platform;
        platform = 0;
        gPlatform = 0;
    }

    event->accept();
    qApp->quit();
}

void MainUserWindow::contextMenuEvent(QContextMenuEvent *event) {
    st_ContextMenuOpen = true;
    QMenu menu(gMainWidget);
    QAction* extendMenu = menu.addAction(tr("Extend Catalog"));
    connect(extendMenu, SIGNAL(triggered()), gMainWidget, SLOT(extendCatalog()));
    QAction* actMenu = menu.addAction(tr("Rebuild Catalog"));
    connect(actMenu, SIGNAL(triggered()), gMainWidget, SLOT(rebuildCatalog()));
    QAction* actOptions = menu.addAction(tr("Options"));
    connect(actOptions, SIGNAL(triggered()), gMainWidget, SLOT(menuOptions()));
    QAction* actExit = menu.addAction(tr("Exit"));
    connect(actExit, SIGNAL(triggered()), gMainWidget, SLOT(close()));
    menu.exec(event->globalPos());
    st_ContextMenuOpen = false;
}

void MainUserWindow::menuOptions() {
    st_showItemList = false;
    hideItemList();
    st_OptionsOpen = true;
    OptionsDlg ops(gMainWidget);
    ops.setObjectName("options");
    ops.exec();

    // Perform the database update
    refreshExtendCatalog();
    m_inputDisplay->raise();
    m_inputDisplay->show();

    setFocus();
    st_OptionsOpen = false;
}

void MainUserWindow::onHotKey() {
    if (st_ContextMenuOpen || st_OptionsOpen) {
        return;
    }

    if (st_Visible) {
        st_Visible = false;
        hideApp();
    } else {
        st_Visible = true;
        showApp();
    }
}

void MainUserWindow::addIconsToList(QList<ListItem>& il){
    for(int i=0; i<il.count();i++){
        if(il[i].getActualIcon().isNull()){
            QIcon ic = getSpecificIcon(il[i]);
            if(ic.isNull()){
                ic = getGenericIcon(il[i]);
            }
            il[i].setActualIcon(ic);
        }
    }
}



QIcon MainUserWindow::getSpecificIcon(CatItem & item) {

    if(m_iconByPathCache.count() > MAX_ICON_CACHE_SIZE)
        { m_iconByPathCache.clear(); }
    if(m_iconByMimetypeCache.count() > MAX_ICON_CACHE_SIZE)
        { m_iconByMimetypeCache.clear(); }

    //If we're asigned an icon already
    if(item.hasEmbeddedIcon()){
        ListItem li(item);
        return li.getActualIcon();
    }

    QIcon icon;

    QString iconPathOrName;
    bool isImage=false;

    if(isImageFile(item.getPath())) {
        iconPathOrName = item.getPath();
        isImage =true;
    }
    if(m_iconByPathCache.contains(iconPathOrName)){
        return m_iconByPathCache[iconPathOrName];
    }

    if(iconPathOrName.isEmpty()){
        iconPathOrName = item.getIcon();
        isImage =false;
    }
    if(m_iconByPathCache.contains(iconPathOrName) ){
        return m_iconByPathCache[iconPathOrName];
    }

    if(iconPathOrName.isEmpty() && item.isOrdinaryVerb()){
        iconPathOrName = item.getPath();
    }

    if (!iconPathOrName.isEmpty()) {

        if(item.isOrdinaryVerb()){
            gPlatform->alterItem(&item,true);
            ListItem li(item);
            icon = li.getActualIcon();
        }
        if(icon.isNull()) {
            QFileInfo f(item.getIcon());
            if (iconPathOrName[0] == CHAR_PATH_SEP && f.exists()) {
                icon = IconFromFile(iconPathOrName);
            } else if(QIcon::hasThemeIcon(iconPathOrName)){
                icon = makeStandardIcon(iconPathOrName);
            }
        }
        if(!icon.isNull() ){
            if(isImage){
                m_iconByPathCache[iconPathOrName] = icon;
            }
            return icon;
        }
    }

    QString mimeType = item.getMimeType();
    if(mimeType.isNull()){
        CatItem actionItem = gPlatform->alterItem(&item,true);
        mimeType = actionItem.getMimeType();
    }


    if(!mimeType.isNull() ){
        if(m_iconByMimetypeCache.contains(mimeType)){
            return m_iconByMimetypeCache[mimeType];
        }
        icon =  gPlatform->iconFromMime(mimeType);
    }


    if (icon.isNull() && QFile::exists(item.getPath())) {
        qDebug() << "trying to get icon from file path : " << item.getPath();
        icon = gPlatform->icon(QDir::toNativeSeparators(item.getPath()));
    }

    if(!mimeType.isNull() && ((int)item.getItemType() >= (int)CatItem::LOCAL_DATA_DOCUMENT)){
        if(!m_iconByMimetypeCache.contains(mimeType)){
            if(item.hasEmbeddedIcon()){
                ListItem li(item);
                icon = li.getActualIcon();
            }
        }
        if(!icon.isNull() && !isImage){
            m_iconByMimetypeCache[mimeType] = icon;
        }
    }

    return icon;

}

QIcon MainUserWindow::getGenericIcon(CatItem & item) {
    QIcon icon;

    if (!item.hasLabel(FILE_CATALOG_PLUGIN_STR) ) {
        //qDebug() << "generic non-file doc : " << item.getPath();
        icon = makeStandardIcon(GENERIC_DOCUMENT_ICON_NAME);
    }

    if (icon.isNull() && item.hasLabel(FILE_CATALOG_PLUGIN_STR) && !QFile::exists(item.getPath())) {
        qDebug() << "use missing file icon for: " << item.getPath();
        icon = makeStandardIcon(FILE_MISSING_ICON_NAME);
    }

    if(item.getItemType() == CatItem::PERSON){
        icon = makeStandardIcon(PERSON_TYPE_ICON);
    }

    if(item.getItemType() == CatItem::MESSAGE){
        icon = makeStandardIcon(MESSAGE_TYPE_ICON);
    }

    if(item.getItemType() == CatItem::PUBLIC_POST){
        icon = makeStandardIcon(FEED_TYPE_ICON);
    }

    if(item.getItemType() == CatItem::TAG || ((int)item.getTagLevel())>(int)( CatItem::POSSIBLE_TAG)){
        icon = makeStandardIcon(TAG_ICON_NAME);
    }

    if (icon.isNull() ) {
        icon = makeStandardIcon(GENERIC_DOCUMENT_ICON_NAME);
    }

    return icon;
}

void MainUserWindow::showApp(bool now) {
    gMarginWidget->show();
    hideItemList();
    activateWindow();

    gMainWidget->show();
    if (!now) {
        fadeIn();
    }
    m_inputDisplay->raise();
    m_inputDisplay->show();
    qApp->syncX();
    // Let the plugins know
    Catalog::showTheApp();
    setFocus();
}

void MainUserWindow::hideApp(bool/* now*/) {
    if (!isVisible()) {return;}
    savePosition();
    if (st_AlwaysShow) {return;}

    st_showItemList = false;
    hideItemList();
    m_inputDisplay->setVisible(false);

//    if (!now) {
//        fadeOut();
//    }
    hide();


    // let the plugins know
    Catalog::hideTheApp();
    gMarginWidget->hide();
}
