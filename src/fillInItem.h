#ifndef FILLINITEM_H
#define FILLINITEM_H

#include <QSettings>
#include <QtXmlPatterns>
#include "inputLanguage.h"


class DataTree{

public:
    virtual bool gotoNextArrayData()=0;
    virtual bool gotoNextHashData()=0;

    virtual bool isAtString()=0;
    virtual bool isAtInteger()=0;
    virtual bool isAtSubTree()=0;

    virtual bool isAtArrayData()=0;

    virtual QString stringAt()=0;
    virtual int intAt()=0;
    virtual DataTree* subDataAt()=0;
    virtual void eatSubData(DataTree* st)=0;
    virtual QString keyAt()=0;
    virtual QString arrayKeyAt()=0;

    virtual DataTree* arrayDataAt()=0;
    //virtual int indexAt()=0;

//    virtual bool hasInternalPath()=0;
//    virtual QString internalPathAt()=0;

    virtual qint32 informationRead(){return 0;};
    virtual bool isEmpty(){return false;}
    virtual void reset(){}

    //DataTree(DataTree& toCopy)=0;


};

struct dataChI {
    DataTree* data;
    QString parentKey;
    int parentIndex;

    dataChI(DataTree* d,QString k, int c){
        data = d;
        parentKey = k;
        parentIndex = c;
    }
    dataChI(){
        data=0;
        parentIndex=0;
    }

};


class XMLDataTree : public DataTree{

    XMLDataTree(QXmlStreamReader* m_reader);

    bool isAtString();
    bool isAtInteger();
    bool isAtSubTree();
    bool isAtArrayData();

    QString stringAt();
    int intAt();

    //Hash stuff
    bool gotoNextHashData();
    bool gotoNextListData();
    DataTree* subDataAt();
    QString keyAt();
    QString arrayKeyAt();

    //Array stuff
    bool gotoNextArrayData();
    DataTree* arrayDataAt();
    int indexAt();

    virtual qint32 informationRead(){
        return m_reader->characterOffset();

    };
    virtual void eatSubData(DataTree* st){delete st;};


private:

    QXmlStreamReader* m_reader;
    QString m_startElementName;

    QXmlStreamAttributes m_attributes;
    QXmlStreamAttributes::iterator m_attribIter;

    bool m_isTextElement;
    bool m_isBeginElement;

    bool m_bodyTextFound;
    QString m_subElementBelowText;

};




class SettingsTree: public DataTree{
public:

    bool isAtString();
    bool isAtInteger();
    bool isAtSubTree();
    bool isAtArrayData();

    QString stringAt();
    int intAt();

    //Hash stuff
    bool gotoNextHashData();
    DataTree* subDataAt();
    virtual void eatSubData(DataTree* st);
    QString keyAt();
    QString arrayKeyAt();

    //Array stuff
    bool gotoNextArrayData();
    DataTree* arrayDataAt();

    qint32 informationRead(){return 0;}

    int getIndexSize(){
        return m_indexSize;
    }
    void reset();

    SettingsTree(QSettings* set, QString group, int indexPos=-1){
        m_set = set;
        m_currentGroup = group;
        m_keyPosition = 0;

        m_indexPosition=indexPos;
        m_atIndex=false;

        m_childIndex=-1;
        m_childIndexSize=-1;

        if(!m_atIndex && m_indexPosition==-1){
            m_set->beginGroup(m_currentGroup);
            m_keyList = m_set->childKeys();
            m_set->endGroup();
        } else if(m_atIndex) {
            m_keyList = m_set->childKeys();
        } else {
            m_indexSize = m_set->beginReadArray(m_currentGroup);
            m_set->setArrayIndex(m_indexPosition);
            m_keyList = m_set->allKeys();
            m_set->endArray();
        }
    }

    virtual bool isEmpty();

    //States:
    // No array - string-value
    // No array - hash-value
    // No array - But within array tree
    // Array - At coding tree in parent
    // Array

private:
    QSettings* m_set;
    QString m_currentGroup;

    int m_keyPosition;
    QList<QString> m_keyList;

    int m_indexPosition;
    bool m_atIndex;
    int m_indexSize;

    int m_childIndex;
    int m_childIndexSize;
};

class VariantTree: public DataTree{

public:
    VariantTree(QVariant v);

    void reset();

    bool isAtString();
    bool isAtInteger();
    bool isAtSubTree();
    bool isAtArrayData();

    QString stringAt();
    int intAt();

    //Hash stuff
    bool gotoNextHashData();
    DataTree* subDataAt();
    virtual void eatSubData(DataTree* st);
    QString keyAt();
    QString arrayKeyAt();

    //Array stuff
    bool gotoNextArrayData();
    DataTree* arrayDataAt();
    //int indexAt();

//    bool hasInternalPath();
//    QString internalPathAt()=0;

    qint32 informationRead(){return 0;}

private:
    
