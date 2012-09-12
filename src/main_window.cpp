/*
OneLine: Information Browser and Application Launcher
Copyright (C) 2011 Hans Solbrig

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
#include "edit_controls.h"
#include "main.h"
#include "appearance_window.h"
#include "globals.h"
#include "options.h"
#include "plugin_interface.h"
#include "cat_builder.h"
#include "list_item.h"
#include "multiTextDisplay.h"
#include "listwithdisplay.h"
#include "listwithframe.h"
#include "fancycontextmenu.h"
#include "messageiconwidget.h"

bool isImageFile(QFileInfo info){
    QList<QByteArray> formats = QImageReader::supportedImageFormats();

    for(int i=0; i< formats.count();i++){
        //Optimization - string addition costly - this avoids it in the common case
        if (info.fileName().endsWith(formats[i], Qt::CaseInsensitive)) {
            if (info.fileName().endsWith(QString(".") + formats[i], Qt::CaseInsensitive))
                {return true;}
        }
    }

    if (info.fileName().endsWith(".ico", Qt::CaseInsensitive))
        { return true;}
    if (info.fileName().endsWith(".jpg", Qt::CaseInsensitive))
        { return true;}
    if (info.fileName().endsWith(".jpeg", Qt::CaseInsensitive))
        { return true;}
    if (info.fileName().endsWith(".gif", Qt::CaseInsensitive))
        { return true;}

    return false;
}

// The normal 'standard icon' of size 48
// look radically and unacceptably different. So I take the more
// consistant ones
QIcon makeStandardIcon(QString name){
    QIcon orig = QIcon::fromTheme(name);
    QIcon copy;
    if(!orig.isNull()){
        copy.addPixmap(orig.pixmap(QSize(16,16)));
        copy.addPixmap(orig.pixmap(QSize(22,22)));
        copy.addPixmap(orig.pixmap(QSize(24,24)));
        copy.addPixmap(orig.pixmap(QSize(32,32)));
        QList<QSize> sizes = orig.availableSizes();
        for(int i=0; i<sizes.count();i++){
            copy.addPixmap(orig.pixmap(sizes[i]));
        }
    }
    return copy;
}

// The normal 'standard icon' of size 48
// look radically and unacceptably different. So I take the more
// consistant ones
QIcon IconFromFile(QString name){
//    QFileInfo fi(name);
//    name = fi.canonicalFilePath();
//    Q_ASSERT(fi.exists());
//
//    QIcon orig(name);
//    QIcon copy;
//    if(!orig.isNull()){
//        copy.addPixmap(orig.pixmap(QSize(16,16)));
//        copy.addPixmap(orig.pixmap(QSize(22,22)));
//        copy.addPixmap(orig.pixmap(QSize(24,24)));
//        copy.addPixmap(orig.pixmap(QSize(32,32)));
//        copy.addPixmap(orig.pixmap(QSize(48,48)));
//        QList<QSize> sizes = orig.availableSizes();
//        for(int i=0; i<sizes.count();i++){
//            copy.addPixmap(orig.pixmap(sizes[i]));
//        }
//    }
//    return copy;

    QImageReader ir;
    QIcon icon;
    ir.setAutoDetectImageFormat(true);
//    QFileInfo fi(name);
//    Q_ASSERT(fi.exists());
    ir.setFileName(name);
    QSize sz = ir.scaledSize();
    if(sz.height() >48){
        sz.scale(48,0,Qt::KeepAspectRatio);
        ir.setScaledSize(sz);
    }
    QImage image = ir.read();
    if(image.isNull()){
        qDebug() << "err: " << ir.error() << "err string" << ir.errorString();
        return QIcon(name);
    }
    QPixmap pm;
    pm = QPixmap::fromImage(image);

    icon.addPixmap(pm);

    const int sizes[] = {16,22,24,32,48};

    for(unsigned int i=0; i<(sizeof (sizes))/(sizeof(int)); i++){
        Q_ASSERT(i <5);
        Q_ASSERT(sizes[i] <100 && sizes[i]>0);
        QPixmap temp = pm.scaledToHeight(sizes[i],Qt::SmoothTransformation);
        if(!temp.isNull()){
            icon.addPixmap(temp);
        }
    }

//    QPixmap temp = pm.scaledToHeight(16,Qt::SmoothTransformation);
//    if(!temp.isNull())
//        icon.addPixmap(temp);
//    temp = pm.scaledToHeight(22,Qt::SmoothTransformation);
//    if(!temp.isNull())
//        icon.addPixmap(temp);
//    temp = pm.scaledToHeight(24,Qt::SmoothTransformation);
//    if(!temp.isNull())
//        icon.addPixmap(temp);
//    temp = pm.scaledToHeight(32,Qt::SmoothTransformation);
//    if(!temp.isNull())
//        icon.addPixmap(temp);
//    temp = pm.scaledToHeight(48,Qt::SmoothTransformation);
//    icon.addPixmap(temp);
    return icon;
}



// Timer Processing below here
//

//We want to have the entire previous catalog available
//before we start and then incrementally add to it afterwards
void MainUserWindow::startTasks(){
    loadCatalog(false);
    loadSources();
    startTimers();
    preloadIcons();

    //Preload items
    QList<CatItem> outItems;
    InputList il;
    CatBuilder::getItemsFromQuery(il, outItems, MAX_ITEMS);
    gMarginWidget->gSetAppPos();

}

void MainUserWindow::preloadIcons(){
    QList<CatItem> verbs = CatBuilder::getMainApps();
    for(int i=0; i<verbs.count();i++){
        this->getSpecificIcon(verbs[i]);
    }
}

void MainUserWindow::startTimers(){
    // Set the timer(s)
    m_choicesUnderstoodTimer = new QTimer(this);
    connect(m_choicesUnderstoodTimer, SIGNAL(timeout()), this, SLOT(choiceUnderstoodTimeout()));
    //m_choicesUnderstoodTimer->start(BASE_TIMER_TIME*MILLISECS_IN_SECOND);
    m_choicesUnderstoodTimer->start(500);

}
// We want a single "control center" to start updates,
// and do explicit searches - this guarantee

//Runs every "1/N moments - fast enough to make search seem
//"instant" but slow enough to guarantee no bogging the system down
void MainUserWindow::choiceUnderstoodTimeout(){
    //Ought to be called only if the inputs haven't changed
    //
    time_t now = appGlobalTime(); //This is in conds...
    m_timeCount++; //This is arbitrary units

    //int timeSinceLastChange = (now - m_lastStateChange); //MILLISECS_IN_SECOND*

    //User Keys
        //explicit searching - this is the fastest thing
        { static int lastupdate=0;
        if(abs(now - lastupdate) > (FORGROUND_SEARCH_DELAY )){

            if(foregroundSearch()) {
                lastupdate = now;
            }
        }}

        //explicit searching - this is the fastest thing
        { static int lastupdate=0;
            if(abs(now - lastupdate) > (FORGROUND_SEARCH_DELAY*5)){

                if(searchFurtherItems()) {
                    lastupdate = now;
                }
            }}

        { static int lastupdate=0;
        if(abs(now - lastupdate) > (SHOW_PREVIEW_DELAY)){
            setItemsSeen();
        }}

        if(m_inputList.slotCount()>1){ return;}

        { static int lastupdate=0;
        if(abs(now - lastupdate)> (BACKGROUND_SEARCH_DELAY)){
            if(backgroundSearch()){
                lastupdate = now;
            }
        }}

    //visible item
        { static int lastupdate =0;
        if(abs(now - lastupdate)> VISIBLE_SOURCES_EXTEND_DELAY){
            extendSelectedItem();
            lastupdate = now;
        }}

        { static int lastupdate =0;
            if(abs(now - lastupdate) >= 1 && !st_ShowingSidePreview){
                tryShowPreview();
                lastupdate=now;
            } else {
                if(!tryHidePreview()){
                    lastupdate=now;
                }
            }
        }


    //Catalog extending
        { static int lastupdate =0;
        if(abs(now - lastupdate)> REFRESH_EXTEND_DELAY*10 ){
            if(refreshExtendCatalog()){
                lastupdate = now;
            }
        }}

    //UI switch to
    { static int lastupdate =0;
        if(abs(now - lastupdate)> DELAYED_UI_EFFECT_TIC ){
            //m_itemChoiceList->delayedAnimateTic();
            QListWidgetItem* index = m_itemChoiceList->hoverItem();
            if(index){
                QString itemPath = index->data(ROLE_ITEM_PATH).toString();
                CatItem it = m_inputList.getItemByPath(itemPath);
                if(!it.isEmpty()){
                    m_inputList.setExpanded(false);
                    m_inputList.setItem(itemPath);
                    m_itemChoiceList->setCurrentItem(it);
                    st_TakeItemFromList =true;
                    st_UserChoiceChanged=true;
                    setItem();
                    lastupdate = now;
                }
            }
        }}

//    //auto-show drop-down - not needed
//    { static int lastupdate =0;
//    if (abs(now - lastupdate)> SHOW_LIST_DELAY
//        &&  (m_inputList.getListItems().count() >0)){
//            st_ShowItemChoices = true;
//            updateDisplay();
//            lastupdate = now;
//    }}

}

void MainUserWindow::loadCatalog(bool refreshCatalog){

    if(refreshCatalog){
        refreshExtendCatalog(true);
        if(m_builder){
            m_builder->wait();
            delete m_builder;
        }
        m_builder = 0;
        refreshExtendCatalog(true, true);
        if(m_builder){
            m_builder->wait();
            delete m_builder;
        }
        m_builder = 0;
    } else {
        CatBuilder::getOrCreateCatalog(*gDirs);
    }
}

void MainUserWindow::loadSources(){
    CatBuilder::loadExtensionSourceForPlugins();
}

void MainUserWindow::rebuildCatalog(){
    refreshExtendCatalog(true);
    //refreshExtendCatalog(false, true);
}

void MainUserWindow::extendCatalog(){
    refreshExtendCatalog(false, true);
}

//We'll only do a full update when requested.
//Extending sources and looking recent items should do it...
bool MainUserWindow::refreshExtendCatalog(bool forceLoad, bool forceExtend){

    time_t now = appGlobalTime();
    if(m_inputList.slotCount()>1){ return false;}
    time_t last_update = gSettings->value(LAST_REFRESH_KEY, 0).toInt();
    CatBuilder::CatalogRefreshMode m;
    if(forceExtend){
        m = CatBuilder::EXTEND;
    } else if(forceLoad){
        m = CatBuilder::LOAD;
//    } else  if(last_update ==0 || (now - last_update > 60*60*24)){
//        m = CatBuilder::REFRESH;
    } else  if((now - last_update > 15*60)){
        m = CatBuilder::SAVE;
    } else {
        if(m_lastExtendText.length()>0 && m_lastExtendText != m_inputList.getUserKeys()){
            m_lastExtendText = m_inputList.getUserKeys();
            return false;
        }
        m = CatBuilder::EXTEND;
    }

    //CatalogRefreshMode m = (CatalogRefreshMode)gSettings->value(IS_SAVED_KEY, (int)CatalogRefreshMode::REFRESH);
    if(m_builder) { return false;}
    if(m_searcher) { return false;}
    m_builder = new CatBuilder(m, *gDirs);
    qDebug() << "Begin refreshExtend " << (int)m;
    connect(m_builder, SIGNAL(catalogFinished()), this, SLOT(catalogBuilt()));
    m_builder->m_userItems = m_inputList.getListItems();
    m_builder->m_userKeys = m_inputList.getUserKeys();
    m_builder->m_keywords = m_inputList.getKeyWords();
    m_builder->m_extensionType = UserEvent::STANDARD_UPDATE;

    m_builder->start(QThread::IdlePriority);
    return true;

}

void MainUserWindow::extendSelectedItem(){

    if(m_builder) { return;}
    if(m_searcher) { return;}
    QStringList sl = m_inputList.getKeyWords();
    if(sl.count()>0){ return;}

    QListWidgetItem *wi = m_itemChoiceList->currentItem();
    if(!wi){return;}

    QString itemPath = wi->data(ROLE_ITEM_PATH).toString();
    if(m_positionBeingUpdated==itemPath){ return; }
    m_positionBeingUpdated=itemPath;

    ListItem it = m_inputList.getItemByPath(itemPath);
    if(it.isEmpty()){ return;}
    if(!it.shouldUpdate(2, UserEvent::SELECTED)){ return;}
//    toUpdate.append(m_inputList.getStubbedItems());
    QList<CatItem> toUpdate;
    toUpdate.append(it);

    //if(toUpdate.isEmpty()){return;}

    m_builder = new CatBuilder(CatBuilder::EXTEND, *gDirs);
    connect(m_builder, SIGNAL(catalogFinished()), this, SLOT(catalogBuilt()));
    m_builder->m_userItems = toUpdate;
    m_builder->m_userKeys = m_inputList.getUserKeys();
    m_builder->m_keywords = m_inputList.getKeyWords();
    m_builder->m_extensionType = UserEvent::SELECTED;
    m_builder->m_extendDefaultSources = false;
    m_builder->start(QThread::IdlePriority);

}

void MainUserWindow::catalogBuilt() {

    time_t now = QDateTime::currentDateTime().toTime_t();
    gSettings->setValue(LAST_REFRESH_KEY, (int)now);
    if(!m_builder) { return;}

    if(m_builder->m_extensionType == UserEvent::SELECTED){
        CatItem& currentRef = m_inputList.currentItemRef();
        for(int i=0; i< m_builder->m_extension_results.count() && !currentRef.isEmpty(); i++){
            CatItem it = m_builder->m_extension_results[i];
            if(currentRef ==it){
                if(st_ShowingSidePreview and !(m_itemChoiceList->showingSidePreview())){
                    st_ShowingSidePreview = false;
                }
                currentRef.merge(it);
                qDebug() << "currentRef.. " << currentRef.getPath() << " currentRef.getPreviewHtml().isEmpty():" << currentRef.getPreviewHtml().isEmpty();
            }
        }
    }

    m_builder->wait();

    if(m_inputList.slotCount()==0){
        if(m_inputList.getListItems().count()==0){
            st_TakeItemFromList=false;
            searchOnInput();
        }
        appendListItems(m_builder->m_timelyItems);

        //Building the catalogue may add to current item
        CatItem& curItem = m_inputList.currentItemRef();
        if(!curItem.isEmpty()){
            CatBuilder::updateItem(curItem,2,UserEvent::SELECTED, true);
        }

        //Update files and list
        fillList();
        showitemList();
        m_itemChoiceList->update();
    }

    delete m_builder;
    m_builder = 0;

}

void MainUserWindow::setItemsSeen(){
    time_t now = appGlobalTime() ;
    time_t timeSinceLastChange = now - m_lastTimeItemsSeen;
    if(timeSinceLastChange < ITEM_SEEN_PERIOD){return; }

    QList<CatItem> searchResults = m_inputList.getListItems();
    if(st_showItemList){
        int visNumber = MIN(m_inputList.getListItems().count(),
            gSettings->value("GenOps/numviewable", "4").toInt());
        visNumber = MIN(searchResults.count(), visNumber);
        for(int i=0;i < visNumber; i++){
            CatBuilder::setShown(searchResults[i]);
        }
    }
    CatItem it = this->m_inputList.currentItem();
    if(!it.isEmpty()){
        CatBuilder::setShown(it, UserEvent::SHOWN_EDITLINE);
    }
    m_lastTimeItemsSeen = now;
}

bool MainUserWindow::backgroundSearch(){
    QStringList keyWords = m_inputList.getKeyWords();
    if(m_builder) { return false;}
    if(m_searcher) { return false;}
    if(m_inputList.fieldInputType()==InputList::USER_TEXT){ return true;}

    if(keyWords.count() > MAX_WORDS_FOR_SEARCH ){ return false;}
    if(keyWords.count() == m_backgroundSearchKeywordCount){ return false;}
    m_backgroundSearchKeywordCount = keyWords.count();

    if(keyWords.count() >0 && (m_backgroundSearchKeys!=m_inputList.getUserKeys())
            && m_inputList.slotCount()==1){
        m_backgroundSearchKeys = m_inputList.getUserKeys();
        if(m_searcher){ delete m_searcher; m_searcher=0;}
        m_searcher = new CatBuilder(CatBuilder::SEARCH, *gDirs);
        m_searcher->m_userKeys = m_inputList.getUserKeys();
        m_searcher->m_keywords = keyWords;
        m_searcher->m_userItems.append(m_inputList.getContextType(FILE_DIRECTORY_PLUGIN_STR));
        m_searcher->start(QThread::IdlePriority);
        connect(m_searcher, SIGNAL(backgroundSearchDone(QString )),
                this, SLOT(backgroundSearchDone(QString )));
    }
    return true;
}

bool MainUserWindow::foregroundSearch(){

    if(st_TakeItemFromList){
        return false;
    }
    if(st_UserChoiceChanged){
        if(m_inputList.isExpanded()){
            QString searchStr = m_inputList.getExpandedSearchStr();
            m_itemChoiceList->setPreviewSearch(searchStr);
        } else {
            searchOnInput();
            updateDisplay();
        }
        st_UserChoiceChanged = false;
        m_oldSearchLevel=0;
        return true;
    }
    return false;
}

bool MainUserWindow::searchFurtherItems(){

    if(!m_inputList.isExpanded() || !st_showItemList)
        { return false; }

    int row = m_itemChoiceList->getCurrentRow();
    int itemCount = m_inputList.getListItemCount();
    if(row<=0 || itemCount <=0){ return false;}

    if(m_inputList.getUserKeys().length()> 15 || m_inputList.getKeyWords().count()>1)
        { return true; }

    QString currentKeys = m_inputList.getUserKeys();
    int searchLevel = m_inputList.getSearchLevel();
    if(searchLevel <= m_oldSearchLevel){ return false;}
    if(row < (itemCount-2) ){ return false;}

    m_oldSearchLevel = searchLevel;
    searchOnInput(&searchLevel);
    //This should increment searchLevel if we found anything...
    if(currentKeys == m_inputList.getUserKeys()){
        m_inputList.setSearchLevel(searchLevel);
    }
    if(itemCount > m_inputList.getListItemCount()){
        updateDisplay();
        st_UserChoiceChanged = false;
    } else {
        //ends the search loop!!
        m_inputList.setSearchLevel(m_oldSearchLevel);
    }
    return true;

}

bool MainUserWindow::tryShowPreview(){
    if(!st_showItemList){ return false;}
    if(!st_ListFilled){ return false;}
    if(st_ShowingSidePreview) { return true; }

    QListWidgetItem* index =  m_itemChoiceList->currentOrHoverItem();;
    if(!index && !m_inputList.isExpanded()){ return true;}

    ListItem pItem;
    if(!m_inputList.isExpanded()){
        QString itemPath = index->data(ROLE_ITEM_PATH).toString();
        pItem = m_inputList.getItemByPath(itemPath);
        if(!(pItem==m_rightPreviewingItem)){
            m_rightPreviewingItem = pItem;
            return false;
        }
        CatBuilder::updateItem(pItem,2,UserEvent::SELECTED, true);
    } else {
        pItem = m_centerPreviewingItem;
    }

    //Only try to show it once...
    st_ShowingSidePreview = true;
    if(!m_inputList.isExpanded() && m_itemChoiceList->testIf_I_CanPreviewItem(pItem) ) {
        m_itemChoiceList->trySetPreviewItem(pItem);
        gMarginWidget->gSetAppPos();
    } else {
        QList<CatItem> childs;
        if(pItem.canFilterWith() && m_inputList.slotPosition()==0){
            InputList il;
            il.setItem(pItem);
            il.pushChildren(pItem);
            CatBuilder::getItemsFromQuery(il, childs, MAX_ITEMS);
        } else {
            childs = pItem.getRelatedItems();
        }
        if(childs.count() >0){
            QList<ListItem> listChilds;
            for(int i=0; i< childs.count(); i++){
                CatItem ch(childs[i]);
                if(m_inputList.acceptItem(&ch)){
                    ListItem li(ch);
                    listChilds.append(li);
                }
            }
            if(listChilds.count()>0){
                //st_ShowingSidePreview = true;
                addIconsToList(listChilds);
                m_itemChoiceList->trySetPreviewItem(pItem, listChilds);
                gMarginWidget->gSetAppPos();
                return true;
            }
        }
        return false;
    }
    return false;
}

bool MainUserWindow::tryHidePreview(){
    if(!st_ShowingSidePreview){ return true;}
    if(st_previewHovered){ return true;}
    if(m_inputList.isExpanded()){ return false;}
    if(st_showItemList){
        if(!st_ListFilled){ return true;}
        time_t now = appGlobalTime();
        if(m_itemChoiceList->m_previewFrame &&
                (m_itemChoiceList->m_previewFrame->m_hovered ||
                now - m_itemChoiceList->m_previewFrame->m_lastHover <= PREVIEW_FLOAT_SECS)){
            return true;
        }
        QListWidgetItem* index = m_itemChoiceList->currentOrHoverItem();
        if(index){
            QString itemPath = index->data(ROLE_ITEM_PATH).toString();
            if(m_rightPreviewingItem.getPath() == itemPath){
                return false;
            }
        }
    }
    hidePreview();
    return true;
}

void MainUserWindow::hidePreview(){
    m_rightPreviewingItem = ListItem();
    st_ShowingSidePreview = false;
    m_itemChoiceList->removeSidePreview();
}

//Event processing..........
void MainUserWindow::miniIconClicked(ListItem it, bool setTheItem){

    if(setTheItem){
        m_inputList.setFilterItem(it);
    } else {
        m_inputList.setFilterItem(ListItem());
    }
    st_ListFilled = false;
    m_inputList.setExpanded(false);
    st_ShowCustomFieldWindow = false;
    st_customFieldTakesKey = false;
    st_TakeItemFromList = false;
    //This will give a new item mini-icon list if necessary
    if(it.getOrganizingCharacteristic().isEmpty()){
        searchOnInput();
    } else {
        m_itemOrigin = FROM_SEARCH;
        st_SearchResultsChanged = true;
    }
    updateDisplay();
}

void MainUserWindow::focusOutEvent ( QFocusEvent * evt) {
    QWidget::focusOutEvent(evt);
    if(m_rubberBander){
        m_rubberBander->hide();
    }
    evt->accept();
    bool keepFocus = !(QApplication::focusWidget() ==0);
    if(!keepFocus && evt->reason() == Qt::ActiveWindowFocusReason){
        if (!st_AlwaysShow ) {
            if (ONELINE_ALWAY_VISIBLE) {
                if (!gMainWidget->isActiveWindow()
                        && !m_itemChoiceList->isActiveWindow()
                        && !st_OptionsOpen) {
                    st_Visible = false;
                    hideApp();
                }
            }
        }
        if(evt->lostFocus()){
            if(!QApplication::focusWidget()){ //st_ShowItemChoices && !itemChoiceList->hasFocus()
                //qDebug() << "(NOT)hiding list cause I lost focus...";
                //m_itemChoiceList->setVisible(false);
            }
            return;
        }
    } else {
        //My main window is greedy and ALWAYS wants the focus
        qDebug() << "lost focus 'cause" << evt->reason();
        if(!st_OptionsOpen){
            setFocus();
        }
    }
    savePosition();
}

void MainUserWindow::focusInEvent ( QFocusEvent *) {
    //clearModifierKeys();
    if(st_showItemList){
        qDebug() << "show list cause I gained focus...";
        m_itemChoiceList->setVisible(true);
    }
    setFocus();
}

void MainUserWindow::itemsUpdatedFromBackground(QList<CatItem> its){
    qDebug() << "itemsUpdatedFromBackground";
    int minWeight = 1000000;
    int currentRow = this->m_itemChoiceList->getCurrentRow();
    for(int i=0; i< its.count(); i++) {
        this->m_inputList.addSingleListItem(its[i],currentRow+1);
        minWeight = MIN(minWeight, its[i].getPositiveTotalWeight());
    }
    if(st_ShowTaskBar){
        Q_ASSERT(m_taskBarControl);
        QList<ListItem> itms= m_inputList.getOrganizingFilterItems(
                m_taskBarControl->itemPositions(), minWeight);
        m_taskBarControl->addItems(itms);
    }
}

void MainUserWindow::backgroundSearchDone(QString searchString)
{
    //skip if this is coming after keys have changed..
    qDebug() << "backgroundSearchDone called with:" << searchString;
    if((searchString ==m_inputList.getUserKeys()
        || m_inputList.getListItems().count()==0)&&
            (m_inputList.fieldInputType()!=InputList::USER_TEXT) && m_inputList.slotCount()==1){
        Q_ASSERT(m_searcher);
        m_searcher->wait();
        m_inputList.filterItems();
        appendListItems(m_searcher->m_extension_results);

        st_SearchResultsChanged = true;
        st_ListFilled = false;
        m_itemOrigin = FROM_SEARCH;
        updateDisplay();
        m_backgroundSearchKeys = searchString;
    }
    m_searcher->wait();
    delete m_searcher;
    m_searcher = 0;
}


//Getting and displaying items....
void MainUserWindow::searchOnInput(int* beginPos) {
    if(st_TakeItemFromList){return;}

    if(m_inputList.atFieldChoicePoint() &&
       m_inputList.getUserKeys().length()>0){
        CatItem it = getSelectedItemFromList();
        Q_ASSERT(!it.isEmpty());
        Q_ASSERT(it.getIsVerbArg());
        m_inputList.setItem(it);
    }

    if(m_inputList.fieldInputType()==InputList::USER_TEXT){
        m_itemChoiceList->update();
        return;
    }

    if( m_inputList.atCustomField()){ return; }

    QStringList keyWords = m_inputList.getKeyWords();
    if(keyWords.count() > MAX_WORDS_FOR_SEARCH ){ return;}

    QList<CatItem> outItems;
    //int visibleItems = this->m_itemChoiceList->getCurrentRow();
    m_inputList.filterItems();
    CatBuilder::getItemsFromQuery(m_inputList, outItems, MAX_ITEMS, beginPos);
    CatBuilder::getMiniIcons(m_inputList);

    st_SearchResultsChanged = true;
    st_ListFilled = false;
    m_itemOrigin = FROM_SEARCH;
    qDebug() << "------searchOnInput adding items";
    addListItems(outItems);
    addMiniIcons();

}

QList<CatItem> MainUserWindow::getItemContextOptions(CatItem item) {
    QList<CatItem> outItems;
    if(m_inputList.slotCount()==1){
        InputList subList;
        subList.setItem(item);
        subList.addSlot();
        CatBuilder::getItemsFromQuery(subList, outItems, 20);
    }
    return outItems;
}

void MainUserWindow::addListItems(QList<CatItem> inItems) {
    m_inputList.addListItemList(inItems);

}

void MainUserWindow::appendListItems(QList<CatItem> inItems){
    m_inputList.appendListItemList(inItems);
}


void MainUserWindow::getListDimension(int & charsAvail, int & columnsAvail){
    //QFont fnt = m_itemChoiceList->get

    QFont fnt = ListItem(m_inputList.currentItem()).getDisplayFont();
    QFontMetrics fm(fnt);
    float avDirCharWidth = fm.averageCharWidth();//fm.width("/mmmmWuiV/")/11;
    float avDirCharHeight = fm.height();
    QRect r = m_inputDisplay->geometry();
    charsAvail = r.width()/ avDirCharWidth;
    columnsAvail = r.height()/avDirCharHeight*3;
}

void MainUserWindow::currentItemChangedSlot(QListWidgetItem *, QListWidgetItem * ){
    st_TakeItemFromList = true;
    m_itemChoiceList->collapseSideExplore();
    updateDisplay();
}

CatItem MainUserWindow::getSelectedItemFromList(){
    QListWidgetItem *it = m_itemChoiceList->currentItem();
    if(it){
        qDebug() << "getSelectedItemFromList: " << it->text();
        QString itemPath = it->data(ROLE_ITEM_PATH).toString();
        qDebug() << "item path:" << itemPath;
        ListItem& item = m_inputList.getItemByPathRef(itemPath);
        if(!it->data(ROLE_LONG_BODY_TEXT).toString().isEmpty()){
            item.setLabel(HTML_PREVIEW_INLINED_KEY);
        }
        return item;
    } else { return CatItem(); }
}

void MainUserWindow::setItem(){
    if (!st_UserChoiceChanged){return;}
    if(m_inputList.atCustomField() ){
        showCustomFieldWindow((CustomFieldInfo::JUST_APPEARING));
        return;
    }

    if( st_TakeItemFromList) {
        CatItem it = getSelectedItemFromList();
        if(!it.isEmpty()){
            CatBuilder::updateItem(it, 2, UserEvent::SELECTED,false);
            m_inputList.setItem(it);
        }
    }

    int fieldBegin=-1;
    int fieldEnd=-1;
    QString outStr = m_inputList.getExplanationText(&fieldBegin, &fieldEnd);
    fieldBegin=-1;
    fieldEnd=-1;
    m_inputDisplay->setExplanationText(outStr, fieldBegin, fieldEnd);

    if(m_inputList.fieldInputType()==InputList::USER_TEXT ){
        if(!m_inputDisplay->takingEditKeys()){
            m_inputDisplay->setMainPlainText(m_inputList.getUserKeys());
        }
        return;
    }

    if(m_inputList.isExpanded()){
        if(!st_KeyFromEdit && !m_inputDisplay->takingEditKeys()){
            m_inputDisplay->setMainPlainText(m_inputList.getExpandedSearchStr());
        }
    } else if(m_itemChoiceList->atMultiInput()){
        QString txt = m_inputList.simpleItemText();
        m_inputDisplay->setMainHtmlText(txt, m_inputList.getUserKeys());

    } else if(m_inputList.showFormattedItem()){
        QString txt = m_inputList.mainItemText();
        m_inputDisplay->setMainHtmlText(txt, m_inputList.getUserKeys());
    } else {
        if(!st_KeyFromEdit && !m_inputDisplay->takingEditKeys()){
            m_inputDisplay->setMainPlainText(m_inputList.getUserKeys());
        }
    }
    setExpressionIcons();
    st_UserChoiceChanged = false;
    st_TakeItemFromList = false;
    addMiniIcons();
}


void MainUserWindow::setExpressionIcons(){
    if(st_tabChanged){
        st_tabChanged = false;
        return;
    }
    m_inputDisplay->clearExpressionIcons();

    QList<CatItem > items;
    for(int i=0; i< m_inputList.iconItemCount();i++){
        items.append(m_inputList.iconItem(i));
    }
    for(int i=0; i< items.count();i++){
        QIcon icon = getSpecificIcon(items[i]);
        if( icon.isNull() && (m_inputList.iconItem(0).getItemType() != CatItem::LOCAL_DATA_DOCUMENT
                ||  (i < m_inputList.iconItemCount()-1 && (m_inputList.iconItemCount() >0))
                || !icon.isNull()) ) {
            icon = getGenericIcon(items[i]);
        }
        if(!icon.isNull()){
            m_inputDisplay->setExpressionIcon(icon,i);
        }
    }

    m_inputDisplay->setActiveIconPos(m_inputList.slotPosition());

}

void MainUserWindow::moveListToSelectedItem(){
    m_itemChoiceList->setCurrentItem(m_inputList.currentItem());
}

void MainUserWindow::fillList(){
    if(!st_SearchResultsChanged) {return;}
    hidePreview();

    int charsAvail;
    int rowsAvail;
    getListDimension(charsAvail, rowsAvail);
    int selectIndex = -1;

    //charsAvail = m_itemChoiceList->getIconTextLength();
    QList<ListItem> il=
        m_inputList.getFormattedListItems(selectIndex,rowsAvail, charsAvail);
    addIconsToList(il);

    if(selectIndex<0){
        selectIndex = m_savedPosition;
    } else {
        m_savedPosition = selectIndex;
    }

    CatItem contextItem = m_inputList.getParentContextItem();
    if(m_itemOrigin == FROM_SEARCH){
        m_itemChoiceList->addSearchList(il);
        if(selectIndex>=0){
            m_itemChoiceList->setCurrentRow(selectIndex);
            //Some defensive programming but oh well...
            if(!m_desiredParentPath.isEmpty()){
                QListWidgetItem *it = m_itemChoiceList->currentItem();
                if(it && m_desiredParentPath !=it->data(ROLE_ITEM_PATH).toString()){
                    for(int i=0; i< m_itemChoiceList->count(); i++){
                        it = m_itemChoiceList->getItem(i);
                        if(it && m_desiredParentPath ==it->data(ROLE_ITEM_PATH).toString()){
                            m_itemChoiceList->setCurrentRow(i);
                            break;
                        }
                    }
                }
                m_desiredParentPath.clear();
            }
        }
        if(st_showItemList){
            m_itemChoiceList->showAnimated();
            m_inputDisplay->setFlat(true);
        }
    } else if(m_itemOrigin == FROM_CHILD){
        m_itemChoiceList->addParentList(il, selectIndex, contextItem);
        m_itemChoiceList->showAnimated();
            m_inputDisplay->setFlat(true);
    } else if(m_itemOrigin == FROM_PARENT){
        if(selectIndex<0){
            selectIndex = m_savedPosition;
        }
        m_itemChoiceList->addChildList(il, selectIndex, contextItem);
        m_itemChoiceList->showAnimated();
        m_inputDisplay->setFlat(true);
    } else if(m_itemOrigin == AS_SIDEVIEW){
        m_itemChoiceList->addSideExplore(il, selectIndex);
    } else {Q_ASSERT(false);}

    addMiniIcons();
    st_SearchResultsChanged  = false;
}

void MainUserWindow::addMiniIcons(){

    if(!m_inputList.isExpanded()){
        int charsAvail;
        int rowsAvail;
        getListDimension(charsAvail, rowsAvail);
        QList<ListItem> mi = m_inputList.getOrganizingFilterItems(10, charsAvail);
        m_itemChoiceList->addMiniIconList(mi);
    } else {
        m_itemChoiceList->addMiniIconList(QList<ListItem>());
    }
    setPathMessage();
}

void MainUserWindow::setPathMessage(){
    int spaceAvailable = m_itemChoiceList->getMessageSpaceAvailable();

    QString msg = m_inputList.getPathText(spaceAvailable);
    msg = msg.simplified();
    if(!msg.isEmpty() && !m_inputList.getUserKeys().isEmpty())
        { msg += QString(" ") + (UI_SEPARATOR_STRING) + " "; }
    msg +=  m_inputList.getUserKeys();

    if(msg.length()>spaceAvailable-10){
        msg = DOTS_CHARS + msg.right(spaceAvailable-3);
    }
//    if(msg.isEmpty())
//        { msg += UI_HIGHEST_RATED_MESSAGE; }

    m_itemChoiceList->addMessage(msg);
}


void MainUserWindow::updateDisplay() {
    //guard against recursive calling
    if(st_updatingDisplay){return;}
    st_updatingDisplay = true;

    if(!st_Visible) {
        qDebug() << "trying to hide application";
        hideApp();
    } else if(st_ShowTaskBar){
        showTaskBar();
    }
    showitemList();
    setItem();
    setFocus();
    st_updatingDisplay = false;
    gMarginWidget->gSetAppPos();
}

void MainUserWindow::showTaskBar(){
    m_inputDisplay->setEditVisibility(false);
    m_taskBarControl->setVisible(true);

}

//User Interaction Functions

void MainUserWindow::mousePressEvent(QMouseEvent *e){

    st_customFieldTakesKey = false;
    st_previewHovered = false;

    m_moveStartPoint = e->pos();
    if(e->button() == Qt::LeftButton ){
        if(!m_rubberBander){
            m_rubberBander = new QRubberBand(QRubberBand::Rectangle);
        }
        QRect r(m_inputDisplay->visibleRect());
        m_rubberBander->setGeometry(r);
        QPoint p = this->mapToGlobal(r.topLeft());
        m_rubberBander->move(p);
    }
    activateWindow();
    setFocus();
    //raise();

}

void MainUserWindow::previewWindowGainedFocus(){
    st_customFieldTakesKey = true;
}

void MainUserWindow::previewWindowHovered(bool h){
    st_previewHovered = h;
}

void MainUserWindow::mouseMoveEvent(QMouseEvent *e){

    QPoint p = e->globalPos();
    p -= m_moveStartPoint;
    st_previewHovered = false;

    if(!m_rubberBander->isVisible())
        { m_rubberBander->show(); }

    m_rubberBander->move(p);
    setPosition(true);

    st_showItemList = false;
    hideItemList();
    setFocus();
}


void MainUserWindow::mouseReleaseEvent(QMouseEvent *){
      m_rubberBander->hide();
      st_previewHovered = false;

      //setPosition(true);
      setFocus();
}


void MainUserWindow::menuEvent(QContextMenuEvent* evt) {
        contextMenuEvent(evt);
}

void MainUserWindow::listMenuEvent(QString itemPath, QPoint p) {
    CatItem item = m_inputList.getItemByPath(itemPath);
    if(!m_contextMenu && m_inputList.slotPosition()==0 && !item.isEmpty()){
        InputList il;
        il.setItem(item);
        il.addSlot();
        QList<CatItem> outItems;
        CatBuilder::getItemsFromQuery(il, outItems, MAX_ITEMS);
        m_contextMenu = new FancyContextMenu(item,outItems);
        m_contextMenu->move(p);
        m_contextMenu->show();//Stay here modally till the user chooses something...
        m_contextMenu->activateWindow();
    }
}

void MainUserWindow::choiceListKeyEvent(QKeyEvent* key) {
    qDebug() << "Got choiceListKeyEvent: " << key->key();
    inputKeyPressEvent(key);
}

void MainUserWindow::inputMethodEvent(QInputMethodEvent *e) {
    QWidget::inputMethodEvent(e);
}

void MainUserWindow::hideItemList(){
    if(!isVisible()){
        m_itemChoiceList->setVisible(false);
    } else  if (!st_showItemList ){
        qDebug() << "Item choice hiden";
        m_itemChoiceList->hideAnimated();
        m_inputDisplay->setFlat(false);
    }
    setFocus();
}

void MainUserWindow::urlChanged(const QUrl &url){
    if(url.toString() == m_customFieldURL){
        return;
    }
    if(st_EdittingCustomField){
        showCustomFieldWindow(CustomFieldInfo::URL_CHANGED);
    }
    //Eventually we'd like to recycle other urls back into CatItems
}

bool MainUserWindow::showCustomFieldWindow(CustomFieldInfo::FieldTransition fi) {
    static bool inFunction = false;
    if(inFunction){ return false; }
    inFunction = true;

    //Don't
    static bool fieldDisplayed = false;
    if(fi == CustomFieldInfo::JUST_APPEARING && st_EdittingCustomField){
        inFunction = false;
        return false;
    }

    if(fi != CustomFieldInfo::UNDETERMINED
            ||  st_EdittingCustomField
            || m_inputList.atCustomField()){
        if(fi == CustomFieldInfo::URL_CHANGED){}

        CustomFieldInfo info;
        if(fi != CustomFieldInfo::UNDETERMINED){
            info.fieldTransition = fi;
        } else if(!st_EdittingCustomField){
            info.fieldTransition = CustomFieldInfo::JUST_APPEARING;
            st_EdittingCustomField = true;
        }

        info.existingUrl = m_itemChoiceList->currentPreviewUrl();
        info.inputList = &m_inputList;
        info.fieldExpression = m_inputList.currentFieldExpression();
        info.pluginSource = m_inputList.currentFieldPlugin();
        CatBuilder::getPluginURL(&info);

        st_customFieldTakesKey = info.windowTakesKeys;
        if(info.showUrl ){
            if(info.newUrl.isEmpty()){
                qDebug() << "warning: got no url at: " << info.fieldExpression << ":" << info.pluginSource;
                inFunction = false;
                return false;
            } else {
                st_ShowCustomFieldWindow = true;
                st_EdittingCustomField = true;
                m_customFieldURL = info.newUrl;
                ListItem li(info.newUrl);
                li.setCustomPluginInfo(PREVIEW_URL_KEY,m_customFieldURL);
                li.setPreviewCSSPath("");
                li.setJavascriptFilterFilePath("");
                m_itemChoiceList->addPreviewWindow(li, this);
                fieldDisplayed = true;
                if(!info.windowSize.isEmpty()){
                    m_itemChoiceList->animateToLocation(info.windowSize);
                }

            }
        }
        if(!info.errorMessage.isEmpty()){
            m_inputDisplay->setExplanationText(info.errorMessage);
        }
        if(info.mustTab){
            st_ShowCustomFieldWindow = false;
            st_customFieldTakesKey = false;
            QKeyEvent ke(QEvent::KeyPress,Qt::Key_Backtab, Qt::NoModifier);
            tabAction(Qt::Key_Tab,&ke);
        }
    }
    inFunction = false;
    return false;
}

void MainUserWindow::showitemList() {

    qDebug() << "showitemChoiceList: st_ListFilled: " << st_ListFilled;

    if(!st_ListFilled){
        if(m_inputList.isExpanded()){
            m_itemChoiceList->addPreviewWindow(m_inputList.previewItem(), this);
        } else {
            m_itemChoiceList->endPreview();
            fillList();
        }
    }
    st_ListFilled = true;

    if (!isVisible() ) {return;}
    if(!st_showItemList ){
        hideItemList();
        return;
    }
    if(st_showItemList){
        m_itemChoiceList->showAnimated();
        m_inputDisplay->setFlat(true);
    }
}

//
//We want to process all keystroke - releases for long-edit
bool MainUserWindow::event ( QEvent * event ){
    if(event->type() == QEvent::KeyPress){
        bool handled = keyPressed((QKeyEvent* )event);
        if(handled){
            event->accept();
            return true;
        }
    } else if(event->type() == QEvent::KeyRelease){
        QKeyEvent* kEvent = (QKeyEvent*) event;
        Qt::KeyboardModifiers m = kEvent->modifiers();

        if(m_inputDisplay && m_inputDisplay->takingEditKeys()){
            bool handled = keyReleased((QKeyEvent* )event);
            if(handled){
                event->accept();
                return true;
            }
        }
//        else {
//            clearModifierKeys();
//        }
    }
    return QWidget::event( event );
}

//We want to skip control chars here to handle down AND up chars in ::event() instead
void MainUserWindow::keyPressEvent(QKeyEvent* key) {
    qDebug() << "..keyPressEvent";
    if(key->modifiers() ==  Qt::AltModifier ){
        st_altKeyDown = true;
        m_itemChoiceList->doUpdate();
        update();
    }
    if(key->modifiers() ==  Qt::ShiftModifier ){
        st_shiftKeyDown = true;
    }
    if(key->text().length()==0){ return; }

    bool handled = keyPressed(key);
    if(!handled){
        QWidget::keyPressEvent(key);
    }
}

//Pass these to long edit...
bool  MainUserWindow::keyReleased(QKeyEvent* kRelease)  {

    if(kRelease->modifiers() == Qt::AltModifier){
        st_altKeyDown = false;
        m_itemChoiceList->doUpdate();
    }

    if(st_customFieldTakesKey && st_ShowCustomFieldWindow){
        m_itemChoiceList->parentKeyRelease(kRelease);
        return true;
    }

    if( m_inputDisplay->takingEditKeys()  ) {
        m_inputDisplay->setKeyRelease(kRelease );
        return true;
    }


    return false;
}

void MainUserWindow::clearModifierKeys(){
    if(st_altKeyDown ){ //m == Qt::AltModifier - can't get up modifiers... grr
        st_altKeyDown = false;
        st_shiftKeyDown = false;
        m_itemChoiceList->doUpdate();
        update();
    }
}




bool MainUserWindow::shouldProcessControlKey(QKeyEvent* key){
    Qt::Key keyType= (Qt::Key) key->key();
    QChar k(key->key());
    Qt::KeyboardModifiers modifiers = key->modifiers();
    if(keyType == Qt::Key_Escape){ return true;}
    if(m_inputDisplay->takingEditKeys()){
        if(keyType == Qt::Key_Tab
            ||  keyType == Qt::Key_Return
            || keyType == Qt::Key_Enter
            || keyType == Qt::Key_Backtab ){
            if(modifiers == Qt::ControlModifier || (modifiers & Qt::ControlModifier)){
                return true;
            }
        }
        return false;
    }

    if(!((modifiers == Qt::NoModifier)||(modifiers == Qt::ShiftModifier))){ return true;}

    if( !k.isLetterOrNumber() && !k.isSpace() && !isStandardSymbol(k)){ return true;}

    return false;

//    return keyType == Qt::Key_Escape || keyType == Qt::Key_Tab
//            || keyType == Qt::Key_Backtab || keyType == Qt::Key_Insert
//            || keyType == Qt::Key_Delete || keyType == Qt::Key_Clear
//            || keyType == Qt::Key_Home ;

}

void MainUserWindow::textChanged(QString newText, QString /* curWord */, int cursorLocation){
    st_TakeItemFromList = false;
    st_UserChoiceChanged = true;
    st_KeyFromEdit = true;
    m_inputList.modifyUserText(newText, cursorLocation);
    setFocus();
}

