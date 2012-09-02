/*
oneline: Multibrowser and App Launcher
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

#include <QtGui>
#include <QUrl>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>
#include "tags.h"
#include "item.h"
#include "plugin_interface.h"

void TagsPlugin::init()
{

}

void TagsPlugin::afterLoad(QList<CatItem>* )
{
}

void TagsPlugin::itemLoaded(CatItem* ){
}

void TagsPlugin::getID(uint* id)
{
	*id = HASH_TAG_PLUGIN;
}


void TagsPlugin::getName(QString* str)
{
	*str = "Tag Plugin";
}

void TagsPlugin::getLabels(InputList* )
{
    return;
}

//If we're filled the catalog correctly, results are already there
void TagsPlugin::getResults(InputList* , QList<CatItem>* )
{
    return;
}

QString TagsPlugin::getIcon()
{
	return libPath + "/icons/tags.png";
}

// Most items should exist but
// Refreshing should make sure the commands exist
// And make clear the
void TagsPlugin::getCatalog(QList<CatItem>* items)
{
    CatItem addTagItem(addPrefix(OPERATION_PREFIX,TAG_OPERATION_NAME),CREATE_TAG_NAME);
    addTagItem.setPluginId(HASH_TAG_PLUGIN);
    addTagItem.setItemType(CatItem::OPERATION);
    addTagItem.setOrganizingType(CatItem::TAG);
    addTagItem.setTakesAnykeys(true);
    addTagItem.setMatchType(CatItem::EXTERNAL_INFO);
    addTagItem.setIcon(TAG_ICON_NAME);
    addTagItem.setUseLongName(true);
    addTagItem.setExternalWeight(VERY_HIGHT_EXTERNAL_WEIGHT,getPluginRep());
    items->append(addTagItem);
//    CatItem removeRemoveTagItem(addPrefix(OPERATION_PREFIX,TAG_OPERATION_NAME),"remove tag");
//    removeRemoveTagItem.setPluginId(HASH_TAG_PLUGIN);
//    removeRemoveTagItem.setItemType(CatItem::OPERATION);
//    removeRemoveTagItem.setTakesAnykeys(true);
//    removeRemoveTagItem.setMatchType(CatItem::EXTERNAL_INFO);
//    removeRemoveTagItem.setIcon(TAG_ICON_NAME);
//    removeRemoveTagItem.setExternalWeight(VERY_HIGHT_EXTERNAL_WEIGHT,getPluginRep());
//    items->append(removeRemoveTagItem);


    CatItem pinOp(addPrefix(OPERATION_PREFIX,PIN_OPERATION_NAME),PIN_ITEM_NAME);
    pinOp.setPluginId(HASH_TAG_PLUGIN);
    pinOp.setItemType(CatItem::OPERATION);
    pinOp.setOrganizingType(CatItem::TAG);
    pinOp.setTakesAnykeys(true);
    pinOp.setMatchType(CatItem::EXTERNAL_INFO);
    pinOp.setIcon(PIN_ICON_NAME);
    pinOp.setUseLongName(true);
    pinOp.setExternalWeight(VERY_HIGHT_EXTERNAL_WEIGHT,getPluginRep());
    items->append(pinOp);

    CatItem depricateOp(addPrefix(OPERATION_PREFIX,DEPRICATE_OPERATION_NAME),DEPRICATE_ITEM_NAME);
    depricateOp.setPluginId(HASH_TAG_PLUGIN);
    depricateOp.setItemType(CatItem::OPERATION);
    depricateOp.setTakesAnykeys(true);
    depricateOp.setMatchType(CatItem::EXTERNAL_INFO);
    depricateOp.setIcon(DEPRICATE_ICON_NAME);
    depricateOp.setUseLongName(true);
    depricateOp.setExternalWeight(VERY_HIGHT_EXTERNAL_WEIGHT,getPluginRep());
    items->append(depricateOp);


    return;
}

//This be done in the main code instead...
//int TagsPlugin::launchItem(InputList* id, QList<CatItem>* results)
//{
//
//    //The empty tag is OK here
//    CatItem tagAction = id->getOperationPart();
//    CatItem theTag;
//    bool remove=false;
//    if(tagAction.getPath() == addPrefix(OPERATION_PREFIX,TAG_OPERATION_NAME)){
//        QString tagName = tagAction.getLongText();
//        CatItem t(addPrefix(TAGITEM_PREFIX,tagName), tagName);
//        t.setTagLevel(CatItem::KEY_WORD);
//        t.setItemType(CatItem::OPERATION);
//        theTag = t;
//    } if(tagAction.getPath() == addPrefix(OPERATION_PREFIX,TAG_OP_REMOVE_NAME)){
//        remove = true;
//    }else {
//        theTag = tagAction;
//    }
//
//    CatItem tagged = id->asStateItem(false);
//    tagAction.addChild(tagged, TAG_LABEL,BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD);
//    if(id->slotCount()==1 && !id->currentItem().isEmpty()){
//        CatItem actualItem = id->currentItem();
//        tagAction.addChild(actualItem, TAG_LABEL);
//        //To remove, add child then mark to remove
//        if(remove){
//            tagAction.removeChild(actualItem);
//        }
//    }
//    results->append(theTag);
//    return MSG_CONTROL_REBUILD;
//
//
//}

void TagsPlugin::doDialog(QWidget* , QWidget** ) {

}

void TagsPlugin::endDialog(bool ) {
}

void TagsPlugin::setPath(QString * path) {
    libPath = *path;
}

int TagsPlugin::msg(int msgId, void* wParam, void* lParam)
{
    int handled = 0;
	switch (msgId)
	{		
	case MSG_GET_ID:
		getID((uint*) wParam);
		handled = true;
		break;
        case MSG_AFTER_LOAD:
            afterLoad((QList<CatItem>*) wParam);
            handled = true;
            break;

	case MSG_GET_LABELS:
                getLabels((InputList*) wParam);
		handled = true;
		break;
	case MSG_GET_RESULTS:
                getResults((InputList*) wParam, (QList<CatItem>*) lParam);
		handled = true;
		break;
	case MSG_GET_CATALOG:
		getCatalog((QList<CatItem>*) wParam);
		handled = true;
		break;
//	case MSG_LAUNCH_ITEM:
//                handled = launchItem((InputList*) wParam, (QList<CatItem>*) lParam);
//		break;
	case MSG_INIT:
		init();
		handled = true;
		break;
	case MSG_GET_NAME:
		getName((QString*) wParam);
		handled = true;
		break;
	case MSG_HAS_DIALOG:
		handled = true;
		break;
	case MSG_DO_DIALOG:
		doDialog((QWidget*) wParam, (QWidget**) lParam);
		break;
	case MSG_END_DIALOG:
		endDialog((bool) wParam);
		break;
	case MSG_PATH:
		setPath((QString *) wParam);
	default:
		break;
	}

	return handled;
}

Q_EXPORT_PLUGIN2(loader, TagsPlugin) 
