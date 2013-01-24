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


//NOTE!!: This file shared by main app and plugins!


#include <QProcess>
#include <QDebug>
#include <QLocale>
#include <QFileInfo>
#include <QMutex>
#include <iostream>

//#include <QtXmlPatterns>
//#include <QXmlQuery>
//#include <QXmlSimpleReader>


#include "item.h"
#include "itemWSHash.h"
//#include "input_list.h"
#include "list_item.h"
#include "inputLanguage.h"
#include "plugin_interface.h"

CatItem the_static_null_catItem;

using namespace std;

//All the routines shared between objects go HERE

extern const QChar beginEmbed(0x26ba);
extern const QChar endEmbed(0x27ba);

time_t gTime_offset_for_testing=0;
bool gThrottleUpdates=500;

//QString PluginInterface::ms_homeDir;
QChar* PluginInterface::ms_homeDirStr;
int PluginInterface::ms_homeDirLen;


QString contextualTimeString(QDateTime d){
    QDateTime now = QDateTime::currentDateTime();
    QString res = "now";
    QDate date = d.date();
    QTime time = d.time();
    qDebug() << "date part" << date;
    qDebug() << "time part" << time;
    if(d.secsTo(now) > (7*24*60*60*365)){
        //return d.toString(Qt::TextDate);
        res = d.toString("d-m h:ap yyyy");
    } else if(d.secsTo(now) > (7*24*60*60)){
        //return d.toString(Qt::TextDate);
        res = d.toString("d-m h:ap");
    } else if(d.secsTo(now) > (2*24*60*60)){
        res =d.toString("dddd h:ap");
    } else if(d.secsTo(now) > (24*60*60)){
        res =d.toString("'yesterday:'h':'ap");
    } else if(d.secsTo(now) > (60*60*1000)){
        res =d.toString("h':'ap");
    } else if(d.secsTo(now) > (60*1000)){
        int minutes = d.secsTo(now)/60;
        QString phrs = QString("$1 ").arg(minutes);
        res = phrs + MINUTES_AGO_PHRASE;
    }
    return res;
}


QString htmlize(QString str){
    return QString("<html><head></head><body>") + str + "</body></html>";
}

QString htmlizeFile(QString path){
    QString res;
    while(!path.isEmpty() && path.simplified().endsWith(ARG_SEPERATOR)){
        path.chop(1);
    }


    QFile f(path);
    if(f.open(QIODevice::ReadOnly)){
        if(f.size()>0){
            res.reserve(f.size()* (1.2));
        }

        while(!f.atEnd()){
            QByteArray line = f.readLine();
            QString processLine = Qt::escape(line);
            res.append(QString("<p>") + processLine + "</p>");
        }
        f.close();
    } else { res = FILE_DOESNT_EXIST_ERROR;}
    return htmlize(res);
}

QString htmlizeStringLines(QString longString){
    QString res;
    QStringList lines = longString.split("\n");
    if(longString.size()>0){
        res.reserve(longString.count()*(1.2) +6);
    }
    for(int i=0; i< lines.count(); i++){
        QString processLine = Qt::escape(lines[i]);
        res.append(QString("<p>") + processLine + "</p>");
    }
    return htmlize(res);
}

QString titlize(QString str){
    return QString("<html><head></head><body><p>") + str + "</p></body></html>";
}

QString htmlFontFormat(QString str, QString font_color_str){
    return QString("<font color=\"")+font_color_str+"\">" + str + "</font>";
}

QString htmlUnderlineFontFormat(QString str, QString font_color_str){
    return QString("<font color=\"")+font_color_str+"\"><u>" + str + "</u></font>";
}

QString htmlFontSizeFormat(QString str, int size){
    return QString("<font size=\"")+QString::number(size)+"px\">" + str + "</font>";
}

QString makeBold(QString str){
    return (QString("<b>") + str + "</b>");
}

QString makeItallic(QString str){
    return (QString("<i>") + str + "</i>");
}

QString expandEnvironmentVars(QString txt)
{
    QStringList list = QProcess::systemEnvironment();
    txt.replace('~', "$HOME$");
    QString delim("$");
    QString out = "";
    int curPos = txt.indexOf(delim, 0);
    if (curPos == -1) return txt;

    while(curPos != -1) {
        int nextPos = txt.indexOf("$", curPos+1);
        if (nextPos == -1) {
            out += txt.mid(curPos+1);
            break;
        }
        QString var = txt.mid(curPos+1, nextPos-curPos-1);
        bool found = false;
        foreach(QString s, list) {
            if (s.startsWith(var, Qt::CaseInsensitive)) {
                found = true;
                out += s.mid(var.length()+1);
                break;
            }
        }
        if (!found)
            out += "$" + var;
        curPos = nextPos;
    }
    return out;
}

//The usually smart Qt doesn't get the ~ character
QString adjustFilePath(QString path){
    if(path[0]=='~'){
        return QDir::toNativeSeparators(QDir::homePath() + path.mid(1));
    }
    return QDir::toNativeSeparators(path);
}

uint stringHash(QString str){
    return qHash(str);
}
long long stringHashHash(QHash<QString, QString> strHash){
    return stringHash(picklHashToEscdString(strHash));
}

//long long stringHashHash(QHash<QString, QString> strHash){
//    if(strHash.size()==0){return 0;}
//
//    QHash<QString, QString>::iterator i = strHash.begin();
//    const int offset((sizeof(h))/2);
//
//    long long h = (qHash(i.value())+100) << (offset);
//    for(; i++;i!=strHash.end()){
//        h ^= (((long long)qHash(strHash.value()) << i));
//        h ^= (((long long)qHash(strHash.key()) << i+offset));
//    }
//    return h;
//}

uint qHash(const CatItem& i) {
    return stringHash(i.getPath());
}

QString intersect(QString first, QString second){
    for(int i=0; i< first.count() && i<second.count(); i++){
        if(first[i]!=second[i]){ return first.left(i); }
    }
    return QString();
}

bool isStandardSymbol(QChar k){
    char theChar = k.toAscii();
    switch(theChar){
        case '@':
        case '!':
        case '#':
        case '~':
        case '`':
        case '$':
        case '%':
        case '^':
        case '&':
        case '*':
        case '(':
        case ')':
        case '_':
        case '-':
        case '+':
        case '=':
        case '{':
        case '}':
        case '[':
        case ']':
        case '|':
        case '\\':
        case '/':
        case ':':
        case ';':
        case '"':
        case '\'':
        case '<':
        case '>':
        case '.':
        case ',':
        case '?':
            return true;
        default:
            return false;
    }

}

bool filterItemByKeys(CatItem& item, QString userStr, QList<int>* outIndex, int char_skips) {
    
    QList<int> index;
    if(userStr.length()>0 && userStr.length()<=2){
        return (item.getName().length()>0 && item.getName().startsWith(userStr,Qt::CaseInsensitive));
    }
    int match_l = consonant_match(item, userStr, &index, char_skips);
    item.setMatchIndex(index);
    if(outIndex){*outIndex = index;}
    //Require full match for short strings, partial match for longer strings
    int matchThresh =  MIN(userStr.length(), 2+(userStr.length()*0.6));
    return (match_l>=matchThresh);
}


//Returns the number of matched letters. Don't use this *DIRECTLY* to decide whether keys match an item
int consonant_match(CatItem & item, QString userStr, QList<int>* outIndex, int max_char_skips )
{

    //So far i_User, the user-string-index, is a good
    //match to the match level, that may change...
    QString itemStr = item.getLowName();
    item.setMatchType(CatItem::USER_KEYS);
    Q_ASSERT(item.getMatchType()==CatItem::USER_KEYS);
    QList<int> matchIndex;
    int i_User=0;
    int j_Item= 0;
    int char_skips=0;
    bool char_skipping = false;

    int consonant_count=0;
    for(int i=0;i< userStr.length();i++){
        if( isConsonant(userStr[i_User] )){ consonant_count++;}
    }
    bool consonant_skipping = consonant_count>2 && (consonant_count> (userStr.length()/2));

    while(true) {
        while (itemStr[j_Item] == userStr[i_User].toLower()){
            matchIndex.append(j_Item);
            j_Item++;
            i_User++;
            if (i_User > userStr.length() || j_Item> itemStr.length()) {
                if(outIndex){ *outIndex = matchIndex; }
                return i_User;
            }
        }
        //We just care about matching conconants
        while (consonant_skipping &&
               (!isConsonant(itemStr[j_Item])  || !isConsonant(userStr[i_User] ))){
            if (!isConsonant(userStr[i_User] )){
                i_User++;
            }
            if (!isConsonant(itemStr[j_Item] )){
                j_Item++;
                //Limit # of times we skip a range of char's
                if(!char_skipping){
                    char_skips++;
                    if(char_skips > max_char_skips){
                        if(outIndex){ *outIndex = matchIndex; }
                        return i_User;
                    }
                    char_skipping = true;
                }
            }
            if (i_User > userStr.length() || j_Item> itemStr.length()){
                if(outIndex){ *outIndex = matchIndex; }
                return i_User;
            }
        }
        while (itemStr[j_Item] != userStr[i_User].toLower()){
            //Here's where the match is incomplete but we don't care (YET!)
            // Remember, this problem IN GENERAL is hard...
            j_Item++;
            if (j_Item> itemStr.count()){
                if(outIndex){ *outIndex = matchIndex; }
                return i_User;
            }
        }
        char_skipping = false;
    }
    if(outIndex){ *outIndex = matchIndex; }
    return i_User;
}