void MainUserWindow::inputKeyPressEvent(QKeyEvent* key) {
    qDebug() << "inputKeyPressEvent: " << key->key();
    qDebug() << "inputKeyPressEvent text: " << key->text();
    keyPressEvent(key);
}


void MainUserWindow::executeItemByHotkey(QKeyEvent* key){
    QChar k(key->key());
    CatItem it = m_inputList.hotkeyActivated(k);
    if(!it.isEmpty()){
        if(it.hasLabel(ORGANIZING_TYPE_KEY)){
            ListItem li(it);
            CatItem fi = m_inputList.getFilterItem();
            if(!(fi == it)){
                miniIconClicked(li,true);
            } else {
                miniIconClicked(li,false);
            }
        } else {
            m_inputList.setItem(it);
            tryExecuteCurrentItem();
        }

    }
}

void MainUserWindow::activateItemByHotkey(QKeyEvent* key){
    QChar k(key->key());
    CatItem it = m_inputList.hotkeyActivated(k);
    if(!it.isEmpty()){
        if(it.hasLabel(ORGANIZING_TYPE_KEY)){
            ListItem li(it);
            CatItem fi = m_inputList.getFilterItem();
            if(!(fi == li)){
                miniIconClicked(li,true);
            } else {
                miniIconClicked(li,false);
            }
        } else {
            m_itemChoiceList->setCurrentItem(it);
            st_TakeItemFromList =true;
            st_UserChoiceChanged=true;
            setItem();
        }
    }
}

