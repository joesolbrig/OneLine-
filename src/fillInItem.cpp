
#include <assert.h>

#include "plugin_interface.h"
#include "item.h"
#include "input_list.h"
#include "fillInItem.h"


FillinItem::FillinItem(CatItem it, int parentIndex): m_generatingItem(it) {

    QSettings s(m_generatingItem.getGeneratingSettings(),
                QSettings::IniFormat);
    SettingsTree st(&s,m_generatingItem.getFillinGenerator());
    DataTree* dt = &st;

    m_depthLimit = 1000;
    m_childLimit= 100;
    m_isBuilding = true;
    m_isParent = false;
    m_justTakeChildren = false;
    m_mergeWithParents = false;
    m_isExtraChild =false;
    m_dataRead =0;
    m_parentIndex=parentIndex;

    addData(dt,0,0);
    setGenerating();
}

FillinItem::FillinItem(DataTree* dt, int dataLimit, int parentIndex) {
    m_dataLimit = dataLimit;
    m_dataRead = 0;
    m_parentIndex=parentIndex;
    if(dataLimit==-1){
        m_dataLimit=1000;
    }
    m_childLimit= 100;

    m_isBuilding = true;
    m_depthLimit = 1000;
    m_isParent = false;
    m_justTakeChildren = false;
    m_mergeWithParents = false;
    m_isExtraChild =false;
    if(dt){
        addData(dt, 0, 0);
    }
}


//This both creates a base item that might be a generator
//and generates a further item...
int FillinItem::addData(DataTree* data, int recur,  int childCounter){
    int res;
    res = addHashData(data,recur, childCounter);
    return res;
}

//This both creates a base item that might be a generator
//and generates a further item...
int FillinItem::addHashData(DataTree* data, int recur, int childCounter){
    Q_ASSERT(data);
    Q_ASSERT(recur < m_depthLimit);//foo remove

    if(recur >= m_depthLimit){return 0;}

    bool go = !(data->isEmpty());
    for(int c=0; go ;c++, go = data->gotoNextHashData()) {
        QString key = data->keyAt();
        if(c >= m_childLimit){
            qDebug() << "\nchild Count exceed limit for: " << key;
            break;
        }

        qDebug() << (QString(2*(recur+1), ' ') + "base key: " +  key) ;
        if(data->isAtString()){
            QString v = data->stringAt();
            if(!v.isEmpty()){ addField(key,v); }
        } else if(data->isAtInteger()){
            addField(data->keyAt(),QString::number(data->intAt()));
        } else {
            QList<dataChI> dataArray;
            if(data->isAtSubTree()){
                if(!m_ChildGenerators.contains(key) && !m_isBuilding){ continue;}
                dataArray.append(
                        dataChI(data->subDataAt(),data->keyAt(),childCounter));
                dataArray.append(getChildrenFromKey(key, data->subDataAt()));
            } else if(data->isAtArrayData()){
                int subIndex=0;
                do{
                    DataTree* subData = data->arrayDataAt();
                    QString  subKey = subData->keyAt();
                    qDebug() << (QString(2*(recur+1), ' ') + "array subkey: " +  subKey);
                    if(!m_isBuilding && !m_ChildGenerators.contains(key) ){
                        if(m_drillDownChildPaths.contains(subKey)){
                            dataArray.append(getChildrenFromKey(subKey, subData));
                        }
                    } else {
                        dataArray.append(dataChI(subData,key, subIndex));
                        dataArray.append(getChildrenFromKey(subKey, data->subDataAt()));
                    }
                } while(data->gotoNextArrayData());
            } else {
                qDebug() << "addData ignoring the key type:" << key;
            }
            addDataArray(dataArray, recur);
            for(int i=0; i<dataArray.count(); i++ ){
                data->eatSubData(dataArray[i].data);
            }
        }
    }
    scanForPathValue(data);
    return m_dataLimit + data->informationRead();

}