bool userkey_match(CatItem & item, QString userStr, QList<int>* outIndex){
    Q_ASSERT(outIndex);
    consonant_match(item, userStr, outIndex);
    if(userStr.length() <= outIndex->length()){
        //Q_ASSERT(userStr.length() == outIndex->length());
        item.setMatchIndex(*outIndex);
        return true;
    }
    return false;


}

//
bool isConsonant(QChar v){
    if (!v.isLetter())
        {return false;}

    v = v.toLower();
    switch (v.toAscii())
    {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'y':
            return false;
        default:
            break;
    }
    return true;
}



bool matches(CatItem* item, QString txt)  {
    int txt_leng = txt.length();
    int match_level = consonant_match(*item,txt);
    return (txt_leng <= match_level);
}



bool timeLessThan(CatItem* it1, CatItem* it2){

    return it1->getCreationTime() < it2->getCreationTime();

}

struct OrderedItem  {

    QString m_label;
    ListItem m_item;
    int m_oldOrder;
    bool m_asKeystrokes;

    OrderedItem(ListItem i, QString l, int o, bool asKey=false){
        m_item = i;
        m_label =l;
        m_oldOrder =o;
        m_asKeystrokes =asKey;
    }

    OrderedItem(const OrderedItem& o){
        m_item = o.m_item;
        m_label =o.m_label;
        m_oldOrder = o.m_oldOrder;
        m_asKeystrokes = o.m_asKeystrokes;
    }

    bool operator < (OrderedItem oi) const{

        if(m_asKeystrokes){

            CatItem otherItem = oi.m_item;
            CatItem theItem = m_item;
            if(matches(&theItem, m_label) &&
               !matches(&(otherItem), m_label)){
               return false;
            }
            if(matches(&(otherItem), m_label)
                && !matches(&theItem, m_label)){
                return true;
            }

        } else{
            int v1 = m_item.getCustomValue(m_label, -1);
            int v2 = oi.m_item.getCustomValue(m_label, -1);
            if(v1==0 && v2==0){
                QString s1 = m_item.getCustomString(m_label);
                QString s2 = oi.m_item.getCustomString(m_label);
                if(s1 < s2) return true;
            }

            //REVERSE ORDER
            if(v1  > v2) return true;
            if(v1  < v2) return false;
        }
        return m_oldOrder < oi.m_oldOrder;
    }
};

QList<ListItem> sortListItems(QList<ListItem> il, QString label, bool asKey){
    QMap< OrderedItem,  OrderedItem> keyOrderedMap;
    for(int i=0; i< il.count(); i++){
        OrderedItem oi(il[i], label, i, asKey);
        keyOrderedMap.insert(oi, oi);
    }
    QList<OrderedItem> oil = keyOrderedMap.keys();
    QList<ListItem> res;
    for(int i=0; i< oil.count(); i++){
        res.append(oil[i].m_item);
    }
    return res;
}

QList<CatItem> sortCatItems(QList<CatItem> il, QString label, bool asKey){
    QList<ListItem> res;
    Q_ASSERT(false);
//    for(int i=0; i< il.count(); i++){
//        OrderedItem oi(il[i], label, i, asKey);
//        keyOrderedMap.insert(oi, oi);
//    }

    for(int i=0; i<il.length();i++){
        ListItem li(il[i]);
        res.append(li);
    }
    res = sortListItems(res, label, asKey);
    QList<CatItem> res2;
    for(int i=0; i<res.length();i++){
        res2.append(res[i]);
    }
    return res2;
}

QString CatItem::getAltDescription() {
    if(this->hasLabel(TEMPORARY_DESCRIPTION_KEY_STR))
        return getCustomString(TEMPORARY_DESCRIPTION_KEY_STR);
    if(this->hasLabel(ALT_DESCRIPTION_KEY_STR))
        return getCustomString(ALT_DESCRIPTION_KEY_STR);
    if(this->hasLabel(DESCRIPTION_KEY_STR))
        return getCustomString(ALT_DESCRIPTION_KEY_STR);

    QString res = getActionParentType();

    if(this->hasLabel(FILE_CATALOG_PLUGIN_STR)){
        res += QString(" ");
        long size = getCharacteristicValue(FILE_SIZE_CHARACTERISTIC_KEY);
        if(size < 1000){
            res += QString::number(size) + " bits ";
        } else  if(size < 1000*1000){
            res += QString::number(size/1000) + " k ";
        } else{
            res += QString::number(size/(1000*1000)) + " mb ";
        }

        QString mimeType = getMimeDescription();
        mimeType.truncate(20);
        if(!mimeType.isEmpty()){
            res += QString(" ") + mimeType ;
        }

        time_t t;
        QDateTime d;
        if(hasLabel(MODIFICATION_TIME_KEY)){
            t = getModificationTime();
        } else {
            t = getCreationTime();
        }
        d = QDateTime::fromTime_t(t);
        Q_ASSERT(d.isValid());
        qDebug() << "datetime: " << d;
        res += (QString(" ") + contextualTimeString(d));
        return res;
    }
    return "";
}


QString CatItem::getActionName() const{
    QString dd = getCustomString(TEMPORARY_NAME_KEY_STR);
    if(!dd.isEmpty()){ return dd; }
    dd = getName();
    if(!dd.isEmpty()){ return dd; }
    return d->m_path;
}

QString CatItem::getArgPath() const{
    QString dd = getCustomString(ARG_PATH_KEY_STR);
    if(!dd.isEmpty()){ return dd; }
    if(d->m_path==RUN_PSEUDO_ITEM_PATH){return "";}
    return d->m_path;
}

void CatItem::setName(QString rawName){

    const QString codedSpace("%20");
    const QString codedAt("%40");

    QString v;
    if(!(rawName.contains(codedSpace)||
         rawName.contains(codedAt))){
        v = rawName;
    } else {
        for(int i=0; i<rawName.length();i++){
            if(rawName[i]=='%'){
                if(rawName.mid(i,3)==codedSpace){
                    v[i]=' ';
                    i+=2;
                }
                if(rawName.mid(i,3)==codedAt){
                    v[i]=' ';
                    i+=2;
                }
            } else {
                v[i] = rawName[i];
            }
        }
    }

    if(v.isEmpty()){
        d->m_data.remove(NAME_KEY_STR);
        v = this->getName();
    } else {
        d->m_data[NAME_KEY_STR] = v;
    }
    d->m_name = v;
    d->m_lowName = d->m_name.toLower();
    if(d->m_data.contains(TEMP_LOW_PATH_KEY_STR)){
        d->m_data.remove(TEMP_LOW_PATH_KEY_STR);
    }
}

void CatItem::setPath(QString rawPath){
    Q_ASSERT(!rawPath.simplified().endsWith(ARG_SEPERATOR));
    Q_ASSERT(!rawPath.isEmpty());
    QString p;

    //A trailing slash seems to be alway serpufluous
    if(rawPath.size() >1 && rawPath.endsWith(CHAR_PATH_SEP)){
        p = rawPath.left(rawPath.size()-1);
    }  else if(!rawPath.startsWith(FILE_PREFIX)){
        p = rawPath;
    } else {
        int toTake = FILE_PREFIX.length()-1;
        while(rawPath.length()>toTake+1
                && rawPath[toTake]=='/'
                && rawPath[toTake+1]=='/' && (toTake<=0 ||(rawPath[toTake-1]!=':'))){
            toTake++;
        }
        rawPath = rawPath.mid(toTake);
        const QString codedSpace("%20");
        const QString codedAt("%40");
        int i=0;

        if(rawPath.contains('%')){
            while(i<rawPath.length()){
                if(rawPath[i]=='%'){
                    if(rawPath.mid(i,3)==codedSpace){
                        p.append(' ');
                        i+=3;
                        continue;
                    }
                    if(rawPath.mid(i,3)==codedAt){
                        p.append('@');
                        i+=3;
                        continue;
                    }
                }
                p.append(rawPath[i]);
                i++;
            }
        }
    }
    p = p.simplified();;
    adjustChildrensPath(p);

    d->m_path = p;
    if(d->m_name.isEmpty()){
        //Need to "cache" toLower result
        QStringList sl = d->m_path.split("/");
        if(sl.count()>0 && !sl[sl.count()-1].isEmpty()){
            d->m_data[TEMP_LOW_PATH_KEY_STR] = sl[sl.count()-1].toLower();
        } else if(sl.count()>1 && !sl[sl.count()-2].isEmpty()){
            d->m_data[TEMP_LOW_PATH_KEY_STR] = sl[sl.count()-2].toLower();
        } else {
            d->m_data[TEMP_LOW_PATH_KEY_STR] = p.toLower();
        }
    }
}