bool MainUserWindow::tryExecuteCurrentItem(){
    st_Visible = false;
    QString errString;
    QList<CatItem> outputList;
    m_inputList.rescanSlot();
    if(!(m_inputList.canRun(&errString))){
        m_inputDisplay->setExplanationText(CANNOT_RUN_ERR);
        st_Visible = true;
        return false;
    }

    int ops = CatBuilder::launchObject(m_inputList, outputList, &errString);
    if(!errString.isEmpty()) {
        m_inputDisplay->setExplanationText(errString);
        st_Visible = true;
        return true; //it did something but the action failed
    }



    m_inputDisplay->clearExpressionIcons();
    m_inputDisplay->clearText();
    m_inputDisplay->setActiveIconPos(0);
    m_itemChoiceList->clear();
    m_itemChoiceList->hide();
    m_itemChoiceList->endMultiInput();
    st_ListFilled = false;
    if (ops > 1) {
        switch (ops) {
            case MSG_CONTROL_OPTIONS:
                menuOptions();
                st_Visible = true;
                break;

            case MSG_TAKE_OUTPUT:
                st_TakeItemFromList=true;
                st_Visible = true;
                return true;

            case MSG_SHOW_ERR:
                st_Visible = true;
                break;

            case MSG_CONTROL_REBUILD:
                searchOnInput();
                refreshExtendCatalog(true);
                break;

            case MSG_NO_ACTION:
            default:
                st_Visible = false;
                m_inputList.clearAll();
                hideApp();
                break;
        }
    }

    return true;
}

