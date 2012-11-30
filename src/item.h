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

#ifndef ITEM_H
#define ITEM_H
#include "is_test.h"
#include <QString>
#include <QStringList>
#include <QBitArray>
#include <QHash>
#include <QDataStream>
#include <QDir>
#include <QSet>
#include <QDateTime>
#include <QMap>
#include <math.h>
#include <QMutex>
#include <QSettings>
#include <QDebug>
#include "constants.h"
#include "qapplication.h"
#include "qdesktopwidget.h"

//#include "globals.h"

/*This is included nearly everywhere, so it includes some general purpose routines and classes


*/

#ifndef MAX
#define MAX(x,y) (((x) > (y))? (x):(y))
#endif

extern time_t gTime_offset_for_testing;

QString crudeHtmlFragmentToPlainText(QString html);

inline time_t appGlobalTime(){
    return QDateTime::currentDateTime().toTime_t()
            +gTime_offset_for_testing;
}


inline QRect gScreenArea(){
    int scrnNum;
    if(UI_USE_PRIMARY_SCRN){
        scrnNum = qApp->desktop()->primaryScreen();
    } else {
        scrnNum = UI_SCRN_NUMBER;
    }

    QRect scr;
    if(UI_RANGEOVER_ENTIRE_SCREEN){
        scr = qApp->desktop()->screenGeometry(scrnNum);
    } else {
        scr = qApp->desktop()->availableGeometry(scrnNum);
    }
    return scr;
}



inline QString addPrefix(QString prefix, QString path){
    if(path.length()>1 && path[0]=='/'){
        if((path.length()>1 && path[1]=='/') ||
           (prefix.length()>1 && prefix.mid(prefix.length()-2,prefix.length()-1)=="/")){
            return prefix + path.mid(2);
        }
        return prefix + path.mid(1);
    } else {return prefix + path;}
}

inline bool hasPrefix(QString path, QString prefix){
    return path.startsWith(prefix);
}


inline QString tagAs(QString string, QString tagInside){
    QString res = "<";
    res += tagInside + ">";
    res += string;
    res += "</"+ tagInside + ">";
    return res;
}

inline QString addPaths(QString path1, QString path2){
    if(path2[0]=='/' && path1[path1.count()-1]=='/'){
        return path1 + path2.mid(1);
    } else if((path2[0]=='/' || path1[path1.count()-1]=='/')) {
        return path1 + path2;
    } else {
        return path1 + '/' +path2;
    }
}


inline bool isNumberField(QString userString){
    bool ok;
    if((userString.toInt(&ok) !=0 || ok)){
        return true;
    } else return false;
}

inline bool isWebAdrField(QString userString){
    if(userString.startsWith(HTTP_PREFIX) || userString.startsWith(HTTPS_PREFIX)){
        return true;
    }else return false;
}

inline bool isEmailAdrField(QString userString){
    if(userString.contains('@') && (!userString.simplified().contains(' '))){
        return true;
    }else return false;
}

inline bool isLongText(QString userString){
    if((userString.length() > (SHORT_TEXT_CUTOFF - TEXT_CUTOFF_BOUNDARY)
        && userString.contains(' ')) ||
       userString.count(" ") >2){
        return true;
   } else return false;
}

inline bool isShortText(QString userString){
    if( userString.length() < SHORT_TEXT_LENGTH || !userString.contains(' ')){
        return true;
    }else return false;
}




inline QString combineStrings(QStringList sl){

    QString res;
    for(int i=0; i<sl.count(); i++){
        QString& in = sl[i];
        QString s(STR_SEP1 + STR_SEP2);
        s.reserve(in.length()+3);
        for(int j=0; j<sl.count(); j++){
            if( (in[j]==STR_SEP1 || (in[j]==STR_SEP2))){
                s.append('\\');
            }
            s.append(in[j]);
        }
        res.append(s);
    }
    return res;
}

inline QStringList splitEscdString(QString s){
    QStringList res;
    QString accum;
    int end = s.count()-1;
    for(int i=0; i<end; i++){
        if(s[i]==STR_SEP1 &&  s[i+1]==STR_SEP2){
            if(i !=0) { res.append(accum);}
            accum.clear();
            i++;
        } else if(i<end
                && s[i]=='\\'
                && (s[i]==STR_SEP1 || s[i]==STR_SEP2)){
            i++;
        } else {
            accum = s[i];
        }
    }
    res.append(accum);
    return res;
}