QString CatItem::itemCacheDir(){
    QString cachePath = QDir::convertSeparators(
            PUSER_APP_DIR
            + FEED_CACHE_DIR
        );
    return cachePath;
}


QList<CatItem> CatItem::getCustomArguments(){
    VerbItem vi(*this);
    QList<CatItem> res;
    if(vi.isCustomVerb()){
        res = vi.getArguments();
    }
    return res;
}

QList<CatItem> CatItem::getRequiredCustomArguments(){
    QList<CatItem> res;
    VerbItem vi(*this);
    if(vi.isCustomVerb()){
        res = vi.getRequiredArguments();
    }
    return res;
}

void CatItem::addArgumentsToRelations(){
    QList<CatItem> res;
    VerbItem vi(*this);
    if(vi.isCustomVerb()){
        res = vi.getActionTypeSignature();
    }
    for(int i=0; i < res.count();i++){
        addParent(res[i], "",BaseChildRelation::OPTIONAL_ACTION_CHILD);
    }
}

CatItem::CatItem(QString templateName, QVariantHash vh, QString prefix){
    d = new InnerItem();

    QFileInfo fileInfo = QApplication ::applicationDirPath();
    QSettings s(fileInfo.absoluteFilePath()+ ITEM_TEMPLATE_DIR + templateName, QSettings::IniFormat);
    QStringList sl = s.childKeys();
    for(int i=0;i < sl.count();i++){
        QString key = prefix + sl[i];
        if(!vh.contains(key)){continue;}

        if(sl[i].startsWith(TEMPLATE_PARENT_STR)){
            CatItem p(sl[i], vh, vh[sl[i]].toString() + prefix);
            addParent(p);
        } else if(sl[i].startsWith(TEMPLATE_CHILD_STR)){
            CatItem c(sl[i], vh, vh[sl[i]].toString() + prefix);
            addChild(c);
        } else if(sl[i].endsWith(TEMPLATE_INT_STR)){
            this->setCustomPluginValue(sl[i],vh[key].toInt());
        } else {
            this->getCustomString(sl[i],vh[key].toString());
        }
    }
}



CatItem CatItem::createFileItem(QSet<QString> &extendedTypes, QFileInfo initialInfo,
                                          CatItem& folderParentItem, int depth,
                                          UserEvent::LoadType lt,
                                          PlatformBase* platform,
                                          CatItem pluginRep) {

    QFileInfo fileInfo = initialInfo;
    bool isLink = false;
    if(initialInfo.symLinkTarget().length()!=0) {
        QFileInfo lkInfo(initialInfo.symLinkTarget());
        fileInfo = lkInfo;
        isLink = true;
    }

    //We'll get all the mime stuff with a different call...
    QString shortName;
    QString fullPath;
    QString suffix = fileInfo.suffix();
    fullPath = fileInfo.absoluteFilePath();
    if(fileInfo.isDir()){
        shortName = fileInfo.baseName();
        if(shortName.isEmpty()){
            QStringList pathParts = fullPath.split(QDir::separator());
            QDir d = fileInfo.absoluteDir();
            shortName = d.dirName();
            if(shortName.isEmpty()){
                if(!pathParts.last().isEmpty()){
                    shortName = pathParts.last();
                } else if(pathParts.length() > 1){
                    shortName = pathParts[pathParts.length()-1];
                }
            }
        }
    } else if(suffix == ONELINE_FILE_EXTENSION){
        QSettings s(fullPath, QSettings::IniFormat);
        CatItem i(&s,pluginRep);
        return i;
    } else {
        shortName = fileInfo.fileName();
    }

    CatItem typeParent;
    if(!fileInfo.suffix().isEmpty()){
        typeParent = CatItem(
                        addPrefix(ACTION_TYPE_PREFIX,
                                  QString(fileInfo.suffix())) );
    } else {
        typeParent = pluginRep;
    }

    CatItem it(fullPath, shortName,HASH_FILE_CATALOG);
    if (fullPath.endsWith("~") ||
        suffix.endsWith("~") ||
        fullPath.endsWith("_files") ||
        fileInfo.isHidden() || folderParentItem.getIsHidden()) {
        it.setHidden();
    }

    it.addCharacteristic(FILE_SIZE_CHARACTERISTIC_KEY, fileInfo.size());

    if(fileInfo.isDir()){
        it.setItemType(CatItem::LOCAL_DATA_FOLDER);
        if(isLink){
            it.setCustomPluginValue(REPLACE_CHILDREN_TEMP_KEY,1);
        }
    }

    it.setCreationTime(fileInfo.created().toTime_t());
    it.setModificationTime(fileInfo.lastModified().toTime_t());

    long guessedRawTime = MAX(fileInfo.lastModified().toTime_t(),
                        abs((long long)(fileInfo.lastRead().toTime_t() - (ONE_DAY_IN_SECONDS))));

    long guessedRelativeTime = MAX( 0, appGlobalTime() - guessedRawTime);

    qint32 guessedWeight = MAX(1,(MEDIUM_EXTERNAL_WEIGHT/(depth)) -log2(depth+guessedRelativeTime))
                           *MEDIUM_EXTERNAL_WEIGHT;

    //We might only do this only for recent additions...
    CatItem mimeParent = platform->alterItem(&it);

    if(!mimeParent.isEmpty()){
        if(mimeParent.getPath() == addPrefix(ACTION_TYPE_PREFIX,
                                             QString(MIME_EXECUTABLE))){
            it.setItemType(CatItem::VERB);
        }
        if(mimeParent.getPath() == addPrefix(
                                    ACTION_TYPE_PREFIX,QString(MIME_SCRIPT))){
            it.setItemType(CatItem::VERB);
        }
        typeParent = mimeParent;
        QString typeString = typeParent.getMimeType();
        if(typeString.isEmpty()){
            typeString = typeParent.getName();
        }

        if(!extendedTypes.contains(typeString) || ((int)lt) > (int)UserEvent::BECOMING_VISIBLE){
            platform->alterItem(&typeParent);
            extendedTypes.insert(typeString);
        } else {
            typeParent.setStub(true);
        }
        it.addParent(typeParent);
    }

    it.setExternalWeight(guessedWeight,typeParent);
    it.setLabel(FILE_CATALOG_PLUGIN_STR);

    if(!folderParentItem.isEmpty()){
        it.setExternalWeight(guessedWeight,folderParentItem);
        it.setChildStubbed(false);
        folderParentItem.setChildStubbed(false);

        CatItem parentCopy = folderParentItem;
        parentCopy.setStub();
        parentCopy.clearRelations();
        Q_ASSERT(parentCopy.getChildren().count()==0);
        //parentCopy.addChild(item);
        it.addParent(parentCopy,FILE_CATALOG_PLUGIN_STR);
        Q_ASSERT(it.hasParentType(FILE_CATALOG_PLUGIN_STR));
        //parent added "in bulk", elsewhere
//        if((int)lt < (int)UserEvent::SEEN){
//        } else {
//            it.addParent(folderParentItem,FILE_CATALOG_PLUGIN_STR);
//        }
    }
    return it;
}