void FillinItem::addDataArray(QList<dataChI> dataArray, int recur){
    Q_ASSERT(recur <30);
    for(int i=0; i<dataArray.count(); i++ ){
        DataTree* subData = dataArray[i].data;
        if(subData){
            QString key = dataArray[i].parentKey;
            int parentIndex = dataArray[i].parentIndex;
            qDebug() << "array key:" << key;
            FillinItem fit(m_ChildGenerators[key]);
            if(fit.m_generatingItem.getGeneratingSettings().isEmpty()){
                Q_ASSERT(!m_generatingItem.getGeneratingSettings().isEmpty());
                fit.m_generatingItem.setGeneratingSettings(m_generatingItem.getGeneratingSettings());
            }
            fit.m_isBuilding = this->m_isBuilding;
            fit.clearValues();
            fit.addData(subData,recur+1, parentIndex);
            addChild(fit,key);
        }
    }
}


QList<dataChI> FillinItem::getChildrenFromKey(QString key, DataTree* data){
    QList<dataChI> res;
    if(!m_isBuilding){
        QList<QString> subPathList = m_drillDownChildPaths[key];
        //QList<QString> subPathList = m_ChildGenerators[key].m_drillDownPaths;
        int c = subPathList.count();
        for(int i=0; i<c;i++){
            QString basePathKey = subPathList[i];
            QList<QString> expr = VerbItem::toPath(basePathKey);
            //expr.pop_front();
            getChildrenFromPath(expr, basePathKey, data, res);
        }
    }
    return res;
}

void FillinItem::getChildrenFromPath(QList<QString> path, QString basePathKey, DataTree* data, QList<dataChI>& res){
    if(data->isAtArrayData()){
        do{
            DataTree* subData = data->arrayDataAt();
            getChildrenFromPath(path, basePathKey, subData, res);
        } while(data->gotoNextArrayData());
    } else {
        getChildrenFromPathHash(path, basePathKey, data, res);
    }
}

void FillinItem::getChildrenFromPathHash(QList<QString> path, QString basePathKey, DataTree* data, QList<dataChI>& res){
    int count=0;
    for(bool go=(!(data->isEmpty())); go==true ; go= data->gotoNextHashData()) {
        QString key = data->keyAt();
        //qDebug() << "child path search subkey:  " << key << "search path: " << path;
        if(key!=path.first()){continue;}
        if(data->isAtSubTree()){
            DataTree* subData = data->subDataAt();
            if(path.length()==1){
                res.append(dataChI(subData,basePathKey, count));
                return;
            } else {
                QList<QString> subPath = path;
                subPath.pop_front();
                getChildrenFromPath(subPath, basePathKey, subData, res);
            }
        } else if(data->isAtArrayData()){
            if(path.length()==1){
                do{
                    DataTree* subData = data->arrayDataAt();
                    res.append(dataChI(subData,key, count));
                } while(data->gotoNextArrayData());
            } else {
                do{
                    DataTree* subData = data->subDataAt();
                    QList<QString> subPath = path;
                    subPath.pop_front();
                    getChildrenFromPath(subPath, basePathKey, subData, res);
                } while(data->gotoNextArrayData());
            }
        } else {
            qDebug() << "getChildrenFromPath ignoring the key:" << data->keyAt();
        }
        count++;
    }
}


void FillinItem::scanForPathValue(DataTree* data){
    for(int i=0; i<m_Attributes.count();i++){
        QList<EntryField > pathEntries = m_Attributes[i].getPathExpressions();
        for(int j=0; j<pathEntries.count();j++){
            QString pathValue = valueByDepth(data,pathEntries[j].pathExpression);
            m_Attributes[i].setField(pathEntries[j].name,pathValue);
        }
    }
}