inline QHash<QString, QString> splitPickedHash(QString s){

    QHash<QString, QString> res;
    QList<QString> l = splitEscdString(s);
    int c = l.count()/2;
    Q_ASSERT((l.count() %2) ==0);

    for(int i=0; i<c; i++)
        { res[l[2*i]] = l[2*i+1]; }
    return res;

}

inline QString picklHashToEscdString(QHash<QString, QString> h){

    QHash<QString, QString>::iterator i = h.begin();
    QStringList sl;
    for(; i!=h.end(); i++){
        sl.append(i.key());
        sl.append(i.value());
    }
    return combineStrings(sl);
}

inline QString formatStringByKey(QString str,
        QList<int> matchIndex, QString keyColor = KEY_COLOR,
        QString extraAttribs ="", int limit=-1){
    QString formatedStr;
    int k = 0;
    int l = -1;
    int indexLen = matchIndex.size();
    if(indexLen > 0){
        l = matchIndex[k]; k++;
    } else if(limit==-1){
        return str;
    } else {
        return str.left(limit);
    }

    bool doingColor = false;
    formatedStr.reserve(str.length() + l);
    for(int j=0;j < str.length() &&(limit==-1||j<limit) ;j++){
       if(j == l ){
           if(!doingColor){
               doingColor = true;
               formatedStr += QString("<font color=\"") +keyColor+  "\""
                   +extraAttribs+ ">";
           }
           if(k < indexLen){
               l = matchIndex[k];k++;
           } else {l =-1;}
       }
       formatedStr += str[j];
       if (j+1!= l ){
           if(doingColor){
               doingColor = false;
               formatedStr += "</font>";
           }
       }
    }
    if (doingColor){
       formatedStr += "</font>";
    }
    return formatedStr;
}



class UserEvent {
    private:
        bool isChosen;
    public:
        enum EventType{
            CHOICE,
            KEYS_TYPE,
            SHOWN_LIST,
            SHOWN_EDITLINE,
            SHOWN_TABBED_EDIT,
            NEW_MESSAGES_NOTED
        } event_type;

        enum LoadType{
            IGNORE,
            JUST_FOUND,
            STANDARD_UPDATE,
            BECOMING_VISIBLE,
            SEEN,
            SELECTED,
            CHOSEN
        };

        UserEvent(EventType evt=CHOICE){
            event_type = evt;
            isChosen = (evt==CHOICE);
        }
        UserEvent(QString str){
            event_type = (EventType)str[0].toAscii();
            isChosen = (bool)str[1].toAscii();
        }
        bool getIsChosen() const{return isChosen;}
        void setIsChosen(bool i){isChosen=i;}
        EventType getEventType() const{return event_type;}
        void setEventType(EventType e){event_type =e;}
        QString toStr(){
            QString str;
            str.append('a'+(int)event_type);
            str.append('a'+(int)isChosen);
            return str;
        }

};

//This has to be updated for each member of whatever class
inline QDataStream &operator<<(QDataStream &out, const UserEvent &item) {

    out <<  (item.getIsChosen());
    out <<  (int)item.getEventType();
    return out;
}

inline QDataStream &operator>>(QDataStream &in, UserEvent &item) {

    bool c;
    in >> c;
    item.setIsChosen(c);;
    int e;
    in >> e;
    item.setEventType((UserEvent::EventType)e);;
    return in;
}

//To AVOID any overloading of values
uint stringHash(QString str);
long long stringHashHash(QHash<QString, QString> strHash);
bool isStandardSymbol(QChar k);


enum GeneratedPriority {
    MATCHES_TEXT,
    RELATED_TO_TEXT,
    CHILD_ACTION,
    PREFUNCTORY_ACTION
};

class InputList;

class CatItem;

extern const QChar beginEmbed;
extern const QChar endEmbed;
class DetachedChildRelation;
class BaseChildRelation;


class DbChildRelation;
class DetachedChildRelation;

//Direct keystroke matching
bool matches(CatItem* item, QString txt);


#define GLOBAL_ITEM_PARENT_ID 1



//Represents a relation between two items..