//Right Arrow into
bool MainUserWindow::expandInto(int , QKeyEvent* controlKey){
    st_showItemList = true;

    CatItem& baseItem = m_inputList.currentItemRef();
    if(baseItem.isEmpty()){
        QListWidgetItem *it = m_itemChoiceList->currentItem();
        if(!it){ return false;}
        QString itemPath = it->data(ROLE_ITEM_PATH).toString();
        baseItem = m_inputList.getItemByPath(itemPath);
        if(baseItem.isEmpty()){ return false;}
    }
    //Clear list
    m_itemChoiceList->addMiniIconList(QList<ListItem>());
    if(!baseItem.getIsTempItem())
        { CatBuilder::updateItem(baseItem,2,UserEvent::SELECTED, true); }

    if(m_itemChoiceList->testIf_I_CanPreviewItem(baseItem) && !m_inputList.isExpanded()){
        hidePreview();
        m_inputList.setExpanded();
        m_inputDisplay->clearText();
        m_centerPreviewingItem = ListItem(baseItem);
        Q_ASSERT(m_inputList.isExpanded());
        st_UserChoiceChanged = true;
        st_ListFilled = false;
    } else if(baseItem.canNavigateTo()) {
        st_UserChoiceChanged = true;
        st_TakeItemFromList = true;
        st_ListFilled = false;
        st_SearchResultsChanged = true;
        m_inputList.pushChildren(baseItem);
        if(baseItem.canFilterWith()){
            if(m_inputList.slotCount()>1){return false;}
            st_TakeItemFromList = false;
            searchOnInput();
        } else {
            CatBuilder::getMiniIcons(m_inputList);
        }

        if(controlKey && controlKey->modifiers() == Qt::ControlModifier){
            m_itemOrigin = AS_SIDEVIEW;
        } else { m_itemOrigin = FROM_PARENT; }

    }
    return true;
}

