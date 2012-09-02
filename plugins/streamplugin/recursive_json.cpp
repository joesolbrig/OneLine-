#include "recursive_json.h"

RecursiveStream::RecursiveStream(QObject* parent, CatItem filt, CatItem arg): AbstractReader(0,filt){
    m_parent = parent;
    m_arg = arg;
}

bool RecursiveStream::relevantToItem(){
    if(m_filt.hasLabel(TYPE_FILTERING_KEY) &&
       (m_arg.getItemType() != (CatItem::ItemType) m_filt.getCustomValue(TYPE_FILTERING_KEY)))
            {return false;}

    //We demand a label this thing filters with
    if(m_filt.hasLabel(LABEL_FILTERING_KEY) ) {
        QString filterLabel = m_filt.getCustomString(LABEL_FILTERING_KEY);
        if( !m_arg.hasLabel(filterLabel)){
            return false;
        }
    } else { return false;}
    if(!m_arg.shouldUpdate(2, UserEvent::SELECTED, 2)){ return false;}
    return true;


}


void RecursiveStream::run(){
    m_extendedURLS.clear();
    QList<CatItem> resList;
    resList = extendItem(m_filt,m_arg);
    extendList(resList,0);
    m_results.append(resList);
    m_finished = true;
}

bool RecursiveStream::extendList(QList<CatItem>& items, int recur){

    QList<CatItem> resList;
    bool haveExtended=true;
    QList<CatItem> newChilds;

    while(haveExtended){
        haveExtended=false;
        for(int i=0; i<items.count(); i++){
            if(m_extendedURLS.contains(items[i].getRequestUrl())){
                resList.append(items[i]);
            } else {
                newChilds = extendChildren(items[i], recur, &haveExtended);
                resList.append(newChilds);
            }
        }
    }
    items = resList;

    return haveExtended;
}

QList<CatItem> RecursiveStream::extendChildren(CatItem& it,int recur, bool* hasExtendedPtr){
    QList<CatItem> res;
    res.append(it);

    if(recur > RECUR_DOWNLOAD_LIMIT){return it.getChildren();}

    if(it.hasLabel(EXTEND_IMMEDIATELY_EXTERNALLY_KEY)){
        return extendItem(it, CatItem(), hasExtendedPtr);
    }
    QList<CatItem> childs = it.getChildren();
    bool ex = extendList(childs, recur);
    if(ex){
        it.clearRelations();
        it.addChildren(childs);
        if(hasExtendedPtr){
            *hasExtendedPtr=true;
        }
    }

    return res;
}

QList<CatItem> RecursiveStream::extendItem(CatItem& item, CatItem arg,  bool* hasExtendedPtr){
    //qDebug() << "RecursiveStream::extendItem: " << item.getPath() << "arg: " << arg.getPath();

    CatItem downloadItem = item;
    QHash<QString, EntryField> vars;
    CatItem resItem;
    VerbItem vi(item);
    if(!arg.isEmpty() && !arg.getPath().isEmpty()){
        vi.setValue(EXTERN_SOURCE_ARG_PATH, arg.getPath());
        vi.setValue(EXTERN_SOURCE_ARG_NAME, arg.getName());

        vars[EXTERN_SOURCE_ARG_PATH].value = arg.getPath();
        vars[EXTERN_SOURCE_ARG_NAME].value = arg.getName();
    }

    QString pathToFillin = item.getCustomString(EXTERN_SOURCE_API_PATH);
    qDebug() << "download path To Fill-in" << pathToFillin;
    QString urlStr = vi.getFilledinValue(pathToFillin);
    if(!urlStr.isEmpty()){
        downloadItem.setRequestUrl(urlStr);
    }
    urlStr = downloadItem.getRequestUrl();
    //qDebug() << "download Url" << urlStr;

    QList<CatItem> res;
    res.append(item);

    if(!m_extendedURLS.contains(urlStr)){
        AbstractFeedReader* ar=0;
        if(downloadItem.hasLabel(JSON_SOURCE_LABEL)){
            m_extendedURLS.insert(urlStr);
            qDebug() << "creating json downloader: " ;
            ar = new JsonReader(m_parent,downloadItem);
        } else if(downloadItem.hasLabel(XSLT_SOURCE_STR)){
            m_extendedURLS.insert(urlStr);
            qDebug() << "creating xslt downloader: " ;
            qDebug() << downloadItem.getRequestUrl();
            ar = new XSLTFeedReader(m_parent, downloadItem);
        } else if(downloadItem.hasLabel(GEN_KEY_SETTINGS ) ){
            qDebug() << "expanding children " ;
            resItem = FillinItem::createChildren(item,vars);
            QList<CatItem> newChilds = extendChildren(resItem, 0);
            if(newChilds.count()>0){
                return newChilds;
            } else {
                item = resItem;
                return QList<CatItem>();
            }
        }
        ar->setValue(EXTERN_SOURCE_ARG_PATH,arg.getPath());
        ar->setValue(EXTERN_SOURCE_ARG_NAME,arg.getName());
        ar->run();

        resItem= ar->getBaseResult();
        if(resItem.hasLabel(EXTEND_IMMEDIATELY_EXTERNALLY_KEY)){
            QList<CatItem> childs = extendItem(resItem);
            for(int i=0; i< childs.count();i++){
                item.addChild(childs[i]);
            }
        }

        if(!resItem.isEmpty()){
            if(hasExtendedPtr){
                *hasExtendedPtr=true;
            }
            if(ar->shouldReplaceParent()){
                QList<CatItem> childs = resItem.getChildren();
                for(int i=0; i< childs.count();i++){
                    item.addChild(childs[i]);
                }
                res.append(childs);
            } else{
                res.append(resItem);
            }
        }

    }

    return res;

}