class BaseChildRelation {
    friend QDataStream &operator >> (QDataStream &in, CatItem &item);
    friend QDataStream &operator<<(QDataStream &out, const CatItem &item);

    friend QDataStream &operator >> (QDataStream &in, const DetachedChildRelation &item);
    friend QDataStream &operator<<(QDataStream &out, DetachedChildRelation &item);

    public:
        enum ChildType{
            UNDETERMINED=0,
            VERB=1,
            CATEGORY=2,
            WEIGHT_SOURCE=3,
            REVERSE_WEIGHT_SOURCE=4,
            ACTION_TYPE=5,
            CHARACTERISTIC=6
        };

        enum ChildRelType {
            ILLEGAL_REL=0,
            UNDETERMINED_REL,
            NORMAL,
            CHARACTERISTIC_RELATION,
            OPTIONAL_ACTION_CHILD,
            PREFERED_ACTION_CHILD,
            MAYBE_DEFAULT_ACTION_CHILD,
            CURRENT_DEFAULT_ACTION_ITEM,
            REPLACEMENT_DEFAULT_ACTION_ITEM,
            DATA_CATEGORY_PARENT,
            MESSAGE_SOURCE_PARENT,
            MESSAGE_SENDER_PARENT,
            ACCOUNT_SOURCE_PARENT,
            TAG_PARENT,
            SUBSTITUTE,
            SYNONYM
        };

    protected:
        qint32 m_usage;
        qint32 m_relevanceWeight;
        qint32 m_externalWeight;
        qint32 m_sourceWeight;
        qint32 m_totalWeight;
        ChildType childType;
        QString relationValue;
        ChildRelType childRelType;
        bool forDelete;
        bool isDefaultActionForApp;

    private:

        int m_siblingOrder;

    public:
        qint32 m_updateWeight;

        BaseChildRelation(){
            forDelete =false;
            isDefaultActionForApp= false;
            m_usage=0;
            m_relevanceWeight=0;
            m_externalWeight=0;
            m_sourceWeight=0;
            m_updateWeight=0;
            m_totalWeight=0;
            m_siblingOrder=-1;
        }

        virtual bool getIsDefaultAction() const {
            if(forDelete){return false;}
            if(childRelType==CURRENT_DEFAULT_ACTION_ITEM) {return true;}
            if(childRelType==REPLACEMENT_DEFAULT_ACTION_ITEM) {return true;}
            return false;
        }

        void setSiblingOrder(int order){
            m_siblingOrder=order;
        }

        int getSiblingOrder(){
            return m_siblingOrder;
        }

        bool hasSiblingOrder(){
            return (m_siblingOrder !=-1);
        }

        virtual bool getIsAction() const {
            if(forDelete){return false;}
            return childRelType==CURRENT_DEFAULT_ACTION_ITEM
                    || childRelType==REPLACEMENT_DEFAULT_ACTION_ITEM
                    || childRelType==MAYBE_DEFAULT_ACTION_CHILD
                    || childRelType== OPTIONAL_ACTION_CHILD; }

        virtual bool getIsDefault() const {
            if(forDelete){return false;}
            return childRelType==CURRENT_DEFAULT_ACTION_ITEM;
        }

        virtual ChildRelType getChildRelType() const{return childRelType;}
        virtual ChildType getChildType() const{return childType;}

        virtual bool getIsDefaultActionParent() const{ return isDefaultActionForApp;}
        virtual void setIsDefaultActionParent(bool d) { isDefaultActionForApp=d;}

        virtual QString getChildPath () const=0;
        virtual QString getParentPath() const=0;
        virtual QString getMatchPath() const=0;
        virtual qint32 getRelevanceWeight() const {return m_relevanceWeight;}
        virtual qint32 getExternalWeight() const {return m_externalWeight;}
        virtual void setExternalWeight(qint32 i) {m_externalWeight=i;}

        virtual qint32 getSourceWeight() const {return m_sourceWeight;}
        virtual void setSourceWeight(qint32 i) {m_sourceWeight=i;}
        
        virtual qint32 getTotalWeight() const {return m_totalWeight;}
        virtual void setTotalWeight(qint32 i) {m_totalWeight=i;}

        virtual qint32 getUpdateWeight() const {return MAX(m_sourceWeight, m_updateWeight);}
        virtual void setUpdateWeight(qint32 i) {m_updateWeight=i;}