bool MainUserWindow::goBackFrom(int , QKeyEvent* ){
    st_ListFilled = false;
    st_showItemList = true;
    st_UserChoiceChanged = true;
    st_SearchResultsChanged = true;
    m_itemChoiceList->endPreview();
    if(m_inputList.isExpanded()){
//        CatItem contextItem = m_inputList.getParentContextItem();
//        m_itemChoiceList->setContextItem(contextItem);
        m_itemOrigin = FROM_CHILD;
        m_inputList.setExpanded(false);
    } else if(m_inputList.isExploringChildren()){
        st_TakeItemFromList = true;
        m_itemOrigin = FROM_CHILD;
        m_desiredParentPath = m_inputList.getParentItem().getPath();
        m_savedPosition = m_inputList.popParent();
    }
    CatBuilder::getMiniIcons(m_inputList);
    addMiniIcons();
    return true;

}

bool MainUserWindow::arrowUpDown(int , QKeyEvent* controlKey){
    if(m_inputList.isExpanded()){
        m_itemChoiceList->parentKeyPress(controlKey);
    } else {
        // The menu control handles the differences and state changes
        if(st_showItemList) {
            m_itemChoiceList->parentKeyPress(controlKey);
        } else if(m_inputList.currentItemRef().isEmpty()
            && !m_inputList.isExploringChildren()){
            m_itemChoiceList->setCurrentRow(0);
        }
        st_showItemList = true;
        st_TakeItemFromList = true;
        st_SearchResultsChanged  = false;
        st_UserChoiceChanged = true;
        if(m_inputList.getListItemCount()== 0 ||
           !m_itemChoiceList->isVisible()){
            st_TakeItemFromList = false;
            searchOnInput();
        }
    }
    return true;
}