CatItem CatItem::createTypeParent(CatItem::ItemType type){
    Q_ASSERT(type >= (int)CatItem::MIN_TYPE &&
             type <= CatItem::MAX_TYPE);

    QString name;
    QString icon;
    switch(type){
        case MIN_TYPE:
            name = "Everything";
            icon = FILE_TYPE_ICON;
            break;
        case VERB:
            name = VERB_NAME;
            icon = UI_NEXTACTION_PLACEHOLDER_ICON;
            break;
        case TAG:
            name = CATEGORY_NAME;
            icon = TAG_TYPE_ICON;
            break;
        case OPERATION:
            name = OPERATIONS_NAME;
            icon = FILE_TYPE_ICON;
            break;
        case LOCAL_DATA_DOCUMENT:
            name = FILES_NAME;
            icon = FILE_TYPE_ICON;
            break;
        case LOCAL:
            name = LOCAL_NAME;
            icon = FILE_TYPE_ICON;
            break;
        case LOCAL_DATA_FOLDER:
            name = FOLDERS_NAME;
            icon = FILE_TYPE_ICON;
            break;
        case ACTION_TYPE:
            name = "Mime Types";
            icon = TAG_TYPE_ICON;
            break;
        case ORGANIZING_TYPE:
            name = "Types";
            icon = TAG_TYPE_ICON;
            break;
        case PLUGIN_TYPE:
            name = "Plugins";
            icon = TAG_TYPE_ICON;
            break;
        case PUBLIC_DOCUMENT:
            name = WEBPAGE_NAME;
            icon = SITE_TYPE_ICON;
            break;
        case PUBLIC_FEED:
            name = FEED_NAME;
            icon = FEED_TYPE_ICON;
            break;
        case PUBLIC_POST:
            name = NEW_POSTS_NAME;
            icon = FEED_TYPE_ICON;
            break;
        case PERSON:
            name = PEOPLE_NAME;
            icon = PERSON_TYPE_ICON;
            break;
        case MESSAGE:
            name = MESSAGES_NAME;
            icon = MESSAGE_TYPE_ICON;
            break;
        case ALERT:
            name = "Alerts";
            icon = FILE_TYPE_ICON;
            break;
        case VALUE:
            name = "Values";
            icon = FILE_TYPE_ICON;
            break;
        case MAX_TYPE:
            name = "Shouldn't Appear";
            icon = FILE_TYPE_ICON;
            break;
    }

    QString path = addPrefix(TYPE_PREFIX,QString::number(type));
    CatItem res(path, name);
    res.setItemType(CatItem::ORGANIZING_TYPE);
    res.setOrganizingType(type);
    res.setLabel(TYPE_PARENT_KEY);
    res.setSourceWeight(MEDIUM_EXTERNAL_WEIGHT);
    res.setTagLevel(CatItem::INTERNAL_SOURCE);
    res.setFilterRole(CatItem::CATEGORY_FILTER);
    res.setCustomValue(CLOSABLE_ORGANIZING_SOURCE_KEY,0);
    res.setStub(true);
    return res;

}





void CatItem::clearExternalWeight(){
    for(int i=0;i<d->children_detached_list.count();i++){
        d->children_detached_list[i].setExternalWeight(0);
        if(d->children_detached_list[i].getChildType() == BaseChildRelation::WEIGHT_SOURCE){
            d->children_detached_list[i].setChildType(BaseChildRelation::UNDETERMINED);
        }
    }
}

int CatItem::getChildCount() const {
    int count = getCustomValue(CHILD_COUNT_STUBBED_KEY,(int)-1);
    //Q_ASSERT(count==-1 || isChildStubbed() || count<= this->getChildRelationCount());
    if(count==-1){
        count = 0;
        long id = getItemId();
        for(int i=0;i<d->children_detached_list.count();i++){
            if(d->children_detached_list[i].getParentId()==id){
                count++;
            }
        }
    }
    //setChildCount(count);
    return count;
}


CatItem CatItem::getUpdateSourceParent(qint32 w){
    CatItem par;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() &&
           (d->children_detached_list[i].getChildType() == (BaseChildRelation::WEIGHT_SOURCE))){
            if(d->children_detached_list[i].getSourceWeight() >= w){
                w = d->children_detached_list[i].getSourceWeight();
                par = d->children_detached_list[i].getParent();
            }
        }
    }
    //Q_ASSERT(!par.isEmpty());
    return par;
}

QList<CatItem> CatItem::getSearchSourceParents(){
    QList<CatItem> res;

    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() ){
            CatItem possiblePar = d->children_detached_list[i].getParent();
            if(possiblePar.isCategorizingSource()){
                if(possiblePar.getItemType() == CatItem::LOCAL_DATA_FOLDER
                   || possiblePar.getItemType() == CatItem::PUBLIC_DOCUMENT
                   || possiblePar.getItemType() == CatItem::PERSON ){
                    res.push_front(possiblePar);
                } else {
                    res.push_back(possiblePar);
                }
            }
        }
    }
    if(isCategorizingSource()){
        res.push_front(*this);
    }
    if(res.count()>0){ return res;}

    int w=0;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() ){
            CatItem possiblePar = d->children_detached_list[i].getParent();
            int weight;
            if(possiblePar.isASource()){
                weight = possiblePar.getSourceWeight();
                if(weight>w){
                    w = weight;
                    res.append(possiblePar);
                }
            }
        }
    }
    return res;
}

QList<CatItem> CatItem::getSourceParents(){
    QList<CatItem> parents;

    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() &&
           ((d->children_detached_list[i].getChildType() == (BaseChildRelation::WEIGHT_SOURCE)
            || d->children_detached_list[i].getParent().isCategorizingSource()))){
            parents.append(d->children_detached_list[i].getParent());
        }
    }
    return parents;
}

qint32 CatItem::getWeightFromSources(){

    qint32 res = 0;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() &&
           (d->children_detached_list[i].getChildType() == (BaseChildRelation::WEIGHT_SOURCE))){
            res = MAX(res,d->children_detached_list[i].getSourceWeight());
        }
    }
    return res;
}

void CatItem::setSourceWeight(qint32 weight, CatItem par){
    Q_ASSERT(!par.getPath().isEmpty());
    QString parentPath = par.getPath();
    bool merged = false;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getParentPath()==parentPath
                || (par.isEmpty()&& d->children_detached_list[i].getSourceWeight()>=0) ){
            d->children_detached_list[i].setSourceWeight(weight);
            d->children_detached_list[i].setChildType(BaseChildRelation::WEIGHT_SOURCE);;
            merged = true;
        }
    }
    if(!merged && !par.isEmpty()){
        DetachedChildRelation dcr(par,*this);
        dcr.setChildType(BaseChildRelation::WEIGHT_SOURCE);
        dcr.setSourceWeight(weight);
        Q_ASSERT(dcr.getSourceWeight() == weight);
        d->children_detached_list.append(dcr);
        Q_ASSERT(d->children_detached_list[d->children_detached_list.count()-1].getSourceWeight() == weight);
    }
    qint32 maxW = getWeightFromSources();
    setCustomPluginValue(SOURCE_WEIGHT_KEY_STR,maxW);
    Q_ASSERT(maxW >= weight);
    Q_ASSERT(isASource());
}

qint32 CatItem::getUpdatingSourceWeight(){

    qint32 res = 0;
    if(isUpdatableSource()){
        for(int i=0;i<d->children_detached_list.count();i++){
            if(d->children_detached_list[i].getChildPath() == getPath() &&
               (d->children_detached_list[i].getChildType() == (BaseChildRelation::WEIGHT_SOURCE))){
                res = MAX(res,d->children_detached_list[i].getUpdateWeight());
            }
        }
    }
    return res;
}

void CatItem::setUpdatingSourceWeight(qint32 weight, CatItem par){
    if(weight>0){ setIsUpdatableSource(true); }
    Q_ASSERT(!par.getPath().isEmpty());
    QString parentPath = par.getPath();
    bool merged = false;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getParentPath()==parentPath
           || (par.isEmpty()&& d->children_detached_list[i].getSourceWeight()>=0) ){
            d->children_detached_list[i].setUpdateWeight(weight);
            d->children_detached_list[i].setChildType(BaseChildRelation::WEIGHT_SOURCE);;
            merged = true;
        }
    }
    if(!merged && !par.isEmpty()){
        DetachedChildRelation dcr(par,*this);
        dcr.setChildType(BaseChildRelation::WEIGHT_SOURCE);
        dcr.setUpdateWeight(weight);
        d->children_detached_list.append(dcr);
        Q_ASSERT(d->children_detached_list[d->children_detached_list.count()-1].getUpdateWeight() >= weight);
    }
    Q_ASSERT(getUpdatingSourceWeight() >= weight);
}



long CatItem::getExternalWeight(qint32 parentId){

    if(parentId==0){
        return getTotalExternalWeight();
    }
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getParentId()==parentId&&
           !d->children_detached_list[i].isForDelete()){
            return d->children_detached_list[i].getExternalWeight();
        }
    }
    return MEDIUM_EXTERNAL_WEIGHT;
}

CatItem CatItem::getWeightParent(){
    long w = this->getCustomValue(EXTERNAL_WEIGHT_KEY_STR, -1);
    CatItem maxParent;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(!d->children_detached_list[i].getExternalWeight() !=MEDIUM_EXTERNAL_WEIGHT&&
           !d->children_detached_list[i].isForDelete()){
            if(d->children_detached_list[i].getExternalWeight() >w){
                maxParent = d->children_detached_list[i].getParent();
                w = d->children_detached_list[i].getExternalWeight();
            }
        }
    }
    return maxParent;
}


long CatItem::getTotalExternalWeight(){
    long w = this->getCustomValue(EXTERNAL_WEIGHT_KEY_STR, -1);
    for(int i=0;i<d->children_detached_list.count();i++){
        if(!d->children_detached_list[i].getExternalWeight() !=MEDIUM_EXTERNAL_WEIGHT&&
           !d->children_detached_list[i].isForDelete()){
            w = MAX(d->children_detached_list[i].getExternalWeight(), w);
        }
    }
    if(w>-1){
        return w;
    } else {
        return MEDIUM_EXTERNAL_WEIGHT;
    }
}