        void setRelationsValue(QString s){
            relationValue = s;
        }

        QString getRelationValue() const {
            return relationValue;

        }

//        virtual qint32 getTotalWeight() const{
//            int externalWeightTemp = MAX(m_relevanceWeight, m_externalWeight/10);
//            return externalWeightTemp;
//        }

        virtual void setRelevanceWeight(qint32 i)=0;
        qint32 getUsage() const {return m_usage;}

        bool operator==(BaseChildRelation& r) const{
            return matches(r);
        }

        bool isEmpty() const {
            return getChildPath().isEmpty();
        }

        bool matches(const BaseChildRelation& i) const{
            return (getChildPath() == i.getChildPath()) &&
                    (getParentPath() == i.getParentPath()) ;
                    //&& getMatchPath() == i.getMatchPath() ;
        }

        qint32 getChildId() const{
            return stringHash(getChildPath());
        }

        qint32 getParentId() const{
            return stringHash(getParentPath());
        }

        QString getId() const {return getParentPath()
                        + META_ITEM_SEP+ getChildPath() +META_ITEM_SEP +getMatchPath();}

        void setChildType(BaseChildRelation::ChildType newChildType){
            childType = newChildType;
        }

        void setChildRelType(BaseChildRelation::ChildRelType setRelType){
            childRelType = setRelType;
        }

        void setNonDefaultAction(){
            if(childRelType==CURRENT_DEFAULT_ACTION_ITEM
                   || childRelType== OPTIONAL_ACTION_CHILD
                   || childRelType== MAYBE_DEFAULT_ACTION_CHILD
                   || childRelType== CURRENT_DEFAULT_ACTION_ITEM){
                childRelType=OPTIONAL_ACTION_CHILD;
            }
        }

        void setDefaultAction(){
            Q_ASSERT(getIsAction());
            childRelType=CURRENT_DEFAULT_ACTION_ITEM;
        }


        void mergeChildRelType(BaseChildRelation::ChildRelType newRelType){
            if(newRelType == UNDETERMINED_REL){
                return;
            }
            if(childRelType==CURRENT_DEFAULT_ACTION_ITEM
               || childRelType==MAYBE_DEFAULT_ACTION_CHILD
               || childRelType==REPLACEMENT_DEFAULT_ACTION_ITEM
               ){
                if(newRelType== OPTIONAL_ACTION_CHILD){
                    return;
                }
                if(newRelType== MAYBE_DEFAULT_ACTION_CHILD
                   && childRelType==CURRENT_DEFAULT_ACTION_ITEM){
                    return;
                }
            }
            childRelType = (BaseChildRelation::ChildRelType)MAX((int)newRelType,(int)childRelType);
        }

        QString toString(){
            return (getParentPath()+ TYPE_LIST_SEP + this->getChildPath());
        }

};

//#include "itemWFields.h"
//#include "itemWHash.h"
//#include "globals.h"
#include "itemWSHash.h"

class DetachedChildRelation : public BaseChildRelation {

    friend QDataStream &operator >> (QDataStream &in, DetachedChildRelation &item);
    friend QDataStream &operator<<(QDataStream &out, const DetachedChildRelation &item);

    private:
        CatItem parent;
        CatItem child;
        QString matchPath;
        bool recorded;

        //What keystrokes evoked this last time??
        //QMap<int, QString>* prev_keystrokes;
    public:

        void setChild(CatItem it){
            Q_ASSERT(child.isEmpty() || child == it);
            child = it;
        }
        void setParent(CatItem it){
            Q_ASSERT(parent.isEmpty() || parent == it);
            parent = it;
        }

        void setForDelete(bool d=true){ forDelete = d; }
        bool isForDelete() const{return forDelete;}

        const CatItem getChild() const { return child; }
        const CatItem getParent() const { return parent; }
        CatItem& getParentRef() { return parent; }
        CatItem& getChildRef() { return child; }

        QString getParentPath() const{ return parent.getPath(); }

        QString getChildPath() const{ return child.getPath(); }

        bool operator==(DetachedChildRelation& r) const{
            return matches(r);
        }

        bool operator==(const DetachedChildRelation& r) const{
            return matches(r);
        }

        QString getMatchPath() const {return matchPath;}
        void setMatchPath(QString s){matchPath = s;}

