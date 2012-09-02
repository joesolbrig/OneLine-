#ifndef HISTORY_ITEM_H
#define HISTORY_ITEM_H

#include "item.h"
#include "item_rep.h"
#include "input_list.h"
#include <QHash>

#include "util/mmap_file/btree.h"

struct HistoryEvent{
    friend QDataStream &operator >> (QDataStream &in, CatItem &HistoryEvent);
    friend QDataStream &operator<<(QDataStream &out, const CatItem &HistoryEvent);

    time_t time;
    QString itemExpr;
    QString itemPath;
    UserEvent userEventType;
    QString itemName;
    QHash<QString, QString> userInputFields;

    static QString typeName(){return QString("HistoryEvent");}

    HistoryEvent(){}
    HistoryEvent(const HistoryEvent& h){
        itemExpr = h.itemExpr;
        itemPath = h.itemPath;
        userEventType = h.userEventType;
        time = h.time;
        itemName = h.itemName;
        userInputFields = h.userInputFields;
    }

    HistoryEvent& operator=(const HistoryEvent& h){
        itemExpr = h.itemExpr;
        itemPath = h.itemPath;
        userEventType = h.userEventType;
        time = h.time;
        itemName = h.itemName;
        userInputFields = h.userInputFields;
        return *this;
    }

    HistoryEvent(QString iN, QHash<QString, QString> uIF, QString itExpr, UserEvent evt, time_t n){
        itemExpr = itExpr;
        userEventType = evt;
        time = n;
        itemName = iN;
        userInputFields = uIF;
    }

    HistoryEvent(CatItem item, UserEvent evt, time_t n){

        QStringList l;
        l.append(item.getPath());
        l.append("0");
        itemExpr =  addPrefix(HISTORY_PREFIX ,combineStrings(l));
        itemPath = item.getPath();

        userEventType = evt;
        time = n;
        itemName = item.getName();
    }

    CatItem toCatItem(){
        CatItem it( addPrefix(HISTORY_PREFIX ,(itemExpr + QString::number(stringHashHash(userInputFields) ))));
        if(!itemName.isEmpty()){
            it.setName(this->itemName);
        }
        CatItem histSrc(HISTORY_SRC_PATH);
        it.setExternalWeightFromTime(time, histSrc);
        it.setCustomPluginValue(HISTORY_INTERNAL_TIME_KEY, time);
        it.setCustomPluginInfo(HISTORY_EVENT_TYPE_KEY, userEventType.toStr());
        it.setCustomPluginInfo(HISTORY_USER_INPUT_KEY, picklHashToEscdString(userInputFields));
        return it;
    }

    //Member needed for object DB
    void merge(const HistoryEvent h){
        Q_ASSERT(time == h.time && (itemExpr == h.itemExpr));
        userEventType = h.userEventType;
    }
    QString getId(){
        if(itemExpr.isEmpty()){return QString();}
        return itemExpr + QString("%1").arg(time);
    }

    bool isEmpty(){
        return itemExpr.isEmpty();
    }

};

//This has to be updated for each member of CatItem, what a drag...
inline QDataStream &operator<<(QDataStream &out, const HistoryEvent &item) {

    out <<  item.userInputFields;
    out <<  ((qint32)item.time);
    out <<  item.itemExpr;
    out <<  item.itemPath;
    out <<  item.userEventType;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, HistoryEvent &item) {

    in >>  item.userInputFields;
    qint32 l;
    in >> l;
    item.time = (time_t)l;
    in >> item.itemExpr;
    in >> item.itemPath;
    in >> item.userEventType;
    return in;
}

#endif // HISTORY_ITEM_H
