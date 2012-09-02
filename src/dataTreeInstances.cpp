
#include <QList>
#include <QHash>
#include <QVariant>
#include "fillInItem.h"
#include <QtXml>

class XMLDataTree; //just for the editor...

XMLDataTree::XMLDataTree(QXmlStreamReader* reader){
    m_reader = reader;
    Q_ASSERT(m_reader->isStartDocument()
             || m_reader->isStartElement()
             || isAtArrayData());
    m_isTextElement = false;
    m_isBeginElement = false;
    m_bodyTextFound = false;

    if(isAtArrayData()){
        m_isTextElement = true;
    } else if(m_reader->isStartElement()){
        m_attributes = m_reader->attributes();
        m_startElementName = m_reader->name().toString();
    } else if(m_reader->isStartDocument()){
        m_isBeginElement = true;
    }
}

bool XMLDataTree::isAtString(){
    if(m_reader->atEnd()){return false;}
    return !isAtInteger() && !isAtArrayData() && !isAtSubTree();
}

bool XMLDataTree::isAtInteger(){
    if(m_reader->atEnd()){return false;}
    if(m_attribIter == m_attributes.end()){return false;}

    QString v = m_attribIter->value().toString();
    bool ok;
    v.toInt(&ok);
    return ok;

}

bool XMLDataTree::isAtArrayData(){
    if(m_reader->atEnd()){return false;}
    if(m_attribIter != m_attributes.end()){return false;}
    if(m_reader->isCDATA() || m_reader->isCharacters()){return true;}
    return false;

}

bool XMLDataTree::isAtSubTree(){
    if(m_reader->atEnd()){return false;}
    if(m_attribIter != m_attributes.end()){ return false; }
    if(isAtArrayData()){return false;}

    Q_ASSERT(m_reader->isStartDocument() || m_reader->isStartElement());
    return true;

}

QString XMLDataTree::stringAt(){
    if(m_reader->atEnd()){return false;}
    return m_attribIter->value().toString();

}

int XMLDataTree::intAt(){
    if(m_reader->atEnd()){return false;}
    QString v = m_attribIter->value().toString();
    return v.toInt();
}

DataTree* XMLDataTree::subDataAt(){
    Q_ASSERT( m_reader->isStartElement());
    Q_ASSERT(m_attribIter == this->m_attributes.end());

    DataTree* dt = new XMLDataTree(m_reader);
    return dt;

}

DataTree* XMLDataTree::arrayDataAt(){
    DataTree* dt = new XMLDataTree(m_reader);
    return dt;

}

QString XMLDataTree::keyAt(){
    if(m_attribIter != m_attributes.end()){
        return m_attribIter->value().toString();
    } else {
        return FORMAL_XML_BODY_NAME;
    }
}

QString XMLDataTree::arrayKeyAt(){
    if(m_attribIter != m_attributes.end()){
        return m_attribIter->value().toString();
    } else {
        return FORMAL_XML_BODY_NAME;
    }
}

bool XMLDataTree::gotoNextHashData(){
    if(m_attribIter != m_attributes.end()){
        m_attribIter++;
        return true;
    }

    if(m_reader->atEnd()){
        return false;
    }

    if(m_reader->isEndElement() ){
        if(m_reader->name() == m_startElementName){
            return false;
        }
    }
    if(!isAtArrayData()){
        m_reader->readNext();
    }
    return true;
}

bool XMLDataTree::gotoNextArrayData(){
    Q_ASSERT(m_attribIter == m_attributes.end());

    if(m_reader->atEnd()){
        return false;
    }

    if(m_reader->isEndElement() ){
        if(m_reader->name() == m_startElementName){
            return false;
        }
    }
    m_reader->readNext();
    return true;
}





////////

bool SettingsTree::gotoNextHashData(){
    m_keyPosition++;
    if(m_keyPosition < m_keyList.count()){
        return true;
    } else {
        return false;
    }
}

void SettingsTree::reset(){
    m_keyPosition=0;
    m_indexPosition= 0;
    m_childIndex=0;
}


bool SettingsTree::isAtString(){
    return !isAtInteger()
            && !isAtSubTree() && !isAtArrayData();
}

bool SettingsTree::isAtInteger(){
    QString v = m_set->value(m_currentGroup + "/"+m_keyList[m_keyPosition],"").toString();
    bool ok;
    v.toInt(&ok);
    return ok;
}