void MainUserWindow::popGuiState(){

    if( m_inputList.isExpanded()){
        m_inputList.setExpanded(false);
        st_ListFilled = false;
        st_showItemList = true;
        st_SearchResultsChanged = true;
        st_customFieldTakesKey = false;
    } else if(st_ShowCustomFieldWindow == true ){
        st_ShowCustomFieldWindow = false;
        st_ListFilled = false;
        st_showItemList = false;
        st_SearchResultsChanged = true;
        st_customFieldTakesKey = false;
        m_inputList.popSlot();
        searchOnInput();
    } if(m_inputList.isExploringChildren()){
        st_showItemList = true;
        st_UserChoiceChanged = true;
        st_TakeItemFromList = true;
        st_SearchResultsChanged = true;
        m_itemOrigin = FROM_CHILD;
        m_savedPosition = m_inputList.popParent();
        m_itemChoiceList->collapseSideExplore();
        st_ListFilled = false;
    } else if(m_inputList.slotPosition()+1 < m_inputList.slotCount() ){
        Q_ASSERT(m_inputList.slotCount()>1);
        m_inputList.popSlot();
        searchOnInput();
        st_ListFilled = false;
    } else  if (st_showItemList) {
        m_itemChoiceList->collapseSideExplore();
        st_showItemList = false;
        st_UserChoiceChanged = false;
        hideItemList();
    }
//    else if(m_inputList.tryClearOrganizingArg()){
//        st_ListFilled = false;
//        st_UserChoiceChanged = true;
//        m_inputList.clearKeys();
//        searchOnInput();
//    }
    else if (!m_inputList.isSelectionEmpty()
            || (m_inputList.getUserKeys().count()>0)
            || m_inputDisplay->hasText()){
        m_inputList.clearItem();
        m_inputList.clearKeys();
        m_inputDisplay->clearText();
        st_UserChoiceChanged = true;
        st_TakeItemFromList = false;
    } else if (m_inputList.slotCount() > 1){
        st_UserChoiceChanged = true;
        m_inputList.popSlot();
        m_inputDisplay->clearExpressionIcons();
        searchOnInput();
//            } else if(!st_ShowTaskBar){
//                this->m_inputList.setAppState(TASKBAR_APP_STATE);
//                st_ShowTaskBar = true;
    } else {
        st_Visible = false;
        m_inputList.clearAll();
        hideApp();
    }

}