        DetachedChildRelation(){
            m_usage=0;
            m_relevanceWeight=0;
            m_externalWeight=0;
            m_totalWeight=0;
            recorded = false;
            forDelete = false;
            childType = BaseChildRelation::UNDETERMINED;
            childRelType = NORMAL;
        }
        DetachedChildRelation(CatItem pP, CatItem cP, QString mp="", BaseChildRelation::ChildType t = BaseChildRelation::UNDETERMINED){
            parent = pP;
            child = cP;
            Q_ASSERT(pP.getPath() !=cP.getPath());
            m_usage=0;
            m_relevanceWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_externalWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_totalWeight=MEDIUM_EXTERNAL_WEIGHT;
            matchPath = mp;
            childType = t;
            childRelType = NORMAL;
        }

        DetachedChildRelation(CatItem pP, CatItem cP, BaseChildRelation& cR){
            Q_ASSERT(pP.getPath() !=cP.getPath());
            parent = pP;
            child = cP;
            m_usage=cR.getUsage();
            m_relevanceWeight=cR.getRelevanceWeight();
            m_externalWeight=cR.getExternalWeight();
            m_sourceWeight=cR.getSourceWeight();
            m_updateWeight=cR.m_updateWeight;
            m_totalWeight=cR.getTotalWeight();
            matchPath = cR.getMatchPath();
            childType = cR.getChildType();
            childRelType = cR.getChildRelType();
            relationValue = cR.getRelationValue();
        }

        DetachedChildRelation(const DetachedChildRelation& dcr){
            child.assign_from(dcr.child);
            parent.assign_from(dcr.parent);
            //Q_ASSERT((child.getPath() !=parent.getPath()) || dcr.isEmpty());
            m_usage=dcr.getUsage();
            m_relevanceWeight=dcr.getRelevanceWeight();
            m_externalWeight=dcr.getExternalWeight();
            m_sourceWeight=dcr.getSourceWeight();
            m_updateWeight=dcr.m_updateWeight;
            m_totalWeight=dcr.getTotalWeight();
            setMatchPath(dcr.getMatchPath());
            recorded = dcr.recorded;
            childType = dcr.childType;
            childRelType = dcr.childRelType;
            relationValue = dcr.relationValue;
        }

        DetachedChildRelation& operator=( const DetachedChildRelation& dcr){
            child = dcr.getChild();
            parent = dcr.getParent();
            m_usage=dcr.getUsage();
            m_relevanceWeight=dcr.getRelevanceWeight();
            m_externalWeight=dcr.getExternalWeight();
            m_sourceWeight=dcr.getSourceWeight();
            m_updateWeight=dcr.m_updateWeight;
            m_totalWeight=dcr.getTotalWeight();
            setMatchPath(dcr.getMatchPath());
            recorded = dcr.recorded;
            childType = dcr.childType;
            childRelType = dcr.childRelType;
            relationValue = dcr.relationValue;

            return *this;
        }


        void merge( const DetachedChildRelation& dcr, bool fromParent, QSet<QString> childRecGuard){
            if(!fromParent){
                child.mergeItem(dcr.getChild(),false,false,childRecGuard);
            } else {
                parent.mergeItem(dcr.getParent(),false,false,childRecGuard);
            }
            Q_ASSERT(child.getPath() !=parent.getPath());
            m_usage=dcr.getUsage();
            m_relevanceWeight=MAX(m_relevanceWeight,dcr.getRelevanceWeight());
            m_externalWeight=MAX(m_externalWeight, dcr.getExternalWeight());
            m_totalWeight=MAX(m_totalWeight, dcr.getTotalWeight());
            m_sourceWeight=MAX(m_sourceWeight, dcr.getSourceWeight());
            m_updateWeight=MAX(m_updateWeight, dcr.m_updateWeight);
            forDelete = forDelete && dcr.forDelete;

            if(!dcr.getMatchPath().isEmpty())
                {setMatchPath(dcr.getMatchPath());}

            recorded = (dcr.recorded == true) && (recorded== true);

            if(!dcr.relationValue.isEmpty())
                {relationValue = dcr.relationValue;}
            if(dcr.childRelType != NORMAL )
                { childRelType = dcr.childRelType; }

            if(dcr.childType !=UNDETERMINED)
                { childType = dcr.childType; }

            return;
        }