bool SettingsTree::isAtSubTree(){
    if(m_keyList[m_keyPosition].startsWith(CHILD_GEN_PREFIX)){
        return true;
    } else{
        return false;
    }
}




bool SettingsTree::gotoNextArrayData(){
    if(!m_atIndex){
        m_childIndex++;
        if(m_childIndex>=m_childIndexSize){
            m_childIndex=-1;
            m_childIndexSize=-1;
            return false;
        }
    } else {
        m_indexPosition++;
        if(m_indexPosition>=m_indexSize-1){
            m_indexPosition=-1;
            return false;
        }
    }
    return true;
}
DataTree* SettingsTree::arrayDataAt(){
    QString groupKey = m_set->value(m_currentGroup +"/"+ m_keyList[m_keyPosition]).toString();
    if(m_atIndex){
        return (new SettingsTree(m_set, groupKey, m_indexPosition));
    } else {
        //Q_ASSERT(m_childIndex>-1);
        qDebug() << "setting tree with key: " << groupKey << " index: " << m_childIndex;

        SettingsTree*  dt = new SettingsTree(m_set, groupKey, m_childIndex);
        if(isAtArrayData() && m_childIndexSize==-1){
            m_childIndexSize = dt->getIndexSize();
        }
        return dt;
    }
}


DataTree* SettingsTree::subDataAt(){

    return arrayDataAt();

}


QString SettingsTree::stringAt(){
    QString k;
    if(m_atIndex){
        //QSetting arrays begin at 1 - not 0!!!
        k = m_currentGroup +"/"
            +  QString::number(m_indexPosition + 1 )
            + "\\" +m_keyList[m_keyPosition];
    } else if(this->m_indexPosition==-1){
        k= m_currentGroup +"/"+ m_keyList[m_keyPosition];
    } else {
        k = m_keyList[m_keyPosition];
        m_set->beginReadArray(m_currentGroup);
        m_set->setArrayIndex(m_indexPosition+1);
        QString v = m_set->value(k,"").toString();
        //qDebug() << "from: " << k << " got: " << v;
        m_set->endArray();
        return v;
    }
    QString v = m_set->value(k,"").toString();
    //qDebug() << "from: " << k << " got: " << v;
    return v;
}

QString SettingsTree::keyAt(){
    if(m_atIndex){
        return m_currentGroup;
    }
    Q_ASSERT((m_keyPosition < m_keyList.length()));
    if(m_keyList[m_keyPosition].startsWith(CHILD_GEN_PREFIX)){
        return m_keyList[m_keyPosition].mid(CHILD_GEN_PREFIX.length());
    } else  if(m_keyList[m_keyPosition].startsWith(ARRAY_CHILD_GEN_PREFIX)){
        return m_keyList[m_keyPosition].mid(ARRAY_CHILD_GEN_PREFIX.length());
    } else {
        return m_keyList[m_keyPosition];
    }
}

bool SettingsTree::isAtArrayData(){
    if(m_atIndex){
        return true;
    } else if(!m_keyList.isEmpty() && m_keyList[m_keyPosition].startsWith(ARRAY_CHILD_GEN_PREFIX)){
        if(m_childIndex==-1){
            m_childIndex=0;
        }
        return true;
    } else {
        return false;
    }
}

QString SettingsTree::arrayKeyAt(){
    if(m_atIndex){
        return m_currentGroup;
    }

    Q_ASSERT((m_keyPosition < m_keyList.length()));
    if(m_keyList[m_keyPosition].startsWith(CHILD_GEN_PREFIX)){
        return m_keyList[m_keyPosition].mid(CHILD_GEN_PREFIX.length());
    } else  if(m_keyList[m_keyPosition].startsWith(ARRAY_CHILD_GEN_PREFIX)){
        return m_keyList[m_keyPosition].mid(ARRAY_CHILD_GEN_PREFIX.length());
    } else {
        return m_keyList[m_keyPosition];
    }


}

int SettingsTree::intAt(){
    QString v = m_set->value(m_currentGroup +"/"+ m_keyList[m_keyPosition],0).toString();
    return v.toInt();
}

void SettingsTree::eatSubData(DataTree* st){
    if(st){ delete st;}
}