void CatItem::setExternalWeight(long weight, CatItem par){
    Q_ASSERT(!par.getPath().isEmpty());
//    if(par.getPath().isEmpty()){
//        if(getPluginId()==0){
//            setCustomPluginValue(EXTERNAL_WEIGHT_KEY_STR, weight);
//            return;
//        }
//    }
    QString parentPath = par.getPath();
    bool merged = false;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getParentPath()==parentPath){
            d->children_detached_list[i].setExternalWeight(weight);
            d->children_detached_list[i].setChildType(BaseChildRelation::WEIGHT_SOURCE);;
            merged = true;
        }
        if(d->children_detached_list[i].getChildPath()==parentPath){
            d->children_detached_list[i].setExternalWeight(weight);
            d->children_detached_list[i].setChildType(BaseChildRelation::REVERSE_WEIGHT_SOURCE);
            merged = true;
            Q_ASSERT(false);
        }
    }
    if(!merged){
        DetachedChildRelation dcr(par,*this);
        dcr.setExternalWeight(weight);
        Q_ASSERT(dcr.getExternalWeight() == weight);
        d->children_detached_list.append(dcr);
        Q_ASSERT(d->children_detached_list[d->children_detached_list.count()-1].getExternalWeight() == weight);
        //Q_ASSERT(d->children_detached_list[d->children_detached_list.count()-1].getParentId() == parentId);
    }

    Q_ASSERT(getExternalWeight(par.getItemId()) == weight);
    Q_ASSERT(getTotalExternalWeight() >= weight);
}

void CatItem::clearRelations() {
    long w = getExternalWeight();
    setCustomValue(EXTERNAL_WEIGHT_KEY_STR,w);
    d->children_detached_list.clear();
    d->m_matchIndex.clear();
    d->variantData.clear();
}

bool CatItem::matchOrganizingTypes( ItemType organizingType, ItemType itemType){
    if(organizingType==CatItem::MIN_TYPE){ return true;}
    QList<ItemType> types = getOrganizingTypeList(organizingType);
    for(int i=0; i<types.count();i++){
        if(types[i]==itemType){
            return true;
        }
    }
    return false;
}



QString CatItem::getParentPath() const {
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath() &&
           !d->children_detached_list[i].isForDelete()){
            return d->children_detached_list[i].getParentPath();
        }
    }
    return QString();
}


//
CatItem& CatItem::addChild(CatItem& childItem, QString pluginName,
                       BaseChildRelation::ChildRelType isDefault){
    Q_ASSERT(childItem.getPath() != getPath());
    Q_ASSERT(!childItem.isEmpty());
    setChildStubbed(false);
    addParentInternal(*this, childItem, pluginName,isDefault);
    return addChildInternal(*this, childItem, pluginName,isDefault);
}

CatItem& CatItem::addParent(CatItem& parentItem, QString pluginName,
                        BaseChildRelation::ChildRelType childRelType,
                        bool isDefaultAppType){
    Q_ASSERT(!parentItem.isEmpty());
    if(parentItem.getItemType() == CatItem::PERSON){
        setCustomPluginInfo(AUTHOR_KEY_STR,parentItem.getName());
    }
    if(childRelType == BaseChildRelation::ACCOUNT_SOURCE_PARENT){
        setAccountItem(parentItem.getPath());
    }
    Q_ASSERT(parentItem.getPath() != getPath());
    addChildInternal(parentItem, *this, pluginName,childRelType, isDefaultAppType);
    return addParentInternal(parentItem, *this, pluginName,childRelType, isDefaultAppType);
}


CatItem& CatItem::addChildInternal(CatItem& parentItem, CatItem& childItem, QString pluginName,
                               BaseChildRelation::ChildRelType relType,
                               bool isDefaultAppType){
    Q_ASSERT(!parentItem.getPath().isEmpty());

//Uncomment if you want to see everything that happens
//    qDebug() << "adding child: " << childItem.getPath() << " to parent: " << parentItem.getPath();


    DetachedChildRelation dcr(parentItem,childItem, pluginName);
    dcr.setChildRelType(relType);
    dcr.setIsDefaultActionParent(isDefaultAppType);

    bool merged = false;
    int insertIndex = parentItem.d->children_detached_list.count();

    for(int i=0;i < parentItem.d->children_detached_list.count();i++){
        if(parentItem.d->children_detached_list[i].getChild()==childItem){

            CatItem toMerge= parentItem.d->children_detached_list[i].getChild();
            toMerge.merge(childItem);
            parentItem.d->children_detached_list[i].setChild(toMerge);
            parentItem.d->children_detached_list[i].mergeChildRelType(relType);
            parentItem.d->children_detached_list[i].setForDelete(false);
            if(isDefaultAppType){
                parentItem.d->children_detached_list[i].setIsDefaultActionParent(true);
            }
            if(childItem.getSiblingOrder()!=-1){
                parentItem.d->children_detached_list[i].setSiblingOrder(
                        childItem.getSiblingOrder());
            }
            merged = true;
            insertIndex = i;
            break;
        }
        //Q_ASSERT((parentItem.d->children_detached_list[i].getParentPath()!=childItem.getPath()));

    }
    if(!merged){
        parentItem.d->children_detached_list.append(dcr);
    }
    Q_ASSERT(parentItem.hasChild(childItem));
    return parentItem.d->children_detached_list[insertIndex].getChildRef();
}

CatItem& CatItem::addParentInternal(CatItem& parentItem, CatItem& childItem,
        QString pluginName, BaseChildRelation::ChildRelType relType,
        bool isDefaultAppType){
    Q_ASSERT(!parentItem.getPath().isEmpty());

    DetachedChildRelation dcr(parentItem,childItem, pluginName);
    dcr.setChildRelType(relType);
    dcr.setIsDefaultActionParent(isDefaultAppType);

    bool merged = false;
    int insertIndex = childItem.d->children_detached_list.count();

    for(int i=0;i < childItem.d->children_detached_list.count();i++){
        if(childItem.d->children_detached_list[i].getParent()==parentItem){
            CatItem toMerge= childItem.d->children_detached_list[i].getParent();
            toMerge.merge(parentItem);
            childItem.d->children_detached_list[i].setParent(toMerge);
            childItem.d->children_detached_list[i].mergeChildRelType(relType);
            childItem.d->children_detached_list[i].setForDelete(false);
            if(isDefaultAppType){
                childItem.d->children_detached_list[i].setIsDefaultActionParent(true);
            }
            merged = true;
            insertIndex = i;
            break;
        }
        //Q_ASSERT((parentItem.d->children_detached_list[i].getChildPath()!=childItem.getPath()));
    }
    if(!merged){
        childItem.d->children_detached_list.append(dcr);
    }
    return childItem.d->children_detached_list[insertIndex].getParentRef();
}

void CatItem::addChildren(QList<CatItem> children){

    for(int i=0; i<children.count(); i++){
        if(children[i].getPath() !=this->getPath()){
            addChild(children[i]);
        }
    }
}

//We happen to know the child
void CatItem::addChildrenBulk(QList<CatItem> children){

    QHash<QString, DetachedChildRelation> relations;
    for(int i=0;i < d->children_detached_list.count();i++){
        DetachedChildRelation dcr = d->children_detached_list[i];
        relations[dcr.toString()] = dcr;
    }
    for(int i=0; !(i==children.size());i++){
        CatItem& child = children[i];
        //We're assume child has no parent copies...
        DetachedChildRelation dcr(*this, child,"");
        child.d->children_detached_list.append(dcr);
        relations[dcr.toString()] = dcr;
    }
    d->children_detached_list = relations.values();
}

void CatItem::addParents(QList<CatItem> parents){
    for(int i=0; i<parents.count(); i++){
        if(parents[i].getPath() !=this->getPath()){
            addParent(parents[i]);
        }
    }
//    QHash<QString, DetachedChildRelation> relations;
//    for(int i=0;i < d->children_detached_list.count();i++){
//        DetachedChildRelation dcr = d->children_detached_list[i];
//        relations[dcr.toString()] = dcr;
//    }
//
//    for(int i=0; !(i==parents.size());i++){
//        DetachedChildRelation dcr(parents[i],*this, "");
//        relations[dcr.toString()] = dcr;
//    }
//
//    d->children_detached_list = relations.values();
}




void CatItem::adjustChildrensPath(QString newPath){

    int insertIndex = d->children_detached_list.count();
    QString oldPath = getPath();

    for(int i=0;i < insertIndex;i++){
        if(d->children_detached_list[i].getParent().getPath()==oldPath){
            CatItem& toMerge= d->children_detached_list[i].getParentRef();
            toMerge.d->m_path = newPath;
        } else  if(d->children_detached_list[i].getChild().getPath()==oldPath){
            CatItem& toMerge= d->children_detached_list[i].getChildRef();
            toMerge.d->m_path = newPath;
        }
    }
}