        qint32 getRelevanceWeight() const {
            return m_relevanceWeight;
        }

        qint32 getParentId() const {
            return stringHash(parent.getPath());
        }

        void setRelevanceWeight(qint32 i){
            m_relevanceWeight = i;
        }

        //Only this class lets you return a real reference;
        CatItem& childAt(){
            return child;
        }
};


class DbChildRelation : public BaseChildRelation {

    friend QDataStream &operator >> (QDataStream &in, DbChildRelation &item);
    friend QDataStream &operator<<(QDataStream &out, const DbChildRelation &item);

    friend QTextStream &operator >> (QTextStream &in, DbChildRelation &item);
    friend QTextStream &operator<<(QTextStream &out, const DbChildRelation &item);

    private:
        QString parentPath;
        QString childPath;
        QString matchTypePath;

        //What keystrokes evoked this last time??
        //QMap<int, QString>* prev_keystrokes;
    public:
        static QString typeName(){return QString("DbChildRelation");}
        virtual QString getChildPath() const {return childPath;}
        virtual QString getParentPath() const {return parentPath;}
        virtual QString getMatchPath() const {return matchTypePath;}
        virtual void setMatchPath(QString mt){matchTypePath=mt;}

        DbChildRelation(){
            m_usage=0;
            m_relevanceWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_externalWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_totalWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_sourceWeight=0;
            m_updateWeight=0;
        }
        DbChildRelation(QString pP, QString cP){
            parentPath = pP;
            childPath = cP;
            m_usage=0;
            m_relevanceWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_externalWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_totalWeight=MEDIUM_EXTERNAL_WEIGHT;
            m_sourceWeight=0;
            m_updateWeight=0;
        }
        DbChildRelation(BaseChildRelation& dcr){
            childPath = dcr.getChildPath();
            parentPath = dcr.getParentPath();
            m_usage=dcr.getUsage();
            m_relevanceWeight=dcr.getRelevanceWeight();
            m_externalWeight=dcr.getExternalWeight();
            m_totalWeight=dcr.getTotalWeight();
            setMatchPath(dcr.getMatchPath());
            setChildRelType(dcr.getChildRelType());
            m_sourceWeight=0;
            m_updateWeight=0;
        }

        DbChildRelation& operator=(const DbChildRelation& dcr){
            childPath = dcr.getChildPath();
            parentPath = dcr.getParentPath();
            m_usage=dcr.getUsage();
            m_relevanceWeight=dcr.getRelevanceWeight();
            m_externalWeight=dcr.getExternalWeight();
            m_totalWeight=dcr.getTotalWeight();
            m_sourceWeight=dcr.getSourceWeight();
            m_updateWeight=dcr.m_updateWeight;
            setMatchPath(dcr.getMatchPath());
            setChildRelType(dcr.getChildRelType());
            return *this;

        }
        void merge(const DbChildRelation& dcr){
            childPath = dcr.getChildPath();
            parentPath = dcr.getParentPath();
            m_usage=dcr.getUsage();
            m_relevanceWeight=dcr.getRelevanceWeight();
            m_externalWeight=dcr.getExternalWeight();
            m_sourceWeight=dcr.getSourceWeight();
            m_updateWeight=dcr.m_updateWeight;
            m_totalWeight=dcr.getTotalWeight();
            relationValue = dcr.relationValue;
            setMatchPath(dcr.getMatchPath());
            return;

        }

        bool matches(BaseChildRelation& i){
            return getChildId() == i.getChildId();
        }


        void setCalled(){
            m_usage++;
        }

        qint32 getRelevanceWeight() const {
            return m_relevanceWeight;
        }

        qint32 getChildId() const{
            return stringHash(childPath);
        }

        qint32 getParentId() const{
            return stringHash(parentPath);
        }

        //Really only for testing...
        void setRelevanceWeight(qint32 i){
            m_relevanceWeight = i;
        }
};

QString titlize(QString str);
QString htmlize(QString str);
QString htmlizeFile(QString path);
QString htmlizeStringLines(QString path);
QString htmlFontFormat(QString str, QString font_str);
QString htmlUnderlineFontFormat(QString str, QString font_color_str);
QString htmlFontSizeFormat(QString str, int size);
QString makeBold(QString str);
QString makeItallic(QString str);