QString FillinItem::valueByDepth(DataTree* data, QList<QString> path){
    Q_ASSERT(data);
    //Q_ASSERT(data->isAtSubTree());
    data->reset();

    bool go=(!(data->isEmpty()));
    while(go) {
        QString key = data->keyAt();
        if(key!=path.first()){
            go = data->gotoNextHashData();
            continue;
        }
        QList<QString> subpath = path;
        subpath.pop_front();

        if(data->isAtString() || data->isAtInteger()){
            if(path.length()==1){
                QString v = data->stringAt();
                return v;
            }
            qDebug() << "warning: path value not tree" << path.first();
        } else if(data->isAtSubTree()){
            DataTree* subData = data->subDataAt();
            QString res = valueByDepth(subData,subpath);
            data->eatSubData(subData);
            return res;
        } else if(data->isAtArrayData()){
            do{
                DataTree* subData = data->arrayDataAt();
                QString key = data->keyAt();
                if(key!=path.first()){continue;}
                QString res = valueByDepth(subData,subpath);
                data->eatSubData(subData);
                return res;
            } while(data->gotoNextArrayData());
        } else {
            //qDebug() << "valueByDepth ignoring the key:" << data->keyAt();
            return "";
        }
        go = data->gotoNextHashData();
    }
    return "";
}




void FillinItem::addHash(QHash<QString, QString> hash){
    QHash<QString, QString>::iterator i = hash.begin();
    for(;i!= hash.end(); i++){
        addField(i.key(),i.value());
    }
}

void FillinItem::addGlobals(QHash<QString, EntryField> hash){
    QHash<QString, EntryField>::iterator i = hash.begin();
    for(;i!= hash.end(); i++){
        addField(i.key(),i.value().value, i.value().valueItem);
    }
    m_globalValues = hash;

}


void FillinItem::addChild(FillinItem fit, QString key){

    if(m_isBuilding){
        if(fit.getIsExtraChild()){
            m_ExtraChildGenerators.append(fit);
        } else {
            qDebug() << "adding child creator with key:" << key;
            m_ChildGenerators[key] = fit;
            if(VerbItem::atPathField(key)){
                QList<QString> pathExpr = VerbItem::toPath(key);
                m_drillDownChildPaths[pathExpr[0]].append(key);
            }
        }
    } else {
        //I could add a system for transforming children later
        fit.setGenerating();
        int childIndex = fit.m_parentIndex;
        CatItem item;
        if(fit.isMergeWithParent()){
            QHash<QString, QString> childAttrs;
            item = fit.createTheItem(m_generatingItem, m_globalValues, key, &childAttrs);
            QHash<QString, QString>::iterator i = childAttrs.begin();
            for(; i!=childAttrs.end(); i++){
                addField(i.key(), i.value());
            }
        } else {
            item = fit.createTheItem(m_generatingItem, m_globalValues);
        }
        if(item.isOrderedBySibling()){
            item.setSiblingOrder(childIndex);
        }

        if(fit.isJustTakeChildren()){
            QList<CatItem> childs = item.getChildren();
            m_children.append(childs);
            QList<CatItem> parents = item.getParents();
            m_parents.append(parents);
        } else if(item.isEmpty()){
            qDebug() << "empty fillin item for: " << key;
        } else  if(fit.isParent()){
            m_parents.append(item);
        } else {
            m_children.append(item);
        }



    }
}

void FillinItem::addInternalGenerator(){
    QString genTreeFile=m_generatingItem.getGeneratingSettings();

    if(!genTreeFile.isEmpty()){
        QSettings s(genTreeFile, QSettings::IniFormat);
        QString genTreeKey = s.value(GEN_KEY_SETTINGS).toString();
        SettingsTree st(&s,genTreeKey);
        addData(&st,0,0);
    }
}

