/*
Oneline
Copyright (C) 2010  Hans Joseph Solbrig

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

#include "feed_reader.h"
#include "twitter_reader.h"
#include "facebook_reader.h"
#include "streamplugin.h"
#include "recursive_json.h"
#include "iconfromurl_reader.h"

QSettings** ggSettings;

//All of our filters will come from other
void StreamPlugin::init(QSettings* testSettings)
{
    QSettings* set;
    if (!testSettings) {
        set = *settings;
        if ( set->value("Xsltplugin/version", 0.0).toDouble() == 0.0 ) {
            set->beginWriteArray("Xsltplugin/spec");
            set->setValue("Xsltplugin/version", 2.0);
            set->endArray();
        }
        Q_ASSERT(settings);
        ggSettings = settings;
    } else {
        set = testSettings;
    }
    mp_twitterReader = 0;

}

void StreamPlugin::getCatalog(QList<CatItem>* items){
    for(int i=0;i<m_filters.size();i++){ items->append(m_filters[i]); }
}

void StreamPlugin::stopExtendingCatalog(){
    emit end_current_downloads();
}

bool StreamPlugin::extendCatalog(SearchInfo* info, QList<CatItem>* r){
    return getFeedReaders(info, r);
}

bool StreamPlugin::getFeedReaders(SearchInfo* info, QList<CatItem>* r){
    qDebug() << "StreamPlugin::extendCatalog: ";
    QList<AbstractReader*> feedDownloaders;
    for(int i=0;i < info->itemListPtr->count();i++){
        CatItem it = info->itemListPtr->at(i);
        qDebug() << "scanning: " << it.getPath();
        feedDownloaders.append(createStreamReader(it,info->m_extensionType));
    }
    r->append(launchFeedReaders(feedDownloaders));

    //A second "round" just for icons and pictures...
    QList<AbstractReader*> iconDownloaders;
    for(int i=0;i < r->count();i++){
        CatItem it = r->at(i);
        qDebug() << "scanning for icon: " << it.getPath();
        iconDownloaders.append(createIconReader(it));
    }
    r->append(launchFeedReaders(iconDownloaders));

    for(int i=0;i < r->count();i++){
        (*r)[i].setUpdated(UserEvent::JUST_FOUND);
    }

    for(int i=0;i < info->itemListPtr->count();i++){
        CatItem it = info->itemListPtr->at(i);
        it.setUpdated(UserEvent::STANDARD_UPDATE);
        it.setSourceUpdateTime(appGlobalTime());
        r->append(it);
    }

    qDebug() << "StreamPlugin, result count: " << r->count();
    return (r->count()>0);
}

QList<CatItem> StreamPlugin::runFeedReaders(QList<AbstractReader*> feedDownloaders){
    QList<CatItem> r;

    launchFeedReaders(feedDownloaders);
    for(int i=0;i < feedDownloaders.count();i++){
        if(feedDownloaders[i]){
            r.append(feedDownloaders[i]->getResults());
            delete feedDownloaders[i];
            feedDownloaders[i] = 0;
        }
    }
    for(int i=0;i < r.count();i++){
        r[i].setUpdated(UserEvent::JUST_FOUND);
    }


    qDebug() << "StreamPlugin, result count: " << r.count();
    return r;
}

QList<CatItem> StreamPlugin::launchFeedReaders(QList<AbstractReader*>& feedDownloaders){
//Asynchronoous version commented-out
//
//    for(int i=0;i < feedDownloaders.count();i++){
//        AbstractReader* s = feedDownloaders[i];
//        if(s){
//            s->start();
//        }
//    }
//    for(int i=0;i < feedDownloaders.count();i++){
//        while(true){
//            feedDownloaders[i]->wait();
//            if(feedDownloaders[i]->isFinished()){
//                break;
//            }
//        }
//        if(feedDownloaders[i]){
//            r->append(feedDownloaders[i]->getResults());
//            delete feedDownloaders[i];
//            feedDownloaders[i] = 0;
//        }
//    }

    for(int i=0;i < feedDownloaders.count();i++){
        AbstractReader* s = feedDownloaders[i];
        if(s){
            s->start();
            while(true){
                feedDownloaders[i]->wait(MAX_WAIT);
                if(feedDownloaders[i]->isFinished()){
                    break;
                }
                feedDownloaders[i]->do_abort();
            }
        }
    }

    QList<CatItem> res;
    for(int i=0;i < feedDownloaders.count();i++){
        if(feedDownloaders[i]){
            res.append(feedDownloaders[i]->getResults());
            delete feedDownloaders[i];
            feedDownloaders[i] = 0;
        }
    }
    feedDownloaders.clear();
    return res;

}

bool StreamPlugin::itemLoading(CatItem* itemPtr, UserEvent::LoadType ){

    if(itemPtr->hasLabel(EXTERNAL_GEN_SOURCE_KEY)
        &&  itemPtr->hasLabel(STREAMPLUGIN_NAME)  ){
        CatItem genItem = *itemPtr;
        Q_ASSERT(m_pluginGenerators.count()<100);
        if(!m_pluginGenerators.contains(genItem)){
            m_pluginGenerators.append(genItem);
            return true;
        }
    }

    Q_ASSERT(itemPtr);
    if(itemPtr->hasLabel(FIREFOX_PLUGIN_NAME) &&
            !itemPtr->hasLabel(UPDATEABLE_SOURCE_KEY)){
        itemPtr->setIsUpdatableSource(true);
        Q_ASSERT(itemPtr->hasLabel(UPDATEABLE_SOURCE_KEY));
        return true;
    }
    return false;
}

void StreamPlugin::getID(uint* id){ *id = HASH_streamPlugin;}

void StreamPlugin::getName(QString* str){ *str = STREAMPLUGIN_NAME;}

void StreamPlugin::getLabels(InputList* ){ }

void StreamPlugin::getResults(InputList* , QList<CatItem>* ){ }

QString StreamPlugin::getIcon() {
    return "";
}

bool StreamPlugin::getCustomFieldInfo(CustomFieldInfo* cfi){
    if(cfi->pluginSource !=STREAMPLUGIN_NAME){
        return false;
    }

    if(cfi->fieldExpression==TWITTER_VERIFY_FIELD){
        if(cfi->fieldTransition == CustomFieldInfo::JUST_APPEARING){
            CatItem it;
            if(mp_twitterReader){
                delete mp_twitterReader;
            }
            mp_twitterReader = new TwitterReader(this,it);
            QString url = mp_twitterReader->createPermissionURL();
            cfi->newUrl = url;
            cfi->showUrl = true;
            cfi->windowTakesKeys = true;
            return true;
        } else if(cfi->existingUrl == TwitterAuthorizeURL){
            if(!cfi->inputList->getUserKeys().isEmpty()){
                Q_ASSERT(mp_twitterReader);

                QByteArray pin(cfi->inputList->getUserKeys().toAscii());

                QOAuth::ParamMap pm = mp_twitterReader->createAccessToken(pin);
                if(pm.isEmpty()){
                    //User can backup or
                    cfi->windowTakesKeys = true;
                    cfi->showUrl=true;
                    cfi->errorMessage = TWITTER_PIN_ERROR;
                } else {
                    QByteArray userToken = pm.value(QOAuth::tokenParameterName());
                    QByteArray userSecret = pm.value(QOAuth::tokenSecretParameterName());

                    cfi->inputList->setArgValue(TWITTER_CONSUMER_KEY_KEY, userToken);
                    cfi->inputList->setArgValue(TWITTER_CONSUMER_SECRET_KEY, userSecret);
                    cfi->mustTab=false;
                    cfi->showUrl=false;
                }
                delete mp_twitterReader;
                mp_twitterReader = 0;
            } else {
                cfi->showUrl = true;
                cfi->windowTakesKeys = false;
            }
        } else {//A Twitter authorize error so users should see it in the Twitter window
            cfi->windowTakesKeys = true;
            cfi->showUrl=true;
        }
    }

    if(cfi->fieldExpression==FACEBOOK_VERIFY_FIELD){
        if(cfi->fieldTransition == CustomFieldInfo::JUST_APPEARING){
            CatItem it;
            FacebookReader fr(this,it);
            QString url = fr.createPermissionURL();
            cfi->newUrl = url;
            cfi->showUrl = true;
            cfi->windowTakesKeys = true;
            return true;
        } else if(cfi->fieldTransition == CustomFieldInfo::URL_CHANGED) {
            CatItem it;
            FacebookReader fr(this,it);
            if(!cfi->existingUrl.isEmpty()){
                QString oAuthCode_temp = fr.scanResultURL(cfi->existingUrl);
                if(oAuthCode_temp.isEmpty()){
                    qDebug() << "got no code, probably first pass: " << cfi->existingUrl;
                    return false;
                }
                qDebug() << "got code" << oAuthCode_temp;


                QString oAuthKey = fr.tokenGetter(oAuthCode_temp);
                qDebug() << "got key" << oAuthKey;

                oAuthKey = fr.extraTokenGetter(oAuthKey);
                qDebug() << "got key" << oAuthKey;

                Q_ASSERT(!oAuthKey.isEmpty());
                cfi->inputList->setArgValue(OAUTHKEY_KEY, oAuthKey);
                cfi->mustTab=true;
                cfi->showUrl=false;

                QString userName = fr.getUserName(oAuthKey);
                qDebug() << "got userName:" << userName;
                cfi->inputList->setArgValue(FACEBOOK_USERNAME_FIELD, userName);
                return true;
            }
        }
    }
    return false;
}


void StreamPlugin::launchItem(InputList* , CatItem* )
{
}

void StreamPlugin::doDialog(QWidget* , QWidget** )
{
}

void StreamPlugin::endDialog(bool )
{

}

QList<AbstractReader*> StreamPlugin::createStreamReader(CatItem item, UserEvent::LoadType lt){
    QList<AbstractReader*> feedDownloaders;
    if(item.hasLabel(FIREFOX_PLUGIN_NAME) ) {
        if(item.hasLabel(UPDATEABLE_SOURCE_KEY)){
            if(!item.isUpdatableSource()){
                return feedDownloaders;
            }
        }
        if(!item.hasLabel(HTML_HEADER_PROCESSED_KEY)){
            qDebug() << "creating http head reader";
            qDebug() << "request url: " << item.getRequestUrl();
            feedDownloaders.append(new HttpHeadReader(this, item));
        } else if(item.hasLabel(STREAM_SOURCE_PATH)){
            qDebug() << "creating xslt/xml reader";
            qDebug() << "request url: " << item.getRequestUrl();
            feedDownloaders.append(new XSLTFeedReader(this, item));
        }
    }

    if(item.hasLabel(TWITTER_SOURCE_STR)){
        qDebug() << "creating twitter reader";
        feedDownloaders.append((AbstractReader*)new TwitterReader(this, item));
    }

    if(item.hasLabel(FACEBOOK_PLUGIN_SOURCE_STR)){
        qDebug() << "creating facebook reader";
        feedDownloaders.append((AbstractReader*)new FacebookReader(this, item));
    }

    if(item.hasLabel(FACEBOOK_PLUGIN_SOURCE_PARENT_STR)){
        QList<CatItem> childs = item.getChildren();
        for(int i=0; i<childs.count(); i++){
            qDebug() << "creating facebook reader";
            if(childs[i].hasLabel(FACEBOOK_PLUGIN_SOURCE_STR)){
                qDebug() << "creating facebook reader";
                feedDownloaders.append((AbstractReader*)new FacebookReader(this, childs[i]));
            }
        }
    }

    if(item.hasLabel(PARENT_SOURCE_ITEM_LABEL)){
        QList<CatItem> childs = item.getChildren();
        for(int i=0; i<childs.count(); i++){
            qDebug() << "creating readers recursively";
            feedDownloaders.append(createStreamReader(item, lt));
        }
    }

    if(item.hasLabel(FILE_CATALOG_PLUGIN_STR)){
        if(item.hasLabel(STREAM_SOURCE_PATH)){
            qDebug() << "creating xml file reader";
            feedDownloaders.append((AbstractReader*)new XmlFileReader(this, item));
        }
    }

    if(lt == UserEvent::SELECTED){
        for(int i=0; i< m_pluginGenerators.count(); i++){
            CatItem filt = m_pluginGenerators[i];
            QString downloadLabel = filt.getCustomString(LABEL_FILTERING_KEY);
            if(!item.hasLabel(downloadLabel)){ break;}

            RecursiveStream* rdr=new RecursiveStream(this, filt,item);
            if(!rdr->relevantToItem()){
                delete rdr;
                continue;
            }
            feedDownloaders.append(rdr);
        }
    }
    return feedDownloaders;
}

QList<AbstractReader*> StreamPlugin::createIconReader(CatItem it){
    QList<AbstractReader*> feedDownloaders;
    if(it.hasLabel(ICON_URL_KEY) && it.getOverrideIcon().isEmpty()){
        feedDownloaders.append((AbstractReader*)new IconFromUrlReader(this, it));
    }
    return feedDownloaders;
}

int StreamPlugin::msg(int msgId, void* wParam, void* lParam)
{
    bool handled = false;
    switch (msgId){
        case MSG_INIT:
                init();
                handled = true;
                break;
        case MSG_GET_LABELS:
                getLabels((InputList*) wParam);
                handled = true;
                break;
        case MSG_GET_ID:
                getID((uint*) wParam);
                handled = true;
                break;
        case MSG_GET_NAME:
                getName((QString*) wParam);
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
        case MSG_EXTEND_CATALOG:
                handled = extendCatalog((SearchInfo*) wParam, (QList<CatItem>*) lParam);
                break;
        case MSG_ITEM_LOADING:
                handled = itemLoading((CatItem* )wParam, (UserEvent::LoadType)(int)lParam);
                break;
        case MSG_STOP_EXTENDING_CATALOG:
                stopExtendingCatalog();
                handled = true;
                break;
        case MSG_CUSTOM_FIELD_EDITING:
                handled = getCustomFieldInfo((CustomFieldInfo*)wParam);
                break;
        case MSG_LAUNCH_ITEM:
                launchItem((InputList*) wParam, (CatItem*) lParam);
                handled = true;
                break;
        case MSG_HAS_DIALOG:
                // Set to true if you provide a gui
                handled = false;
                break;
        case MSG_DO_DIALOG:
                // This isn't called unless you return true to MSG_HAS_DIALOG
                doDialog((QWidget*) wParam, (QWidget**) lParam);
                break;
        case MSG_END_DIALOG:
                // This isn't called unless you return true to MSG_HAS_DIALOG
                endDialog((bool) wParam);
                break;

        default:
                break;
    }
    return handled;
}

Q_EXPORT_PLUGIN2(myplugin, StreamPlugin)