inline QString posFormat(QString str, QString font_str, int start, int numChar){
    Q_ASSERT(numChar >0);
    QString res;
    if(start > 0){
        res = str.mid(0, start);}
    if(start+1 < str.length()){
        res = res + htmlUnderlineFontFormat(str.mid(start, numChar), font_str);
    }
    if(start + numChar < str.length()){
        res = res + str.mid(start + numChar);}
    return  res;
}


bool CatLess (CatItem* left, CatItem* right);
bool CatLessNoPtr (CatItem & a, CatItem & b);
bool WeightLess (CatItem & a, CatItem & b);
int consonant_match(CatItem & item, QString userStr, QList<int>* outIndex= 0, int max_char_skips=100);
bool filterItemByKeys(CatItem& item, QString userStr, QList<int>* outIndex=0, int char_skips=100);
bool userkey_match(CatItem & item, QString userStr, QList<int>* outIndex= 0);
bool isConsonant(QChar v);
bool isVowell(QChar v);


inline QDataStream &operator<<(QDataStream &out, const DetachedChildRelation &item) {
    Q_ASSERT(!item.child.getPath().isEmpty());
    out << item.child;
    Q_ASSERT(!item.parent.getPath().isEmpty());
    out << item.parent;
    out << item.matchPath;

//    Q_ASSERT(!item.child.getPath().isEmpty());
//    out << item.child.getPath();
//    Q_ASSERT(!item.parent.getPath().isEmpty());
//    out << item.parent.getPath();
    out << item.m_usage;
    out << item.m_externalWeight;
    out << item.m_relevanceWeight;
    out << item.m_sourceWeight;
    out << item.m_updateWeight;
    out << item.m_totalWeight;
    out << item.childRelType;
    out << item.relationValue;
    out << QString("ItemChildRelation");
   return out;
}

inline QDataStream &operator>>(QDataStream &in, DetachedChildRelation &item) {
    CatItem dc;
    in >> dc;
    item.child = dc;
    CatItem dp;
    in >> dp;
    item.parent =dp;
//    QString cp;
//    in >> cp;
//    CatItem dc(cp);
//    item.child = dc;
//    QString pp;
//    in >> pp;
//    CatItem dp(pp);
//    dp.setIsTempItem(true);
//    item.parent =dp;

    QString plp;
    in >> plp;
    item.matchPath = plp;

    in >> item.m_usage;
    in >> item.m_externalWeight;
    in >> item.m_relevanceWeight;
    in >> item.m_sourceWeight;
    in >> item.m_updateWeight;
    in >> item.m_totalWeight;
    int i;
    in >> i;
    item.childRelType = (DetachedChildRelation::ChildRelType)i;
    in >> item.relationValue;
    QString buffer;
    in >> buffer;
    Q_ASSERT(buffer== "ItemChildRelation");
    return in;
}

inline QDataStream &operator<<(QDataStream &out, const DbChildRelation &item) {
    out << item.getChildPath();
    out << item.getParentPath();
    out << item.getMatchPath();
    out << item.getUsage();
    out << item.getExternalWeight();
    out << item.getSourceWeight();
    out << item.m_updateWeight;
    out << item.getRelevanceWeight();
    out << item.getTotalWeight();
    out << (int)item.childRelType;
    out << (int)item.childType;
    out << item.relationValue;
    out << QString("ItemDbRel");
    return out;
}

inline QDataStream &operator>>(QDataStream &in, DbChildRelation &item) {

    in >> item.childPath;
    in >> item.parentPath;
    in >>  item.matchTypePath;
    in >> item.m_usage;
    in >> item.m_externalWeight;
    in >> item.m_relevanceWeight;
    in >> item.m_sourceWeight;
    in >> item.m_updateWeight;
    in >> item.m_totalWeight;
    int is_d;
    in >> is_d;
    item.childRelType =(BaseChildRelation::ChildRelType)is_d;
    in >> is_d;
    item.childType =(BaseChildRelation::ChildType)is_d;
    in >> item.relationValue;
    QString buffer;
    in >> buffer;
    Q_ASSERT(buffer== "ItemDbRel");
    return in;
}

QString expandEnvironmentVars(QString txt);
QString adjustFilePath(QString path);

uint qHash(const CatItem& i);

QString intersect(QString first, QString second);

//#include "input_list.h"

#endif