void MainUserWindow::tabAction(int k, QKeyEvent* controlKey){
    Q_ASSERT(m_inputList.checkIntegrity());

    CatItem& baseItem = m_inputList.currentItemRef();
    if(!baseItem.isEmpty())
        {CatBuilder::updateItem(baseItem, 2, UserEvent::SELECTED,false);}

    st_SearchResultsChanged = true;
    st_UserChoiceChanged = true;
    st_ListFilled = false;
    st_TakeItemFromList =false;
    st_ShowCustomFieldWindow = false;
    st_customFieldTakesKey = false;
    st_tabChanged = true;
    st_EdittingCustomField = false;

    m_itemChoiceList->collapseSideExplore();
    m_itemChoiceList->raise();

    bool backTab = k== Qt::Key_Backtab
                   || controlKey->modifiers() == Qt::ShiftModifier
                   || (controlKey->modifiers() & Qt::ShiftModifier);
    if(backTab) {
        if(m_inputList.slotPosition()==0){
            st_UserChoiceChanged = true;
            return;
        }
        Q_ASSERT(m_inputList.slotPosition() == m_inputList.slotCount()-1);
        st_UserChoiceChanged = true;
        m_inputList.popSlot();
        m_inputList.rescanSlot();;
    } else {
        if(baseItem.isEmpty()){
            st_UserChoiceChanged = true;
            st_TakeItemFromList = true;
            setItem();
        }
        if(m_inputList.atEnd()){
            m_inputDisplay->clearText();
            QString err="generic error";
            if(m_inputList.customVerbChosen() &&
                    m_inputList.atCustomField() &&
                    !showCustomFieldWindow((CustomFieldInfo::FIELD_ACCEPTED))){
                st_UserChoiceChanged = false;
                m_inputDisplay->setExplanationText(err);
                return;
            } else if(m_inputList.canAddSlot(&err)){
                if(m_inputList.currentItem().isEmpty() && !m_inputList.usingCustomVerb()){
                    QIcon ic = makeStandardIcon(UI_TEXT_PLACEHOLDER_ICON);
                    m_inputDisplay->setExpressionIcon(ic,m_inputList.slotPosition());
                }
                m_inputList.addSlot();
                m_itemChoiceList->clear();
            } else {
                st_UserChoiceChanged = false;
                m_inputDisplay->setExplanationText(err);
                return;
            }
        } else { m_inputList.moveRight(); }
    }

    if(m_inputList.usingCustomVerb() || m_inputList.isOperationChosen()) {
        st_showItemList = true;
        st_SearchResultsChanged  = false;
        if(m_inputList.atCustomField()){
            st_UserChoiceChanged = true;
        } else if(m_itemChoiceList->setMultiInput(&m_inputList)){
            if(m_inputList.isOperationChosen()){
                searchOnInput();
            }
            st_UserChoiceChanged = true;
        }
    } else {
        m_itemChoiceList->endMultiInput();
    }
    setPosition();

    int slotPos = m_inputList.slotPosition();
    if(m_inputDisplay->activeExpressionIcon() < slotPos) {
        m_inputDisplay->animateIconRight();
    } else if(m_inputDisplay->activeExpressionIcon() > slotPos) {
        m_inputDisplay->animateIconLeft();
    }

    if(m_inputList.shouldExecuteNow()){
        if(tryExecuteCurrentItem()){
            m_itemChoiceList->endMultiInput();
        }
    } else if(!backTab) {
        searchOnInput();
    }
}