bool SettingsTree::isEmpty(){
    if(m_keyList.count()==0){
        qDebug() << "null settings tree:" << this->m_currentGroup;
        return true;
    } else  if(m_keyPosition >= m_keyList.count()){
        return true;
    } else {
        return false;
    }
}

/////////////////////////

VariantTree::VariantTree(QVariant v){
    if( v.canConvert((QVariant::Map))){
        m_keyHash = v.toMap();
        if(m_keyHash.count()>0){
            m_keyHashIter = m_keyHash.begin();
        } else {
            m_keyHashIter = m_keyHash.end();
            Q_ASSERT(m_keyHashIter == m_keyHash.end());
        }

    } else if( v.canConvert((QVariant::List))){
        m_list = v.toList();
        m_listIndex = 0;
        m_keyHash.clear();
        m_keyHashIter = m_keyHash.end();
        qDebug() << "creating array version of Variant tree length: " << m_list.count();
    } else {
        qDebug() << "unprocess QVariant tree"  << v.typeName();
        m_listIndex = 0;
        //Q_ASSERT(false);
    }
}

void VariantTree::reset(){
    if(m_keyHash.count() ==0 ) { return;}
    m_keyHashIter = m_keyHash.begin();
}


bool VariantTree::gotoNextHashData(){
    if(m_keyHash.count() ==0 ) { return false;}

    m_keyHashIter++;

    if(m_keyHashIter == m_keyHash.end()){
        return false;
    }
    m_list.clear();
//    else if(m_keyHashIter.value().canConvert((QVariant::List))){
//        m_list = m_keyHashIter.value().toList();
//    } else {
//        m_list.clear();
//    }
//    m_listIndex = 0;
    return true;
}

bool VariantTree::isAtString(){
    if(m_keyHash.count() ==0 ) { return false;}
    if(m_keyHashIter == m_keyHash.end()){
        return false;
    }
    return m_keyHashIter.value().canConvert((QVariant::String));
}

bool VariantTree::isAtInteger(){
    if(m_keyHash.count() ==0 ) { return false;}
    if(m_keyHashIter == m_keyHash.end()){
        return false;
    }
    return m_keyHashIter.value().canConvert((QVariant::Int));
}

bool VariantTree::isAtSubTree(){
    if(m_keyHash.count() ==0 ) { return false;}
    if(m_keyHashIter == m_keyHash.end()){
        return false;
    }

    if(m_keyHashIter.value().canConvert((QVariant::Map))){
        return true;
    };
    return false;
}

bool VariantTree::isAtArrayData(){

    if(m_list.length()>0){ return true; }
    if(m_keyHash.count() ==0 ) { return false;}
    if( m_keyHashIter.value().canConvert((QVariant::List))){
        m_list = m_keyHashIter.value().toList();
        m_listIndex = 0;
        if(m_list.count()>0){
            return true;
        }
    }
    return false;
}

bool VariantTree::gotoNextArrayData(){
    m_listIndex++;
    return (m_listIndex < m_list.count());
}

DataTree* VariantTree::arrayDataAt(){
    Q_ASSERT(m_listIndex < m_list.count());
    DataTree* res = new VariantTree (m_list[m_listIndex]);
    return res;

}


QString VariantTree::stringAt(){
    QString v = m_keyHashIter.value().toString();
    QString vAlt = m_keyHash[m_keyHashIter.key()].toString();
    Q_ASSERT(v == vAlt);
    qDebug() << "variant tree string value: " << v;
    return v;
}

int VariantTree::intAt(){
    QVariant v = m_keyHashIter.value();
    return v.toInt();
}

QString VariantTree::keyAt(){
    if(m_keyHash.count() ==0 || m_keyHashIter == m_keyHash.end()) {
        if(m_list.length()>0){
            return "list_dummy";
        } else {
            return "";
        }
    }
    QString v = m_keyHashIter.key();
    return v;
}

QString VariantTree::arrayKeyAt(){
    QString v = m_keyHashIter.key();
    return v;
}

DataTree* VariantTree::subDataAt(){
    Q_ASSERT(m_keyHashIter != m_keyHash.end());
    DataTree* res = new VariantTree (m_keyHashIter.value());
    return res;

}


void VariantTree::eatSubData(DataTree* st){
    delete st;
}