    QMap<QString, QVariant> m_keyHash;
    QMap<QString, QVariant>::iterator m_keyHashIter;

    QVariantList m_list;
    int m_listIndex;

};

class FillinItem {

public:
    FillinItem(CatItem parent, int parentIndex=0);

    FillinItem(DataTree* data=0, int dataLimit=-1, int parentIndex=0);
    int addData(DataTree* set, int recur, int count);
    int addHashData(DataTree* set, int recur, int count);
    void addDataArray(QList<dataChI> dataArray, int recur=0);
    QString valueByDepth(DataTree* data, QList<QString> path);
    void addField(QString k, QString v, CatItem it=CatItem());
    void addField(QString k, EntryField f){ addField(k, f.value, f.valueItem);}
    void addHash(QHash<QString, QString> hash);
    void addGlobals(QHash<QString, EntryField> hash);
    void addInternalGenerator();
    void scanForPathValue(DataTree* data);
//    CatItem createTheItem(CatItem parentItem=VerbItem(), QHash< QString, QString >
//                                              globals=QHash< QString, QString >());
    CatItem createTheItem(VerbItem parentItem,
                          QHash<QString, EntryField> globals,
                          QString prefix=QString(),
                          QHash<QString, QString>* valuesAsHash=0);

    static CatItem createChildren(CatItem item, QHash<QString, EntryField> vars);
    QList<dataChI> getChildrenFromKey(QString key, DataTree* data);
    void getChildrenFromPath(QList<QString> path, QString basePathKey, DataTree* data, QList<dataChI>& res);
    void getChildrenFromPathHash(QList<QString> path, QString basePathKey, DataTree* data, QList<dataChI>& res);
    bool isGenerating(){ return !m_isBuilding;}
    void setGeneratingItem(CatItem it){
        m_generatingItem = it;
    }
    void setGenerating(){
        m_isBuilding=false;
        QHash<QString, FillinItem>::iterator i = m_ChildGenerators.begin();
        for(;i!= m_ChildGenerators.end(); i++){
            QString k = i.key();
            qDebug() << "has gen at: " << k;
            m_ChildGenerators[k].m_isBuilding=true;
        }
        for(int i=0;i< m_ExtraChildGenerators.count(); i++){
            m_ExtraChildGenerators[i].m_isBuilding=false;
            QHash<QString, FillinItem>::iterator innterItter = (m_ExtraChildGenerators[i].m_ChildGenerators).begin();
            for(;innterItter!= m_ExtraChildGenerators[i].m_ChildGenerators.end(); innterItter++){
                QString k = innterItter.key();
                qDebug() << "has gen at: " << k;
                m_ExtraChildGenerators[i].m_ChildGenerators[k].m_isBuilding=true;
            }
        }
    }

    bool isParent(){ return m_isParent;}
    void setParent(bool isParent=true){ m_isParent=isParent;}

    bool isJustTakeChildren(){ return m_justTakeChildren;}
    void setJustTakeChildren(bool t=true){ m_justTakeChildren=t;}

    bool isMergeWithParent(){ return m_mergeWithParents;}
    void setMergeWithParents(bool t=true){ m_mergeWithParents=t;}

    QList<VerbItem> getItems(){
        return m_Attributes;
    }



protected:
    void addPath(CatItem* it, CatItem parent);

    int subDataLimit(){
        return m_dataLimit>-1 ? MAX(m_dataLimit -m_dataRead, 0) : -1;

    }

    void addChild(FillinItem, QString key);

    void clear();

    QHash<QString, FillinItem> createFillinList(DataTree* data=0);
    QHash<QString, FillinItem> createFillinHash(DataTree* data=0);

    QList<VerbItem>  creatItemList(QSettings* set, QString group);
    void clearValues();

    FillinItem getGenChild(QString key){
        return m_ChildGenerators[key];
    }

    void addVerb(VerbItem v){
        m_Attributes.append(v);
    }

    bool getIsExtraChild(){
        return m_isExtraChild;
    }

    void setIsExtraChild(bool s=true){
        m_isExtraChild = s;
    }

private:
    VerbItem m_generatingItem;
    int m_dataLimit;
    int m_childLimit;
    int m_dataRead;
    int m_parentIndex;

    //QList<QString> m_drillDownPaths;
    QList<VerbItem> m_Attributes;
    QList<FillinItem> m_ExtraChildGenerators;
    QHash<QString, FillinItem> m_ChildGenerators;
    QHash<QString, QList<QString> > m_drillDownChildPaths;
    QHash<QString, EntryField> m_globalValues;
    QList<CatItem> m_children;
    QList<CatItem> m_parents;
    //CatItem m_itemBeingBuilt;

    bool m_isBuilding;

    bool m_isParent;
    bool m_justTakeChildren;
    bool m_mergeWithParents;
    bool m_isExtraChild;

    int m_depthLimit;


};

#endif // FILLINITEM_H