void MainUserWindow::operateOnItem(QString path, const CatItem opItem){
    CatItem item = m_inputList.getItemByPath(path);
    CatItem operationItem = opItem;

    QList<CatItem> children = operationItem.getChildren();
    CatItem childAction;
    if(children.length()>0){ childAction = children[0];}

    if( operationItem.isOperation() ){
        if(operationItem.getPath() == addPrefix(OPERATION_PREFIX,GOTO_CHILD_OPERATION)){
            //Since we're called from within the list, can't change the list directly
            //Post an event to change it later instead.
            QKeyEvent* ke = new QKeyEvent(QEvent::KeyPress,Qt::Key_Right, Qt::NoModifier);
            qApp->postEvent(this,ke, Qt::HighEventPriority);
        } else if( operationItem.getPath() == addPrefix(OPERATION_PREFIX,SET_PRIORIT_OPERATION)){
            int w = operationItem.getCustomValue(SET_PRIORITY_KEY_STR);
            item.setScaledSourceWeight(w);
            CatBuilder::updateItem(item,1,UserEvent::STANDARD_UPDATE);
        } else if(operationItem.getPath() == addPrefix(OPERATION_PREFIX,PIN_OPERATION_NAME)
            || childAction.getPath() == addPrefix(OPERATION_PREFIX,PIN_OPERATION_NAME)){
            item.setPinned(m_inputList.getUserKeys());
            item.setIsTempItem(false);
            CatBuilder::addItem(item);
            m_itemChoiceList->updateItem(ListItem(item));
        } else if(operationItem.getPath() == addPrefix(OPERATION_PREFIX,DEPRICATE_OPERATION_NAME)
            || childAction.getPath() == addPrefix(OPERATION_PREFIX,DEPRICATE_OPERATION_NAME)){
            if(item.getItemType() == CatItem::MESSAGE && item.isUnseenItem()){
                item.setSeen();
                st_TakeItemFromList = false;
                searchOnInput();
            } else {
                item.setIsDepricated();
                item.setIsTempItem(false);
            }
            CatBuilder::addItem(item);
            m_inputList.removeItem(item);
            m_itemChoiceList->removeItem(item);
        } else if(operationItem.getPath() ==
                  addPrefix(OPERATION_PREFIX,ACTIVATE_OPTION_ITEM)){
            Q_ASSERT(children.length()==1);
            //Hoping we can do this directly...
            st_TakeItemFromList = false;
            m_inputList.setItem(path);
            QKeyEvent ke(QEvent::KeyPress,Qt::Key_Backtab, Qt::NoModifier);
            tabAction(Qt::Key_Tab,&ke);
            if(m_inputList.acceptItem(&childAction)){
                m_inputList.setItem(childAction);
                if(!tryExecuteCurrentItem()){
                    QKeyEvent ke(QEvent::KeyPress,Qt::Key_Tab, Qt::NoModifier);
                    tabAction(Qt::Key_Tab,&ke);
                }
            }
        } else if(operationItem.getPath() ==
                  addPrefix(OPERATION_PREFIX,SELECTION_OPERATION_NAME)){
            if(m_inputList.getItemByPath(path).isEmpty()
                && !m_inputList.findItemInParent(path).isEmpty()){
                m_inputList.popToParentPosition(path);
                m_itemChoiceList->collapseSideExplore();
                m_itemOrigin = FROM_SEARCH;
                st_SearchResultsChanged = true;
                st_ListFilled = false;
                st_TakeItemFromList = false;
            } else {
                st_TakeItemFromList = true;
            }
            st_UserChoiceChanged = true;
        }
    }
    updateDisplay();
}


//Processes all text directly except when you have subordinate edit function...
bool MainUserWindow::keyPressed(QKeyEvent* key) {
    if(m_rubberBander){
        m_rubberBander->hide();
    }

    Qt::KeyboardModifiers modifiers = key->modifiers();
    if(!(Qt::AltModifier & modifiers)){
        clearModifierKeys();
    }

    QChar k(key->key());
    QString keyStr = key->text();
    Qt::Key keyType= (Qt::Key) key->key();
    st_KeyFromEdit = false;
    st_previewHovered = false;
    //if(m_inputDisplay->takingEditKeys()){
        qDebug() << "key events:" << k << " : "
                << QString::number(key->key(), 16) << " :" << keyStr << ":";
    //}
    if((st_altKeyDown || modifiers == Qt::ALT) && k.isLetterOrNumber()){
        if(st_shiftKeyDown){
            executeItemByHotkey(key);
        } else {
            activateItemByHotkey(key);
        }
        return true;
    }

    if((st_customFieldTakesKey && st_ShowCustomFieldWindow) &&
            keyType!=Qt::Key_Escape){
        m_itemChoiceList->parentKeyPress(key);
        return true;
    }

    if((keyStr.length() ==0)
        && m_inputDisplay->takingEditKeys()){
        m_backgroundSearchKeys.clear();
        if((modifiers == Qt::NoModifier)||(modifiers == Qt::ShiftModifier) || ((modifiers == Qt::ControlModifier))){
            if(keyType == Qt::Key_Down ||
               keyType == Qt::Key_Up ||
               keyType == Qt::Key_Right ||
               keyType == Qt::Key_Left ||
               keyType == Qt::Key_End ||
               keyType == Qt::Key_Home ||
               keyType == Qt::Key_PageUp ||
               keyType == Qt::Key_PageDown  ){
                m_inputDisplay->setKeyPress(key);
                key->accept();
                return true;
            }
        }
        return false;
    }

    //if (k==Qt::Key_Alt || k==Qt::Key_Shift || Qt::Key_Control || Qt::Key_Meta){}
    if(shouldProcessControlKey(key)) {
        bool used = processControlKey(key);
        if(used){
            key->accept();
        }
        return used;
    } else {
        m_backgroundSearchKeys.clear();
        m_inputList.setSearchLevel(0);
        m_oldSearchLevel=-1;
        if(m_inputDisplay->setKeyPress(key)){

            st_TakeItemFromList = false;
            st_UserChoiceChanged = true;

            if(!st_KeyFromEdit){
                m_inputList.appendUserkeys(keyStr);
            }
            key->accept();
            m_inputDisplay->setExplanationText("");
            m_inputList.clearUnMatchItem();
        }
        if(!m_inputList.getUserKeys().isEmpty()){
            addMiniIcons();
        }
        return true;
    }
}

//The UI logic should live here...
bool MainUserWindow::processControlKey(QKeyEvent* controlKey){

    int k = controlKey->key();

    m_itemOrigin = FROM_SEARCH;

    bool handled = true;

    switch(k){
        //Back-up on choice
        //Either remove a menu or remove one item
        //Or hide the app
        case Qt::Key_Escape:
            popGuiState();
            break;
        case Qt::Key_Backspace:
            st_UserChoiceChanged = true;
            st_TakeItemFromList = false;
            m_inputList.backspaceKey();
            if(!m_inputList.isExpanded()){
                m_inputDisplay->clearText();
                searchOnInput();
            }
            break;
        //Show the menu or move up or down one item in it...
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
            //move into sub item
            handled = arrowUpDown(k, controlKey);
            break;
        case Qt::Key_Right:
            expandInto(k, controlKey);

            break;
        //
        case Qt::Key_Left:
            goBackFrom(k, controlKey);
            break;
        case Qt::Key_Backtab:
        case Qt::Key_Tab:
            tabAction(k, controlKey);
            break;
        //Do the current choice
        case Qt::Key_Return:
        case Qt::Key_Enter: {

                CatItem& actionItem = m_inputList.currentItemRef();
                if(!actionItem.isEmpty()){
                    CatBuilder::updateItem(actionItem,2);
                }
                if(controlKey->modifiers() == Qt::ControlModifier){
                    if(m_itemChoiceList->testIf_I_CanPreviewItem(actionItem) && !m_inputList.isExpanded()){
                        m_inputList.setExpanded();
                        st_ListFilled = false;
                        st_showItemList = true;
                    }
                    break;
                } else if (!actionItem.isEmpty()) {
                    if(m_inputList.verbNeedMoreInfo()){
                        tabAction(k,controlKey);
                    }
                    st_UserChoiceChanged = true;
                }
                tryExecuteCurrentItem();
                break;
            }
            break;
        default:
            setFocus();
            return false; //We can't do anything so continue normal processing
    }
    updateDisplay();
    setFocus();
    return handled;
}