void CatItem::saveToSettings(QSettings* set){

    QHash<QString, QString>::iterator i = d->m_data.begin();
    for(;i!=d->m_data.end(); i++){
        set->setValue(i.key(),i.value());
    }
    set->setValue(PATH_KEY_STR,d->m_path);
    if(!d->m_name.isEmpty()){
        set->setValue(NAME_KEY_STR,d->m_name);
    }

    for(int i=0;i<d->children_detached_list.count();i++){
        QString k;
        if(d->children_detached_list[i].isForDelete()){continue;}

        if(d->children_detached_list[i].getParentPath()==getPath()){
            k = ITEM_CHILD_CONFIG_FIELD;
            if(d->children_detached_list[i].getIsDefaultActionParent()){
                k+= DEFAULT_APP_ACTION_MIXIN;
            }
            k +=QString::number(i);
            set->setValue(k,d->children_detached_list[i].getChildPath());
        } else {
            k = ITEM_PARENT_CONFIG_FIELD;
            if(d->children_detached_list[i].getIsDefaultActionParent()){
                k+= DEFAULT_APP_ACTION_MIXIN;
            }
            k+= QString::number(i);
            set->setValue(k,d->children_detached_list[i].getParentPath());
        }
    }
}

//I should use this for mime types but I'm not...
void CatItem::addCharacteristic(QString characteristicName,
                    int value, BaseChildRelation::ChildRelType relationType){

    if(characteristicName== TIME_SORT_CATEGORY_ITEM_PATH){
        this->setCreationTime(value);
        return;
    }

    QString valueName;
    QString valuePath;
    CatItem valueContainer;
    if(!characteristicName.contains(PREFIX_REGEX)) {
        valueName = characteristicName;
        valuePath = (addPrefix( VALUE_PREFIX,valueName));;
    } else {
        QStringList sl = characteristicName.split(PREFIX_REGEX);
        valueName = sl[0];
        valuePath = characteristicName;
    }

    valueContainer.setPath(valuePath);
    valueContainer.setCustomPluginValue(CHARACTERISTIC_VALUE_KEY, value);
    if(relationType != BaseChildRelation::NORMAL){

        valueContainer.setTagLevel(CatItem::ACTION_TAG);
        valueContainer.setItemType(CatItem::ACTION_TYPE);
    }

    CatItem charContainer(addPrefix(BUILTIN_PREFIX,characteristicName));

    charContainer.setLabel(CHARACTERISTIC_SOURCE_LABEL);

    valueContainer.addParent(charContainer);
    this->addParent(valueContainer);

    setCustomPluginValue(CHARACTERISTIC_CACHE_PREFIX+characteristicName,value);

}

void CatItem::addCharacteristic(QString characteristicName, QString
                                valueStr, BaseChildRelation::ChildRelType relationType){

    QString valueName;
    QString valuePath;
    if(!characteristicName.contains(PREFIX_REGEX)) {
        valueName = characteristicName;
        valuePath = (addPrefix( VALUE_PREFIX,valueName));;
    } else {
        QStringList sl = characteristicName.split(PREFIX_REGEX);
        valueName = sl[0];
        valuePath = characteristicName;
    }

    CatItem valueContainer(valuePath);
    valueContainer.setCustomPluginInfo(CHARACTERISTIC_VALUE_KEY, valueStr);
    valueContainer.setCustomPluginInfo(CHARACTERISTIC_NAME_KEY, characteristicName);
    CatItem charContainer(addPrefix(BUILTIN_PREFIX,characteristicName));
    if(relationType !=BaseChildRelation::NORMAL){
        valueContainer.setTagLevel(CatItem::ACTION_TAG);
        valueContainer.setItemType(CatItem::ACTION_TYPE);
    }
    valueContainer.addParent(charContainer);
    setCustomPluginInfo(CHARACTERISTIC_CACHE_PREFIX+characteristicName,valueStr);

}

QString CatItem::getCharacteristic(QString name, QString defaultValue){
    QList<CatItem> pars = getParents();
    for(int i=0; i< pars.count(); i++){
        if (pars[i].hasLabel(CHARACTERISTIC_NAME_KEY)){
            if(pars[i].getCustomString(CHARACTERISTIC_NAME_KEY) == name){
                return pars[i].getCustomString(CHARACTERISTIC_VALUE_KEY);
            }
        }
    }
    return getCustomString(CHARACTERISTIC_CACHE_PREFIX+name,defaultValue);
    //addCharacteristic(name,  defaultValue, defaultRelType);

}

long CatItem::getCharacteristicValue(QString name, long defaultValue){

    QString resStr = getCharacteristic(name,QString::number(defaultValue));
    return resStr.toLong();
}

CatItem CatItem::getParentByLabel(QString label){

    QList<CatItem> pars = getParents();
    for(int i=0; i< pars.count(); i++){
        if (pars[i].hasLabel(label) && !pars[i].getCustomString(label).isEmpty()){
            return pars[i];
        }
    }
    return CatItem();
}

QList<CatItem> CatItem::getCharacteristics(){
    QList<CatItem> pars = getParents();
    QList<CatItem> resList;
    for(int i=0; i< pars.count(); i++){
        if(pars[i].hasLabel(TAG_LABEL)){
            CatItem p = pars[i];
            p.setCustomPluginInfo(TEMP_CHARACTERISTIC_VALUE_KEY, TAG_CONSTANT_VALUE);
            resList.append(pars[i]);
        } else if (pars[i].hasLabel(CHARACTERISTIC_NAME_KEY)){
            QList<CatItem> gps = pars[i].getParents();

            QString charValue = pars[i].getCustomString(CHARACTERISTIC_VALUE_KEY);
            for(int j=0; j< gps.count(); j++){
                if(gps[i].getPath() == addPrefix(BUILTIN_PREFIX,charValue)){
                    gps[i].setCustomPluginInfo(TEMP_CHARACTERISTIC_VALUE_KEY, charValue);
                    resList.append(gps[i]);
                    break;
                }
            }
        }
    }
    return resList;
}


CatItem CatItem::setChildForRemove(CatItem childItem){
    Q_ASSERT(!childItem.getPath().isEmpty());
    CatItem res;
    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChild()==childItem){
            res = d->children_detached_list[i].getChild();
            d->children_detached_list[i].setForDelete();
            //d->children_detached_list.removeAt(i);
        }
    }
    return res;
}


bool CatItem::mergeItem(CatItem s, bool allowDiff, bool thisIsOriginal, QSet<QString> childRecGuard){

    Q_ASSERT(s.getFullWeight() >0);
    Q_ASSERT(getFullWeight() >0);
#  ifndef QT_NO_DEBUG
    if(getPath() != s.getPath() && !allowDiff){
        qDebug() << "error merging: " << getPath() << " with " << s.getPath() << " WTF!";
    }
#  endif
    Q_ASSERT(getPath() == s.getPath() || allowDiff);
    Q_ASSERT(!s.getPath().isEmpty());
    if(allowDiff){ d->m_path = s.getPath(); }

    childRecGuard.insert(getPath());

    if(s.hasRealName()){
        setName(s.getName());
    }
    if(s.getMatchType()!=CatItem::USER_KEYS){
        setMatchType(s.getMatchType());
    }
    setMatchIndex(s.getMatchIndex());

    if(s.getIsTempItem()){ return false; }
    if(!hasLabel(TIME_EXPLICITELY_SET_KEY_STR)
            &&
            !s.hasLabel(TIME_EXPLICITELY_SET_KEY_STR) ){
        d->m_creation_time = MIN(d->m_creation_time, s.d->m_creation_time);
    } else if(s.hasLabel(TIME_EXPLICITELY_SET_KEY_STR)){
        d->m_creation_time = s.d->m_creation_time; }

    {QHash<QString, QString>::iterator i;
    bool soureStub = s.isStub();
    for(i=s.d->m_data.begin();i!=s.d->m_data.end();++i){
        if(i.key()==CHILD_COUNT_STUBBED_KEY){
            if(i.value()=="-1"){ continue;}
        }

        if(i.key().endsWith(ATTRIB_PRIORITY_POSTFIX))
            { continue; }

        QString pKey = i.key()+ATTRIB_PRIORITY_POSTFIX;
        if(d->m_data.contains(pKey) ){
            if(thisIsOriginal || getCustomValue(pKey) > s.getCustomValue(pKey))
                {continue;}
        }

        if(i.key() == IS_CHILD_STUB_KEY){
            if(d->m_data.contains(IS_CHILD_STUB_KEY)){
                d->m_data[i.key()]=  (int)((bool)(d->m_data[i.key()].toInt()) && (bool)(i.value().toInt()));
                continue;
            }
        }

        if(s.d->m_data.contains(pKey) ) {
            d->m_data[pKey]=s.d->m_data[pKey];
        }

        if(!d->m_data.contains(i.key()) || !soureStub){
            d->m_data[i.key()]=i.value();
        }
    }}

    {QHash<QString, QVariant>::iterator i;
    for(i=s.d->variantData.begin();i!=s.d->variantData.end();++i){
        d->variantData[i.key()]=i.value();
    }}

    //Very costly (if you have many children)...
    //int childCount=this->getChildCount();
    for(int i=0; i < s.d->children_detached_list.count();i++){
        int index=-1;
        for(int j=0; j < d->children_detached_list.count();j++){
            if(s.d->children_detached_list[i].getParentPath() ==
                    d->children_detached_list[j].getParentPath() &&
               s.d->children_detached_list[i].getChildPath() ==
                    d->children_detached_list[j].getChildPath()){
                index = j;
                break;
            }
        }
        //index = d->children_detached_list.indexOf(s.d->children_detached_list[i]);
        if(index != -1){
            DetachedChildRelation& originalRel = d->children_detached_list[index];
            bool fromParent = ( originalRel.getParentPath() == getPath() );
            if((!childRecGuard.contains(originalRel.getParentPath()) && !fromParent)||
                    (!childRecGuard.contains(originalRel.getChildPath()) && fromParent)){
                childRecGuard.insert(originalRel.getParentPath());
                childRecGuard.insert(originalRel.getChildPath());
                originalRel.merge(s.d->children_detached_list[i],fromParent, childRecGuard);
            }
        } else {
            d->children_detached_list.append(
                    s.d->children_detached_list[i]);
            //childCount++;
        }
    }
    //setChildCount(childCount);
    return true;
}