void FillinItem::addField(QString k, QString v, CatItem it){

    //qDebug() << "addField k:" << k  << "value" << v;
    if(m_isBuilding){
        if(k ==(IS_PARENT_KEY)){
            setParent();
        } else if(k == (TAKE_CHILD_KEY)){
            setJustTakeChildren();
        } else if(k == (MERGE_W_PARENT_KEY)){
            setMergeWithParents();
        } else if(k == (GEN_FROM_PARENT_KEY)){
            setIsExtraChild();
        } else {
            VerbItem vi(k);
            vi.setInput(v);
            vi.setCustomPluginInfo(ATTRIBUTE_GENERATOR_KEY,v);
            m_Attributes.append(vi);
        }
    } else {
        for(int i=0;i< this->m_Attributes.count(); i++){
            m_Attributes[i].setField(k,v,"",it);
        }
        for(int i=0;i< m_ExtraChildGenerators.count(); i++){
            m_ExtraChildGenerators[i].setGenerating();
            m_ExtraChildGenerators[i].addField(k,v,it);
        }
    }
}

CatItem FillinItem::createChildren(CatItem item, QHash<QString, EntryField> vars){
    FillinItem fi(item);
    fi.setGenerating();
    fi.addGlobals(vars);
    fi.addInternalGenerator();
    VerbItem vi(item);
    return fi.createTheItem(vi, QHash<QString, EntryField>());

}

CatItem FillinItem::createTheItem( VerbItem parentItem,QHash<QString, EntryField> globals,
                                   QString prefix, QHash<QString, QString>* valuesAsHash){
    Q_ASSERT(!m_isBuilding);


    qDebug() << "createTheItem - parentItem" << parentItem.getPath();

    //Children should be added already...
    QHash<QString, EntryField> vars = globals;
    if(!vars.isEmpty()){
        for(int j=0;j< m_Attributes.count(); j++){
            QHash<QString, EntryField>::iterator i = vars.begin();
            for(; i!=vars.end(); i++){
                EntryField e = i.value();
                e.name = i.key();
                //e.valueItem = i.value().valueItem;
                m_Attributes[j].setUnasignedField(e);
                //addField(i.key(), i.value());
            }
        }
    }

    CatItem res;
    if(!parentItem.isEmpty()){
        QHash<QString, CatItem> pAttr = parentItem.getArgHash();
        QHash<QString, CatItem>::iterator i = pAttr.begin();
        for(; i!=pAttr.end(); i++){
            QString k = i.key();
            QString v = i.value().getName();
            if(v.isEmpty()){ continue;}
            qDebug() << "for parrent item Key:" << k << "setting value:" << v;
            res.setCustomPluginInfo( k,v);
        }
    }

    for(int i=0;i< m_Attributes.count(); i++){
        QString genString =m_Attributes[i].getCustomString(ATTRIBUTE_GENERATOR_KEY);
        QString k = m_Attributes[i].getName();
        QString v = m_Attributes[i].fillInFields(genString);
        if(v.isEmpty()){ continue;}
        //qDebug() << "for m_attrib Key:" << k << "setting value:" << v << "attrib path" << m_Attributes[i].getPath();
        res.setCustomPluginInfo(k , v);
        if(valuesAsHash){
            valuesAsHash->insert(prefix + FIELD_TYPE_SEPERATOR + m_Attributes[i].getName(),
                        m_Attributes[i].fillInFields(genString));
        }
    }

    if(res.isEmpty()){
        qDebug() << "adding placehold path";
        res.setPath(PLACEHOLDER_PATH);
    }

    for(int i=0;i< m_ExtraChildGenerators.count(); i++){
        m_ExtraChildGenerators[i].setGenerating();
        CatItem cItem;
        if(globals.isEmpty()){
            cItem = m_ExtraChildGenerators[i].createTheItem(parentItem, globals);
        } else {
            cItem = m_ExtraChildGenerators[i].createTheItem(parentItem, globals);
        }
        if(!parentItem.isEmpty()){
            cItem.addParent(parentItem);
        }
        addPath(&cItem, res);

        if(m_ExtraChildGenerators[i].isParent()){
            Q_ASSERT(!res.hasParent(cItem));
            res.addParent(cItem);
        } else {
            Q_ASSERT(!res.hasChild(cItem));
            res.addChild(cItem);
        }
    }

    for(int i=0;i< m_children.count(); i++){
        CatItem cItem = m_children[i];
        addPath(&cItem, res);
        res.addChild(cItem);
    }
    for(int i=0;i< m_parents.count(); i++){
        CatItem it = m_parents[i];
        addPath(&it, res);
        res.addParent(it);
    }

    if(res.getCustomString(FILLIN_GEN_FILE_KEY).isEmpty()){
        QString genFile = m_generatingItem.getCustomString(FILLIN_GEN_FILE_KEY);
        if(genFile.isEmpty()){
            genFile = m_generatingItem.getGeneratingSettings();
        }
        Q_ASSERT(!genFile.isEmpty());
        res.setCustomPluginInfo(FILLIN_GEN_FILE_KEY,genFile);
    }
    qDebug() << "fillin - created item name: " << res.getName();
    qDebug() << "path: " << res.getPath();
    return res;
}