QList<CatItem> CatItem::getCategories() const{
    QList<CatItem> res;

    for(int i=0;i<d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getChildPath() == getPath()&&
           !d->children_detached_list[i].isForDelete()){
            CatItem par = (d->children_detached_list[i]).getParent();
            if((int)(par.getTagLevel()) >
                    (int)CatItem::ATOMIC_ELEMENT){
                res.append(d->children_detached_list[i].getParent());
            }
        }
    }
    return res;

}

bool CatItem::hasChildren() const {
    if(isChildStubbed()){
        return (getChildCount()>0);
    }

    for(int i=0; !(i==d->children_detached_list.size());i++){
        Q_ASSERT(((d->children_detached_list.value(i).getParentPath() == getPath()
                 ||
                 d->children_detached_list.value(i).getChildPath() == getPath())) &&
                 !d->children_detached_list[i].isForDelete());
        if(d->children_detached_list.value(i).getParentPath() == getPath()){
            Q_ASSERT( d->children_detached_list.value(i).getChildPath() != getPath());
            return true;
        }
    }
    return false;
}

QList<CatItem> CatItem::getChildren(BaseChildRelation::ChildRelType relLevel){
    //Q_ASSERT(!isChildStubbed());

    QList<CatItem> resList;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        Q_ASSERT(d->children_detached_list.value(i).getParentPath() == getPath()
                 ||
                 d->children_detached_list.value(i).getChildPath() == getPath());
        if(d->children_detached_list.value(i).getParentPath() == getPath() &&
           !d->children_detached_list[i].isForDelete() &&
           ((int)d->children_detached_list[i].getChildRelType() >=(int)relLevel)){
            resList.append(d->children_detached_list.value(i).getChild());
        }
    }
    return resList;
}

QList<CatItem> CatItem::getRelatedItems(){
    QList<CatItem> characteristicItems;
    QList<CatItem> anonChildren;
    if(onlyChildrenRelated()){
        anonChildren = getChildren();
        for(int i=0; !(i==anonChildren.size());i++)
            { characteristicItems.append(subChilds(anonChildren[i])); }
    } else {
        for(int i=0; !(i==d->children_detached_list.size());i++){
            CatItem it;
            QString charName;
            Q_ASSERT(d->children_detached_list.value(i).getParentPath() == getPath()
                     ||
                     d->children_detached_list.value(i).getChildPath() == getPath());
            if(d->children_detached_list.value(i).getParentPath() == getPath() &&
               !d->children_detached_list[i].isForDelete() ){
                it = d->children_detached_list.value(i).getChild();
                charName = it.getRelationName();
                if(charName.isEmpty()){
                    continue;
                }
            } else if(d->children_detached_list.value(i).getChildPath() == getPath() &&
               !d->children_detached_list[i].isForDelete() ){
                it = d->children_detached_list.value(i).getParent();
                charName = it.getRelationName();
                //characteristicItems.append(d->children_detached_list.value(i).getChild());
            }
            ItemType r = it.getItemType();
            if(r == CatItem::ORGANIZING_TYPE || r== PLUGIN_TYPE){
                continue;
            }
            if(charName.isEmpty()){
                anonChildren.append(subChilds(it));
            } else {
                QString newName = charName + ": "+ it.getName();
                it.setName(newName);
                characteristicItems.append(subChilds(it));
            }
        }
        characteristicItems.append(anonChildren);
    }
    return characteristicItems;
}


QList<CatItem> CatItem::subChilds(CatItem it){
    QList<CatItem> res;
    QList<CatItem> childs = it.getChildren();
    if(it.getIsIntermediateSource() && childs.count()>0){
        res.append(childs);
    } else {
        res.append(it);
    }
    return res;

}

QList<CatItem> CatItem::organizingCategoryParents(){

    QList<CatItem> parentItems;

    for(int i=0; !(i==d->children_detached_list.size());i++){
        CatItem it;
        Q_ASSERT(d->children_detached_list.value(i).getParentPath() == getPath()
                 ||
                 d->children_detached_list.value(i).getChildPath() == getPath());
        if(d->children_detached_list.value(i).getChildPath() == getPath() &&
                  !d->children_detached_list[i].isForDelete() ){
            it = d->children_detached_list.value(i).getParent();

            TagLevel tl = it.getTagLevel();
            ItemType iType = it.getItemType();

            if(tl== CONTAINER || tl== POSSIBLE_TAG || tl==EXTERNAL_SOURCE ||
               iType==PERSON || iType == ORGANIZING_TYPE){
                parentItems.append(it);

            }
        }
    }

    return parentItems;
}


QList<CatItem> CatItem::getAssociatedNormalVerbs(){
    if(this->getItemType() ==CatItem::VERB){
        return QList<CatItem>();
    }
    CatItem par = getTypeParent(BaseChildRelation::DATA_CATEGORY_PARENT);

    QList<CatItem> actionChildren;

    for(int i=0; !(i==par.d->children_detached_list.size());i++){
        CatItem posAction = par.d->children_detached_list[i].getChild();
        BaseChildRelation::ChildRelType rt = par.d->children_detached_list[i].getChildRelType();

        if(getItemType() > CatItem::LOCAL_DATA_DOCUMENT
                && !(hasLabel(FILE_CATALOG_PLUGIN_STR)) &&
                !(bool)posAction.getCustomValue(VERB_TAKES_URIS)){
            continue;
        }
        if(!(rt == BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD
                || rt == BaseChildRelation::OPTIONAL_ACTION_CHILD
                || rt == BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM)){
            continue;
        }
        if(par.d->children_detached_list[i].isForDelete()){
            continue;
        }
        actionChildren.append(posAction);
    }
    return actionChildren;
}

QList<CatItem> CatItem::getDataTypesAssocWVerb(){
    if(this->getItemType() !=CatItem::VERB){
        return QList<CatItem>();
    }
    QList<CatItem> actionParent;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        CatItem associatedParent = d->children_detached_list[i].getParent();
        BaseChildRelation::ChildRelType rt = d->children_detached_list[i].getChildRelType();
        if((rt == BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD
           || rt == BaseChildRelation::OPTIONAL_ACTION_CHILD
           || rt == BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM)&&
           !d->children_detached_list[i].isForDelete()){
            actionParent.append(associatedParent);
        }
    }
    return actionParent;
}

QList<CatItem> CatItem::getAllActionTypeParents(){
    if(this->getItemType() ==CatItem::VERB){
        return QList<CatItem>();
    }

    QList<CatItem> res;
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()&&
           !d->children_detached_list[i].isForDelete()){
            if(d->children_detached_list[i].getChildRelType()
                    == BaseChildRelation::DATA_CATEGORY_PARENT){
                res.append(d->children_detached_list[i].getParent());
            }
        }
    }
    return res;


}

QList<CatItem> CatItem::getParents(){
    QList<CatItem> resList;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()&&
           !d->children_detached_list[i].isForDelete()){
            resList.append(d->children_detached_list.value(i).getParent());
        }
    }
    return resList;
}

CatItem& CatItem::getParentRef(CatItem model)  {
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list[i].getParentPath() == model.getPath()){
            return d->children_detached_list[i].getParentRef();
        }
    }
    return the_static_null_catItem;
}

CatItem& CatItem::getChildRef(CatItem model) {
    Q_ASSERT(!isChildStubbed());
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list[i].getChildPath() == model.getPath()&&
           !d->children_detached_list[i].isForDelete()){
            return d->children_detached_list[i].getParentRef();
        }
    }
    return the_static_null_catItem;
}

QList<DetachedChildRelation> CatItem::getParentRelations() const {
    QList<DetachedChildRelation> resList;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            resList.append(d->children_detached_list.value(i));
        }
    }
    return resList;

}

QList<DetachedChildRelation> CatItem::getChildRelations() const {
    Q_ASSERT(!isChildStubbed());
    QList<DetachedChildRelation> resList;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            resList.append(d->children_detached_list.value(i));
        }
    }
    return resList;
}

QList<DetachedChildRelation> CatItem::getSiblingOrderRels() const {
    QList<DetachedChildRelation> resList;
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list.value(i).getSiblingOrder()!=-1){
                resList.append(d->children_detached_list.value(i));
            }
        }
    }
    return resList;
}

bool CatItem::hasChild(CatItem ci) const {
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getChildPath() == ci.getPath()&&
           !d->children_detached_list[i].isForDelete()){
            Q_ASSERT(ci.getPath() ==d->children_detached_list.value(i).getChildPath());
            return true;
        }
    }
    return false;
}

bool CatItem::hasParent(CatItem ci) const{
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getParentPath() == ci.getPath()){
            //Q_ASSERT(ci.getPath() ==d->children_detached_list.value(i).getChildPath());
            return true;
        }
    }
    return false;
}

bool CatItem::hasParentType(QString pluginId) const {
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list.value(i).getParent().hasLabel(pluginId)&&
           !d->children_detached_list[i].isForDelete()){
            return true;
        }
    }
    return false;
}

CatItem CatItem::getActionTypeChild() const {
    CatItem actionChild;
    CatItem posAction;
    bool defaultFound = false;
    bool associatedFound = false;

    for(int i=0; !(i==d->children_detached_list.size());i++){
        posAction = d->children_detached_list[i].getChild();
        if(d->children_detached_list[i].getIsDefaultAction()){
            Q_ASSERT(posAction.getActionType() == CatItem::VERB_ACTION);
            actionChild = posAction;
            defaultFound = true;
        }
        if(d->children_detached_list[i].getChildRelType() == BaseChildRelation::OPTIONAL_ACTION_CHILD
           && !defaultFound && !associatedFound){
            Q_ASSERT(posAction.getActionType() == CatItem::VERB_ACTION);
            actionChild = posAction;
        }
        if((d->children_detached_list[i].getChildRelType() == BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD
            || d->children_detached_list[i].getChildRelType() == BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM
            || d->children_detached_list[i].getChildRelType() == BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM)
           && !defaultFound){
            Q_ASSERT(posAction.getActionType() == CatItem::VERB_ACTION);
            actionChild = posAction;
            associatedFound = true;
        }
        if( d->children_detached_list[i].getChild().getActionType()== CatItem::VERB_ACTION
            && actionChild.isEmpty()){
            actionChild = posAction;
        }
    }
    return actionChild;
}

QString  CatItem::getParentChar(QString key){
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list[i].getParent().hasLabel(key)){
                return d->children_detached_list[i].getParent().getCustomString(key);
            }
        }
    }
    return "";
}

CatItem CatItem::getTypeParent(CatItem::ItemType t) const {
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list[i].getParent().getItemType()== t){
                return d->children_detached_list[i].getParent();
            }
        }
    }
    return CatItem();
}

QList<CatItem> CatItem::getTypeParents(CatItem::ItemType t) const {
    QList<CatItem> res;
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list[i].getParent().getItemType()== t){
                res.append(d->children_detached_list[i].getParent());
            }
        }
    }
    return res;
}

CatItem CatItem::getTypeParent(BaseChildRelation::ChildRelType t) const {
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list[i].getChildRelType()== t){
                return d->children_detached_list[i].getParent();
            }
        }
    }
    return CatItem();
}

QList<CatItem> CatItem::getTypeParents(BaseChildRelation::ChildRelType t) const {
    QList<CatItem> res;
    for(int i=0; i < d->children_detached_list.size();i++){
        if(d->children_detached_list.value(i).getChildPath() == getPath()){
            if(d->children_detached_list[i].getChildRelType()== t){
                res.append(d->children_detached_list[i].getParent());
            }
        }
    }
    return res;
}

CatItem CatItem::getActCategory() const {
    for(int i=0; !(i==d->children_detached_list.size());i++){
        if(d->children_detached_list[i].getIsDefaultAction()
            && d->children_detached_list[i].getChild().getActionType()==CatItem::VERB_ACTION ){
            return d->children_detached_list[i].getChild();
        }
    }
    return CatItem();

}


void CatItem::addSavedRelations(DetachedChildRelation cR){
    if(cR.getParentPath() == getPath() || (cR.getChildPath() == getPath())){
        for(int i=0; i < d->children_detached_list.size();i++){
            if(d->children_detached_list[i].getChildPath() == cR.getChildPath()
                && d->children_detached_list[i].getParentPath() == cR.getParentPath()){
                d->children_detached_list[i].merge(cR,false,QSet<QString>());
                return;
                //Q_ASSERT(false);
            }
        }
        d->children_detached_list.append(cR);
    }
}


CatItem CatItem::getChild(int i){
    Q_ASSERT(!isChildStubbed());
    return d->children_detached_list[i].getChild();
}

CatItem CatItem::getChildById(qint32 id){
    Q_ASSERT(!isChildStubbed());
    for(int i=0;i < d->children_detached_list.count();i++){
        if(d->children_detached_list[i].isForDelete()){continue;}
        if(d->children_detached_list[i].getChild().getItemId()==id)
            { return d->children_detached_list[i].getChild();}
    }
    return CatItem();

}

void CatItem::setPluginSourceForChildren(QString pluginName){
    Q_ASSERT(!pluginName.isEmpty());
    for(int i=0;i < d->children_detached_list.count();i++){
        if(d->children_detached_list[i].getMatchPath()=="")
            { d->children_detached_list[i].setMatchPath(pluginName);}
    }
}

//bool InputList::acceptItem(CatItem* it){
//    QString argTypeString;
//    if(inputData[itemPositionIndex].organizingArg.isCustomVerb()){
//        argTypeString = getDefaultCustomArg().getArgType();
//    } else {
//        argTypeString = inputData[itemPositionIndex].organizingArg.getArgType();
//    }
//    if(!(argTypeString.isEmpty())){
//        return it->getParents().contains(CatItem(argTypeString));
//        //return it->matchParent(argTypeString);
//    } else if(itemPositionIndex==0){
//        return true;
//    } else  if (it->getActionType()==CatItem::OPERATION_ACTION) {
//        return !isLabeledFromStart();
//    } else if (isLabeledFromEnd()){
//        return (it->getActionType()==CatItem::OPERATION_ACTION);
//    } else if(it->getActionType() == CatItem::VERB_ACTION){
//        return !hasVerb;
//    } else if(it->getActionType()==CatItem::NOUN_ACTION){
//        return normalVerbFromStart;
//    }
//    return true;
//
//}


/*! \file
    \brief A Documented file.
    
    Details.
*/


#ifdef Q_WS_WIN
int getDesktop() { return DESKTOP_WINDOWS; }
void runProgram(QString path, QString args) {
	SHELLEXECUTEINFO ShExecInfo;

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_FLAG_NO_UI;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = (LPCTSTR) (path).utf16();
	if (args != "") {
		ShExecInfo.lpParameters = (LPCTSTR) args.utf16();
	} else {
		ShExecInfo.lpParameters = NULL;
	}
	QDir dir(path);
	QFileInfo info(path);
	if (!info.isDir() && info.isFile())
		dir.cdUp();
	ShExecInfo.lpDirectory = (LPCTSTR)QDir::toNativeSeparators(dir.absolutePath()).utf16();
	ShExecInfo.nShow = SW_NORMAL;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);	
}
#endif

#ifdef Q_WS_MAC

#endif

#ifdef Q_WS_X11
int getDesktop()
{
    QStringList list = QProcess::systemEnvironment();
    foreach(QString s, list)
	{
	    if (s.startsWith("GNOME_DESKTOP_SESSION"))
		return DESKTOP_GNOME;
	    else if (s.startsWith("KDE_FULL_SESSION"))
		return DESKTOP_KDE;
	}
    return -1;
}

void runProgram(QString toRun) {
    QProcess proc;
    toRun = toRun.simplified();
    proc.startDetached(toRun);
    return;
}


#endif