void FillinItem::addPath(CatItem* it, CatItem parent){
    if(it->getPath() ==PLACEHOLDER_PATH){
        if(!it->getName().isEmpty()){
            it->setPath(addPaths(parent.getPath(),
                it->getName()));
        }
    }


}

void FillinItem::clearValues(){
    for(int i=0;i< this->m_Attributes.count(); i++){
        m_Attributes[i].clearForParse();
    }

    QHash<QString, FillinItem>::iterator i = m_ChildGenerators.begin();
    for(;i!= m_ChildGenerators.end(); i++){
        m_ChildGenerators[i.key()].clearValues();
    }

    for(int i=0;i< this->m_ExtraChildGenerators.count(); i++){
        m_ExtraChildGenerators[i].clearValues();
    }
}




//int FillinItem::addData(DataTree* data, int recur){
//    Q_ASSERT(data);
//    //Q_ASSERT(data->isAtSubTree());
//
//    if(recur >= m_depthLimit){return 0;}
//
//    QString offset((recur+1)*2,QChar(' '));
//    offset = "|" + offset;
//
//
//    bool go = !(data->isEmpty());
//    while(go) {
//        //qDebug() << offset + "looping with:" << data->keyAt();
//
//        if(data->isAtString()){
//            QString k = data->keyAt();
//            QString v = data->stringAt();
//            addField(k,v);
//        } else if(data->isAtInteger()){
//            addField(data->keyAt(),QString::number(data->intAt()));
//        } else if(data->isAtSubTree()){
//            QString key = data->keyAt();
//            if(m_ChildGenerators.contains(key) || m_isBuilding){
//                DataTree* subData = data->subDataAt();
//                m_ChildGenerators[key].m_isBuilding = this->m_isBuilding;
//                m_ChildGenerators[key].addData(subData,recur+1);
//                addChild(
//                        m_ChildGenerators[data->keyAt()], data->keyAt());
//                data->eatSubData(subData);
//            } else {
//                qDebug() << offset + "no tree at:" << key;
//            }
//
//        } else if(data->isAtArrayData()){
//            do{
//                DataTree* subData = data->arrayDataAt();
//                QString key = data->keyAt();
//                qDebug() << "array key:" << key;
//                if(m_ChildGenerators.contains(key)){ //subData->isAtSubTree() &&
//                    FillinItem fit(m_ChildGenerators[key]);
//                    if(fit.m_generatingItem.getGeneratingSettings().isEmpty()){
//                        fit.m_generatingItem.setGeneratingSettings(m_generatingItem.getGeneratingSettings());
//                    }
//                    fit.m_isBuilding = this->m_isBuilding;
//                    fit.clearValues();
//                    fit.addData(subData,recur+1);
//                    addChild(fit,data->keyAt());
//                } else {
//                    qDebug() << offset + "no subdata at array:" << key;
//                }
//                data->eatSubData(subData);
//            } while(data->gotoNextArrayData());
//        } else {
//            qDebug() << offset + "ignoring the key:" << data->keyAt();
//        }
//        go = data->gotoNextHashData();
//    }
//    scanForPathValue(data);
//    return m_dataLimit + data->informationRead();
//
//}
