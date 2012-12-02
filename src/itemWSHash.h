#ifndef INNER_ITEM_H
#define INNER_ITEM_H

#include <QString>
#include <QVariant>
#include <QList>
#include <QSettings>
#include <QByteArray>
#include <QTextStream>
#include <QUrl>

#include "constants.h"
#include "item.h"
//#include "platform_base.h"

class PlatformBase;
#define USING_ITEM_HASH_CLASS
#define USING_STRING_HASH

//extern CatItem* the_static_null_catItemPtr;
QString contextualTimeString(QDateTime t);

class DetachedChildRelation;

class CatItem {
public:
    friend QDataStream &operator >> (QDataStream &in, CatItem &item);
    friend QDataStream &operator<<(QDataStream &out, const CatItem &item);
    friend class DetachedChildRelation;
    enum FilterRole{
        UNDEFINED_ELEMENT=0,
        MESSAGE_ELEMENT=1,
        CATEGORY_FILTER=2,
        ACTIVE_CATEGORY=3,
        SUBCATEGORY_FILTER=4,
        ERROR_MESSAG=5
    };

    enum TagLevel{
        ATOMIC_ELEMENT=0,
        CONTAINER=1,
        BASE_PLUGIN=2,
        ACTION_TAG=3,
        INTERNAL_STATIC_SOURCE=4,
        INTERNAL_SOURCE=5,
        EXTERNAL_SOURCE=6,
        POSSIBLE_TAG=7, //includes directories...
        KEY_WORD=8,
        //EXPLICIT_TAG=9
    };
    enum MatchType {
        NOT_MATCHED,
        DATA_INFERENCE,
        EXTERNAL_INFO,
        CHILD_RELATION,
        USER_KEYWORD,
        USER_WORDS,
        USER_KEYS
    };

    enum ActionType {
        NOUN_ACTION,
        VERB_ACTION,
        OPERATION_ACTION,
        UNDEFINED_ACTION,
    };

    enum DistinguishedType {
        NOT_DISTINGUISHED=0,
        HIGH_VALUE,
        UNUSUAL_TYPE,
        UNUSUAL_ACTION,
        LAST_DISTINGUISHED
    };

    //ORDER MATTTERS!! - organized by
    enum ItemType {
        //Actions
        MIN_TYPE=0,
        //SCRIPT, //local text file that's run
        VERB=10, //Application, Web service Verb, Anything else 'external' you can tell to take an action
        TAG=20, //activate a particular tag, categorize using a tag...

        //Operations
        //TIME, //categorize by time...
        //LOCATION, //" location
        OPERATION=40, //things like *creating* a tag, a meta-action or action within Oneline of some sort..

        //Below are "atomic"/noun types...
        LOCAL_DATA_DOCUMENT=50, //A bucket of text, especially something that can have tags...
        LOCAL=55, //For filtering only - file or an app or anything local
        LOCAL_DATA_FOLDER=60, //A bucket of text, especially something that can have tags...
        ACTION_TYPE=70, // Mime type classifies the kind action - has a verb as a child to do action

        //Organizing
        ORGANIZING_TYPE=75, // Misc type classifies other items
        PLUGIN_TYPE=80, // Specific to a plugin and source ...Classify operation - Pure key word/search item...

        //Web/net based
        PUBLIC_DOCUMENT=90, //A web-page, ftp document or similar public thing...
        PUBLIC_POST=100, //A TIME-SENSATIVE web-page, facebook post, Tweet or similar
        PUBLIC_FEED=102, //Posts and documents
        PERSON=105, //Contact Anyone or thing you can send messages to...
        MESSAGE=110, //Document from a contact -- private to a person or person(s)...
        ALERT =120, // An internal message or an action that is done once
        VALUE=130, //Number, matrix, internal or program-object
        MAX_TYPE=140
    };

    enum InterationType {
        NORMAL_INTERACTION,
        CUSTOM_VERB_INTERACTION,
        VERB_ARG_INTERACTION
    };


    class InnerItem : public QSharedData {
    public:
        InnerItem(){
            m_creation_time = 0;
            m_matchType=CatItem::USER_KEYS;
        }
        InnerItem(const InnerItem &other)
            : QSharedData(other),
            m_data(other.m_data),
            m_path(other.m_path),
            m_creation_time(other.m_creation_time),
            children_detached_list(other.children_detached_list),
            m_name(other.m_name),
            m_lowName(other.m_lowName),
            m_matchType(other.m_matchType),
            variantData(other.variantData),
            m_matchIndex(other.m_matchIndex)
        {
            if(m_lowName.isEmpty() && !m_name.isEmpty()){
                m_lowName = m_name.toLower();
            }
        }
        ~InnerItem() { }

        QHash<QString, QString > m_data;
        QString m_path;
        time_t m_creation_time;
        QList<DetachedChildRelation> children_detached_list;

        //Essentially values called most often and so "cached" instead of in hash..
        QString m_name;
        QString m_lowName;
        CatItem::MatchType m_matchType;

        //Temp info...
        QHash<QString, QVariant > variantData;//only to temporarily store icon or other data
        QList<int> m_matchIndex;
    };

private:

//    QHash<QString, QString > d->m_data;
//    QString d->m_path;
//    time_t d->m_creation_time;
//    QList<DetachedChildRelation> d->children_detached_list;
//    QHash<QString, QVariant > d->variantData;
//    QList<int> matchIndex;
//
//    //Essentially values called most often and so "cached" instead of in hash..
//    QString d->m_name;
//    QString d->m_lowName;
//    MatchType d->m_matchType;

QSharedDataPointer<InnerItem> d;


public:
    static QString typeName(){return QString("CatItem");}

    CatItem(){
        d = new InnerItem();
    }

    explicit CatItem(QString full): d() {
        d = new InnerItem();

        setPath(full);
        Q_ASSERT(!full.isEmpty());
        d->m_creation_time = appGlobalTime();
    }

    CatItem(QString full, QString shortN) {
        d = new InnerItem();
        setName(shortN);
        setPath(full);
        Q_ASSERT(!full.isEmpty());
        d->m_creation_time = appGlobalTime();
    }

    CatItem(QString full, QString shortN, uint i_d) {
        d = new InnerItem();
        setPath(full);
        setName(shortN);
        setPluginId((i_d));
        Q_ASSERT(!full.isEmpty());
        d->m_creation_time = appGlobalTime();
    }

    /** This was the constructor most used by old plugins - remove eventually
    \param full The full path of the file to execute
    \param The abbreviated name for the entry
    \param i_d Plugin id (0 for app itself)
    \param iconPath The path to the icon for this entry
    */
    CatItem(QString full, QString shortN, uint i_d, QString iconPath){
        d = new InnerItem();
        setPath(full);
        setName(shortN);
        setPluginId((i_d));
        setIcon(iconPath);
        Q_ASSERT(!full.isEmpty());
        d->m_creation_time = appGlobalTime();
    }

    CatItem(QString description, QString full, QString shortN, uint i_d, QString iconPath){
        d = new InnerItem();
        setPath(full);
        d->m_creation_time = appGlobalTime();
        setDescription(description);
        setName(shortN);
        setPluginId((i_d));
        setIcon(iconPath);
        Q_ASSERT(!full.isEmpty());
    }

    explicit CatItem(QList<QString> fieldList){
        d = new InnerItem();
        d->m_creation_time = appGlobalTime();
        setPath(fieldList[0]);
        setName( fieldList.count() > 1 ? fieldList[1] : fieldList[0]);
        setDescription( fieldList.count() > 2 ? fieldList[2] : fieldList[0]);
    }

    explicit CatItem(QSettings* set, CatItem pluginParent=CatItem()){
        d = new InnerItem();
        d->m_creation_time = appGlobalTime();
        setItemType(VERB);
        setUseDescription(true);

        if(set->contains(PATH_KEY_STR)){
            setPath(set->value(PATH_KEY_STR).toString());
        } else {
            setPath(addPrefix(CUSTOM_VERB_PREFIX,set->fileName()));
            QFileInfo fi(set->fileName());
            QString name = fi.baseName();
            setName(name);
        }

        this->setCustomPluginInfo(FILLIN_GEN_FILE_KEY, set->fileName());

        QString keyedName = set->value(NAME_KEY_STR).toString();
        if(!keyedName.isEmpty()){ setName(keyedName); }
        setDescription(set->value(ITEM_DESCRIPTION_CONFIG_FIELD).toString());
        setVerbDescription( set->value(ITEM_DESCRIPTION_GEN_FIELD).toString());
        QStringList sl = set->childKeys();
        for(int i=0;i < sl.count();i++){

            QString key = sl[i];
            if(key == CUSTOM_VERB_DISCRIPTION_KEY_STR){ setItemType(CatItem::VERB); }

            if(key == ITEM_PARENT_CONFIG_FIELD ){
                CatItem par(set->value(key).toString());
                pluginParent = par;
                addParent(pluginParent);
            } else if(key == ITEM_TYPE_CONFIG_FIEILD){
                setItemType((CatItem::ItemType)set->value(sl[i]).toLongLong());
            } else if(key.startsWith(ITEM_SYNONYM_PATH)){
                CatItem par(set->value(key).toString());
                addParent(par, "", BaseChildRelation::SYNONYM);
            } else if(key.startsWith(ITEM_PARENT_CONFIG_FIELD)){
                CatItem p(set->value(key).toString());
                p.setItemType(CatItem::TAG);
                p.setStub(true);
                addParent(p,"",BaseChildRelation::UNDETERMINED_REL, key.contains(DEFAULT_APP_ACTION_MIXIN));
            } else if(key.startsWith(ITEM_CHILD_CONFIG_FIELD)){
                CatItem p(set->value(key).toString());
                p.setStub(true);
                addChild(p);
            } else if(key.startsWith(ITEM_SECTION_CHILD_CONFIG_FIELD)){
                QString group = set->value(key).toString();
                set->beginGroup(group);
                CatItem p(set,pluginParent);
                addChild(p);
                QString endGroup = set->group();
                if(!endGroup.isEmpty()){
                    set->endGroup();
                }
                p.setStub(true);
            } else if(key!=PATH_KEY_STR) {
                setCustomPluginInfo(key,set->value(key).toString());
            }
        }

        QList<CatItem> cA = getRequiredCustomArguments();
        for(int i=0; i < cA.count(); i++){
            addParent(cA[i],
            FILE_CATALOG_CUSTOM_VERB_SOURCE, BaseChildRelation::OPTIONAL_ACTION_CHILD);
        }

        //last so it can try to find plugin parent..
        if(set->contains(EXTERNAL_WEIGHT_KEY_STR)){
            setExternalWeight(set->value(PATH_KEY_STR).toLongLong(), pluginParent);
        }
    }

    void saveToSettings(QSettings* set);

    CatItem(const CatItem &s): d(s.d) {
//        d = new InnerItem();
//        assign_from(s);
    }

//    CatItem(CatItem &s) : d(s.d) {
//        d = new InnerItem();
//        assign_from(s);
//    }
//
//    CatItem& operator=( const CatItem &s ) {
//        assign_from(s);
//        return *this;
//    }

    CatItem (QString templateName, QVariantHash vh, QString prefix="");

    bool operator==( const CatItem &s ) {
        return (d->m_path == s.d->m_path);
    }
    bool operator==(const CatItem& s) const{
        return (d->m_path == s.d->m_path);
    }

    bool operator < (const CatItem& other) const{
        if (d->m_path < other.d->m_path)
        {return true;}
        return false;
    }

    bool operator > (const CatItem& other) const{
        if (d->m_path > other.d->m_path)
            {return true;}
        return false;
    }


    static CatItem createActionItem(QString content_type, QString desc="",
                                    QString name="", QString pluginName = FILE_CATALOG_PLUGIN_STR){
        CatItem mimeType(addPrefix(ACTION_TYPE_PREFIX,QString(content_type)),content_type);
        if(!name.isEmpty()){
            mimeType.setName(name );
        } else { mimeType.setName(content_type); }

        if(!desc.isEmpty()){
            mimeType.setDescription(desc);
        } else {
            mimeType.setDescription(content_type);
        }
        mimeType.setTagLevel(CatItem::ACTION_TAG);
        mimeType.setItemType(CatItem::ACTION_TYPE);
        mimeType.setExternalWeight(MINIMUM_EXTERNAL_WEIGHT,CatItem(pluginName));
        mimeType.setCustomPluginInfo(REAL_MIMETYPE_KEY,content_type);
        mimeType.setSourceWeight(MEDIUM_EXTERNAL_WEIGHT);
        return mimeType;
    }

    static CatItem createTypeItem(QString typeName){
        CatItem theType(addPrefix(TYPE_PREFIX,QString(typeName)),typeName);
        if(!typeName.isEmpty()){
            theType.setName(typeName);
        } else { theType.setName(typeName); }
        theType.setDescription(typeName);
        theType.setTagLevel(CatItem::ACTION_TAG);
        theType.setItemType(CatItem::ACTION_TYPE);
        theType.setExternalWeight(MINIMUM_EXTERNAL_WEIGHT,CatItem("internal"));
        theType.setCustomPluginInfo(REAL_MIMETYPE_KEY,typeName);
        theType.setOrganizingCharacteristic(typeName);
        return theType;
    }




    static CatItem createTempfolder(QString name, QList<CatItem> children){
        CatItem tempFolderItem(name);
        tempFolderItem.setIsTempItem(true);
        tempFolderItem.setUseLongName(true);
        tempFolderItem.setTagLevel(CatItem::POSSIBLE_TAG);
        tempFolderItem.addChildren(children);
        tempFolderItem.setIcon(INTERNAL_PSUEDO_FOLD_ICON);
        tempFolderItem.setItemType(CatItem::ORGANIZING_TYPE);
        tempFolderItem.setCustomPluginValue(PLACE_HOLDER_ITEM_KEY,(int)true);
        return tempFolderItem;
    }

    static CatItem createRunPseudoItem(){
        CatItem runItem(RUN_PSEUDO_ITEM_PATH);
        runItem.setIsTempItem(true);
        runItem.setUseLongName(true);
        runItem.setIcon(UI_NEXTACTION_PLACEHOLDER_ICON);
        return runItem;
    }

    static CatItem createFileItem(QSet<QString> &extendedTypes, QFileInfo initialInfo,
                                              CatItem& folderParentItem, int depth,
                                              UserEvent::LoadType lt,
                                              PlatformBase* thePlatform,
                                              CatItem pluginRep);

    static CatItem getPrimaryCategory(QSet<CatItem> possibleParents);
    static QString getCannonicalFieldName(QString key) {
        if(key == PATH_KEY_STR || key == "path"){
            return ANY_ITEM_NAME;
        } if(key == NUMBER_NAME || key == NAME_KEY_STR || key == "name" || key== SHORT_TEXT_NAME){
             return SHORT_TEXT_NAME;
        } else if(key == WEB_ADDRESS_NAME  || key == DEFAULT_HTML_MIME_TYPE){
            return WEB_ADDRESS_NAME;
        } else if(key==RFC_EMAIL_MESSAGE_NAME || (key=="message")){
            { return MESSAGE_NAME; }
        } else if(key == LONG_TEXT_NAME || key == USER_DESCRIPTION_KEY_STR || key == "description")
            { return LONG_TEXT_NAME; }
        return key;

    }

    static bool atUserDataField(QString argType){
        argType = getCannonicalFieldName(argType);
        if(argType == WEB_ADDRESS_NAME){ return false; }
        if(argType == EMAIL_ADDRESS_NAME){ return false; }
        if(argType == YOUR_EMAIL_ADDRESS_NAME){ return true; }
        if(argType == RFC_EMAIL_MESSAGE_NAME){ return false; }
        if(argType == FILE_NAME){ return false; }
        if(argType == FOLDER_NAME){ return false; }
        if(argType == ANY_ITEM_NAME){ return false; }
        if(argType == SHORT_TEXT_NAME){ return true; }
        if(argType == LONG_TEXT_NAME){ return true; }
        return true;
    }

    static bool atItemSelectOnlyField(QString){
        return false;
    }
    static CatItem& addChildInternal(CatItem& parentItem, CatItem& childItem, QString pluginName,
                          BaseChildRelation::ChildRelType isDefault=BaseChildRelation::NORMAL,
                          bool isDefaultAppType=false);

    static CatItem& addParentInternal(CatItem& parentItem, CatItem &childItem, QString pluginName="",
                          BaseChildRelation::ChildRelType isDefault=BaseChildRelation::NORMAL,
                          bool isDefaultAppType=false);
    static CatItem createTypeParent(CatItem::ItemType type);

    void setName(QString rawName);
    void setPath(QString rawPath);

    void makePseudoChild(int index, int total){
        CatItem it;
        it = *this;
        clearRelations();
        addChild(it);
        setListOffset(UI_LIST_OFFSET);
        setUseDescription(true);
        setName(QString(" ...")+ getName());
        setDescription(QString::number(index) + " of "+ QString::number(total));
    }

    void makePseudoParent(QString positionString){
        CatItem it;
        it = *this;
        clearRelations();
        addChild(it);
        setUseDescription(true);
        setDescription(positionString);
    }

    QString getItemTypeAsString(){
        switch(getItemType()){
            case MIN_TYPE: return "min_type";
            case VERB: return "verb";
            case TAG: return "tag";
            case OPERATION: return "operation";
            case LOCAL_DATA_DOCUMENT: return "local_data_document";
            case LOCAL: return "local";
            case LOCAL_DATA_FOLDER: return "local_data_folder";
            case ACTION_TYPE: return "action_type";
            case ORGANIZING_TYPE: return "organizing_type";
            case PLUGIN_TYPE: return "plugin_type";
            case PUBLIC_DOCUMENT: return "public_document";
            case PUBLIC_FEED: return "public_feed";
            case PUBLIC_POST: return "public_post";
            case PERSON: return "person";
            case MESSAGE: return "message";
            case ALERT: return "alert";
            case VALUE: return "value";
            case MAX_TYPE: return "max_type";
        }
        Q_ASSERT(false);
        return "unknown";
    }

    void setListOffset(int i) {
        setCustomPluginValue(LIST_OFFSET_KEY,i);
    }

    int getListOffset() {
        int offset = getCustomValue(LIST_OFFSET_KEY,0);
        return offset;
    }

    CatItem& setActionType(QString actionType, QString pluginName = FILE_CATALOG_PLUGIN_STR,
                            QString desc="", QString name=""){

        if(actionType.contains("/")){
            QStringList sl = actionType.split("/");
            if(sl[0].isEmpty()){
                CatItem typeParent = createTypeItem(sl[0]);
                addParent(typeParent);
            }
        }

        CatItem mimeParent = createActionItem(actionType,desc,name,pluginName);
        mimeParent.setStub();
        addParent(mimeParent, pluginName,BaseChildRelation::DATA_CATEGORY_PARENT);
        return getParentRef(mimeParent);
    }

    CatItem& getParentRef(CatItem model);
    CatItem& getChildRef(CatItem model);

    QString getMimeType() const {
        QString mType = getActionParentType();
        if(mType.isEmpty()){
            mType = getCustomString(REAL_MIMETYPE_KEY);
        }
        return mType;

        //return getCustomString(MIME_TYPE_NAME_KEY);
    }

    QString getMimeDescription() const {
        QString mType = getActionParentDescription();
        if(mType.isEmpty()){
            mType = getCustomString(REAL_MIMEDESCRIPT_KEY);
        }
        return mType;

        //return getCustomString(MIME_TYPE_NAME_KEY);
    }

    QString getPreviewHtml(){
        return getCustomString(HTML_PREVIEW_KEY);
    }

    void setPreviewHtml(QString k){
        return setCustomPluginInfo(HTML_PREVIEW_KEY, k);
    }

    InterationType getInterationType(){
        if(isCustomVerb()){
            return CUSTOM_VERB_INTERACTION;
        }
        if(getIsVerbArg()){
            return VERB_ARG_INTERACTION;
        }
        return NORMAL_INTERACTION;

    }

    //can be used for incrementally constructing a CatItem based on "best guess"
    CatItem& setValue(QString key, QString value ) {
        if(key.toLower()=="path" || key.toLower()=="fullpath" || key == PATH_KEY_STR){
            if(d->m_name.isEmpty() || (getName() == d->m_path)){
                QString theName;
                if(value.contains("/")){
                    QStringList sl = value.split("/");
                    if(!(sl[sl.length()-1].isEmpty())){
                        theName = sl[sl.length()-1];
                    } else if(sl.length() >1){
                        theName = sl[sl.count()-2];
                    }
                } else { theName = value;}
                if(!theName.isEmpty()){
                    setName(theName);
                }
            }
            setPath(value);
        } else if(key.toLower()=="name" || key.toLower()=="shortname"){
            setName(value);
        } else if(key.toLower()=="date" || key.toLower() == "date-modified"){
            QDateTime date = QDateTime::fromString(value, Qt::ISODate);
            d->m_creation_time = MAX(d->m_creation_time, (time_t)date.toTime_t());
        } else if(key.toLower()=="description" || key.toLower()=="fulldescription"){
            setDescription(value);
        } else if(key.toLower()=="parent" ){
            CatItem p(value);
            addParent(p);
        } else {
            setCustomPluginInfo(key,value);
        }
        return *this;
    }

    bool mergeItem(CatItem s, bool allowDiff=false, bool thisIsOriginal=false, QSet<QString> childRecGuard=
                   QSet<QString>());

    bool isEmpty() const {
        return d->m_path.isEmpty();
    }

    bool onlyChildrenRelated(){
        return !(getItemType() == CatItem::PUBLIC_POST ||
                 getItemType() == CatItem::MESSAGE);
    }


    void setForDelete() {
        setCustomPluginInfo(SET_FOR_DELETE_KEY_STR,"for_delete");
    }
    bool isForDelete() const {
        if(!getCustomString(SET_FOR_DELETE_KEY_STR).isEmpty()){
            return true;
        } else {
            return false;
        }
    }

    void setForDBInsertOnly(bool b= true){
        setCustomPluginValue(SET_FOR_DBINSERT_KEY_STR,b);
    }

    bool isForDBInsertOnly() const {
        return (bool)getCustomValue(SET_FOR_DBINSERT_KEY_STR, (bool)false);
    }

    /** Apply a label to this query segment */
    void setLabel(QString l) {
        QString str("");
        d->m_data[l] =str;

    }

    /** Check if it has the given label applied to it */
    bool hasLabel(QString maybeLabel) const {
        if(d->m_path.isEmpty()){
            return false;
        }
        if(d->m_data.contains((maybeLabel))){
            return true;
        }

        QStringList sl = d->m_path.split("://");
        QString name = sl[0] ;
        if(name == maybeLabel)
            {return true;}
//        if((sl.length() == 1 || (d->m_path[0]==QChar('/'))) && s == FILE_CATALOG_PLUGIN_STR)
//            { return true; }
        if(name == FILE_NAME && maybeLabel == FILE_CATALOG_PLUGIN_STR)
            { return true; }
        if(d->m_path[0]== CHAR_PATH_SEP && maybeLabel == FILE_CATALOG_PLUGIN_STR)
            { return true; }
        if( name == HTTP_NAME &&  maybeLabel == FIREFOX_PLUGIN_NAME)
            { return true; }
        if(name == HTTPS_NAME && maybeLabel == FIREFOX_PLUGIN_NAME)
            { return true; }
        if(d->m_path[0]== CHAR_PATH_SEP && d->m_path.endsWith(HTTP_EXTENSION1)
                && maybeLabel == FIREFOX_PLUGIN_NAME)
            { return true; }
        if(d->m_path[0]== CHAR_PATH_SEP && d->m_path.endsWith(HTTP_EXTENSION2)
                && maybeLabel == FIREFOX_PLUGIN_NAME)
            { return true; }
        if(name == TAGITEM_NAME && maybeLabel == TAG_LABEL)
            { return true; }
        if(name + "://"== maybeLabel)
            {return true;}
        return false;
        //return d->m_data.contains((s));
    }

    QSet<QString> getLabels() const{
        return d->m_data.keys().toSet();
    }


    QString getParentPath() const;

    //Simplistic now, later has parent assign preview URLs
    QUrl previewUrl() const{
        if(hasLabel(FIREFOX_PLUGIN_NAME)){
            return QUrl(d->m_path);
        } else if(hasLabel(FILE_DIRECTORY_PLUGIN_STR)){
            return QUrl(d->m_path);
        }
        return QUrl();
    }

    QString getNameForEditLine(QString keyColor = KEY_COLOR,
                             QString extraAttribs ="",
                             int limit=-1) const{
        return formatStringByKey(getName(),getMatchIndex(),keyColor,extraAttribs, limit );
    }

    bool isHistoryItem() const{
        QString n = d->m_path;
        n.truncate(HISTORY_PREFIX.length());
        //Compare does -1,0,1
        bool isHistory = (getName().compare(HISTORY_PREFIX,Qt::CaseInsensitive)==0);
        //Q_ASSERT(!isHistory || hasLabel((HISTORY_INTERNAL_TIME)));
        return isHistory;
    }

    void clearRelations();

    int getChildRelationCount() const {
        return d->children_detached_list.count();
    }


    void clearTempInformation() {
        d->m_data.remove(TEMPORARY_DESCRIPTION_KEY_STR);
        d->m_data.remove(TEMPORARY_NAME_KEY_STR);
        d->m_data.remove(TEMPORARY_PRIORITY_KEY_STR);
        d->m_data.remove(TEMPORARY_LONG_HTML_KEY_STR);
        d->m_data.remove(ORDER_BY_SIBLING_KEY);
        d->m_data.remove(SIBLING_ORDER_KEY);

        if(d->m_data.contains(NAME_KEY_STR)){
            d->m_name = d->m_data[NAME_KEY_STR];
            d->m_lowName = d->m_name.toLower();
        }
        //this->setIsTempItem(false);
    }

    bool hasChildren() const;
    bool canNavigateTo() const{
        if(getItemType() == CatItem::VERB){
            if(!hasLabel(VERB_TAKES_NO_ARGS_KEY)){
                QList<CatItem> actionTypes = getTypeParents(CatItem::ACTION_TYPE);
                for(int i=0;i<actionTypes.count();i++){
                    if(actionTypes[i].getChildCount()>0){
                        return true;
                    }
                }
            }
            return false;
        }
        if(getItemType() == CatItem::PUBLIC_POST ||
           getItemType() == CatItem::MESSAGE ||
           getItemType() == CatItem::PERSON ||
           getItemType() == CatItem::TAG ||
           getItemType() == CatItem::ORGANIZING_TYPE )
            { return true;}
        //if(hasChildren()){return true;}
        if((int)getTagLevel() > (int)CatItem::ATOMIC_ELEMENT)
            { return true; }
        return false;
    }

    bool canFilterWith() const{
        //only if there are earlier siblings...
        if(getSiblingOrder()>0){
            return true;
        }

        if (getItemType() == CatItem::VERB){
            return true;
        }
        return (bool)getCustomValue(IS_FILTER_ITEM, (int)false);
    }

//    bool indexByKey() const {
//
//        if(d->m_path.startsWith(BUILTIN_PREFIX)){
//            return false;
//        }
//        if(d->m_path.startsWith(USERTEXT_PREFIX)){
//            return false;
//        }
//        if(d->m_path.startsWith(XSLT_PREFIX)){
//            return false;
//        }
//        if(d->m_path.startsWith(ACTION_TYPE_PREFIX)){
//            return false;
//        }
//
//        if(d->m_path.startsWith(HISTORY_PREFIX)){
//            return false;
//        }
//        if(d->m_path.startsWith(PLUGIN_PATH_PREFIX)){
//            return false;
//        }
//        if(d->m_path.startsWith(COMPOUND_ITEM_PREFIX)){
//            return false;
//        }
//
//        if(getCustomValue(DONT_KEY_INDEX)){
//            return false;
//        }
//
//        return true;
//    }

    //void addSavedRelation(DetachedChildRelation cR);
    QList<DetachedChildRelation> getParentRelations() const;
    bool hasParentType(QString pluginId) const;
    QList<CatItem> getCategories() const;
    QList<DetachedChildRelation> getChildRelations() const;
    QList<DetachedChildRelation> getSiblingOrderRels() const;
    QList<DetachedChildRelation> getRelations()  const {  return d->children_detached_list; }

    CatItem getActionTypeChild() const;

    QString getParentChar(QString key);
    CatItem getTypeParent(CatItem::ItemType t) const;
    QList<CatItem> getTypeParents(CatItem::ItemType t) const;
    CatItem getTypeParent(BaseChildRelation::ChildRelType t) const;
    QList<CatItem> getTypeParents(BaseChildRelation::ChildRelType t) const;
    CatItem getActCategory() const;
    bool hasChild(CatItem ci) const;
    bool hasParent(CatItem ci) const;


    void setPluginSourceForChildren(QString pluginName);

    void addSavedRelations(DetachedChildRelation cR);

    void setCustomPluginInfo(QString key, QString str) {
        if(key ==EXTERN_SOURCE_ARG_PATH){
            CatItem p(SOURCE_W_ARG_PATH);
            addParent(p);
        }

        if(key == MIME_TYPE_NAME_KEY){
            setActionType(str);
        } else if(key == PATH_KEY_STR){
            setPath(str);
        } else if(key == NAME_KEY_STR){
            setName(str);
        } else {
            d->m_data[key]= str;
        }
    }

    bool hasCustomVariant(QString key) const{
        return d->variantData.contains(key);
    }

    void setCustomPluginVariant(QString key, QVariant qv) {
        d->variantData[key]= qv;
    }

    QVariant getCustomPluginVariant(QString key)  const {
        return d->variantData[key];
    }

    void setCustomPluginValue(QString key, qint32 i) {
        if(key == POSITIVE_WEIGHT_KEY_STR){
            i = MAX(DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR - i, 1);
            this->setTotalWeight(DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR - i);
        } else if(key == ITEM_CREATION_TIME_KEY_STR){
                setCreationTime(i);
        } else {
            QString str =  QString("%1").arg(i);
            d->m_data[key]= str;
        }

        Q_ASSERT(getCustomValue(key)== i);
    }

    void setCustomValue(QString key, qint32 i) {
        setCustomPluginValue(key, i);
    }

    qint32 getCustomValue(QString key, qint32 defaultValue=0) const {
        if(key == POSITIVE_WEIGHT_KEY_STR){
            return getPositiveTotalWeight();
        } else if(key == ITEM_CREATION_TIME_KEY_STR){
            return getCreationTime();
        }  else if(key == WEIGHT_SECTION_KEY_STR){
            return getWeightSection();
        }

        if(d->m_data.contains(key))
            { return d->m_data[(key)].toInt();}
        else {return defaultValue;}
    }
    QString getCustomString(QString key, QString defaultValue="") const{
        if(key == PATH_KEY_STR || key == "path"){
            return d->m_path;
        } else  if(key == NAME_KEY_STR || key == "name" || key== SHORT_TEXT_NAME){
            return getName();
        } else  if(key == RFC_EMAIL_MESSAGE_NAME || key == "sender" || key =="author"){
            return getAuthor();
        } else  if(key == "emailAddress"){
            return getEmailAddr();
        } else if(key == USER_DESCRIPTION_KEY_STR || key == "description"){
            return getFormattedDescription();
        } else if(key == LONG_TEXT_KEY_STR || key == LONG_TEXT_NAME){
            return getLongText();
        } else if(key == TO_STR ){
            return getCustomString(TO_STR);
        }

        if(d->m_data.contains(key)){
            return d->m_data[key];
        } else {return defaultValue;}
    }




    QString getValueSection() const {

        if(this->hasLabel(TEMP_CHARACTERISTIC_VALUE_KEY)){
            if(this->hasLabel(SCALE_VALUE_MODIFIER)){
                int v = this->getCustomValue(TEMP_CHARACTERISTIC_VALUE_KEY);
                return QString::number(log(v));
            }
            if(d->m_path == TIME_SORT_CATEGORY_ITEM_PATH){
                int v = appGlobalTime() - getCustomValue(TEMP_CHARACTERISTIC_VALUE_KEY);
                return QString::number(log(v));
            }
            return getCustomString(TEMP_CHARACTERISTIC_VALUE_KEY);
        }

        if(this->hasLabel(SCALE_VALUE_MODIFIER)){
            int v = this->getCustomValue(CHARACTERISTIC_VALUE_KEY);
            return QString::number(log(v));
        } else {
            return getCustomString(CHARACTERISTIC_VALUE_KEY);
        }


    }

    CatItem& addChild(CatItem& childItem, QString pluginName="",
                  BaseChildRelation::ChildRelType defaultType=BaseChildRelation::UNDETERMINED_REL);
    CatItem& addParent(CatItem& parentItem, QString pluginName="",
                   BaseChildRelation::ChildRelType defaultType=BaseChildRelation::UNDETERMINED_REL,
                   bool isDefaultAppType= false);

    void addChildren(QList<CatItem> children);
    void addChildrenBulk(QList<CatItem> children);
    void addParents(QList<CatItem> parents);
    static CatItem& addChildrenInternal(CatItem& parentItem, QList<CatItem>& children);
    static CatItem& addParentsInternal(QList<CatItem>& parents, CatItem &childItem);

    void addCharacteristic(QString characteristicName,
            int value,
            BaseChildRelation::ChildRelType relationType = BaseChildRelation::NORMAL);
    void addCharacteristic(QString characteristicName,
            QString valueStr,
            BaseChildRelation::ChildRelType relationType = BaseChildRelation::NORMAL);
    QString getCharacteristic(QString name, QString defaultValue);
    long getCharacteristicValue(QString name, long defaultValue=0);
    CatItem getParentByLabel(QString label);

    QList<CatItem> getCharacteristics();

    CatItem setChildForRemove(CatItem childItem);
//    CatItem& recursivelyAddChildren(CatItem initialItem, QList<QString> l);
    QList<CatItem> getChildren(BaseChildRelation::ChildRelType relLevel=BaseChildRelation::UNDETERMINED_REL);
    QList<CatItem> getParents();

    QList<CatItem> getRelatedItems();
    static QList<CatItem> subChilds(CatItem);
    QList<CatItem> organizingCategoryParents();

    bool isChildStubbed() const { return (bool)getCustomValue(IS_CHILD_STUB_KEY,(int)false);}
    void setChildStubbed(bool b=true){ setCustomPluginValue(IS_CHILD_STUB_KEY,(int)b);}

    int getChildCount() const;

    void setNounChildCount(int c){ setCustomPluginValue(CHILD_COUNT_STUBBED_KEY,c);}

    CatItem getChild (int i);
    CatItem getChildById(qint32 i);


    bool merge(CatItem s, bool thisIsOriginal=false){
        mergeItem(s, false, thisIsOriginal);
        return true;
    }

    //Weighting and weighing based functions
    bool isCategorizingSource() const {
        if(hasLabel(IS_CATEGORING_SOURCE_KEY)){
            return (bool)getCustomValue(IS_CATEGORING_SOURCE_KEY);
        }
        if(hasLabel(IS_INTERMEDIATE_SOURCE_KEY)){return false; }
        if(hasLabel(FIREFOX_PLUGIN_NAME) &&
           hasLabel(STREAM_SOURCE_PATH)){ return true; }
        if( getItemType() == CatItem::LOCAL_DATA_DOCUMENT
           || getItemType() == CatItem::OPERATION){
            return false;
        }

        if(getItemType()==CatItem::LOCAL_DATA_FOLDER){return true;}

        //if(hasSourceWeight()){ return true; }

        return (int)getTagLevel() > (int)INTERNAL_STATIC_SOURCE;
    }

    bool isUserItem(){

        if(!getDescription().isEmpty()){ return true;}
        if(this->getItemType()== CatItem::PERSON){ return true;}

        if(d->m_path.startsWith(BUILTIN_PREFIX)){return false;}
        if(d->m_path.startsWith(VALUE_PREFIX)){return false;}
        if(d->m_path.startsWith(HISTORY_PREFIX)){return false;}
        if(d->m_path.startsWith(COMPOUND_ITEM_PREFIX)){return false;}
        if(d->m_path.startsWith(COMPOUND_FILE_PREFIX)){return false;}
        if(d->m_path.startsWith(ACTION_TYPE_PREFIX)){return false;}
        if(d->m_path.startsWith(ACTION_NAME)){return false;}
        if(d->m_path.startsWith(TYPE_PREFIX)){return false;}
        if(d->m_path.startsWith(XSLT_SOURCE_PREFIX)){return false;}
        if(d->m_path.startsWith(XSLT_PREFIX)){return false;}
        if(d->m_path.startsWith(ITEM_SYNONYM_PATH)){return false;}
        if(d->m_path.startsWith(SYNONYM_PREFIX)){return false;}

        switch( getTagLevel()){
            case  BASE_PLUGIN: return false;
            case INTERNAL_STATIC_SOURCE: return false;
            case INTERNAL_SOURCE: return false;
            case EXTERNAL_SOURCE: return false;
            default: return true;
        }
    }

    bool isUpdatableSource(){ return (bool)getCustomValue(UPDATEABLE_SOURCE_KEY);}
    void setIsUpdatableSource(bool b=true){
        setCustomPluginValue(UPDATEABLE_SOURCE_KEY,(int)b);
    }

    time_t getSourceUpdateTime(){ return (time_t)getCustomValue(SOURCE_UPDATE_TIME_KEY,0);}
    void setSourceUpdateTime(time_t b){ setCustomPluginValue(SOURCE_UPDATE_TIME_KEY,(int)b);}

    bool supportsSourceUpdateTime(){ return (time_t)getCustomValue(SOURCE_UPDATE_TIME_KEY,0);}
    void setSupportsSourceUpdateTime(time_t b){ setCustomPluginValue(SOURCE_UPDATE_TIME_KEY,(int)b);}




    QList<CatItem> getSearchSourceParents();
    CatItem getUpdateSourceParent(qint32 w = 0);
    QList<CatItem> getSourceParents();

    qint32 getWeightFromSources();
    void setSourceWeight(qint32 v, CatItem par);

    qint32 getUpdatingSourceWeight();
    void setUpdatingSourceWeight(qint32 v, CatItem par);



    void setExternalWeight(long i, CatItem par);
    long getExternalWeight(qint32 parentId=0);
    CatItem getWeightParent();

    long getTotalExternalWeight();
    void clearExternalWeight();
    void setExternalWeightFromTime(time_t i, CatItem par){

        qint32 parentId = par.getItemId();

        time_t now = appGlobalTime();
        time_t from_now = MAX(now-i,1); //avoid taking ln(0) later...

        // ln(60* ONE_DAY_IN_SECONDS) = 14.767940336
        //
        // We're creating "zones" from now, scaled exponentially from now onward
        // Approximately ...
        // 60 - 30 days = MAX_EXTERNAL_WEIGHT/15
        // 30 - 15 days = MAX_EXTERNAL_WEIGHT*2/15
        // 15 - 7.5 days = MAX_EXTERNAL_WEIGHT*3/15
        // Etc
        qint32 scaledNow = -VERY_HIGHT_EXTERNAL_WEIGHT * (MAX(log(from_now/(60* ONE_DAY_IN_SECONDS)), 0)/15);
        qint32 val =MAX(scaledNow, getExternalWeight(parentId));
        setExternalWeight(val, par);
    }

    long getTimedExternalWeight(){
        CatItem p = CatItem(getParentPath());
        time_t t = this->getCreationTime();
        setExternalWeightFromTime(t,p);
        return getExternalWeight();
    }

    long getFullWeight(){
        return getTotalWeight();
    }

    int getWeightSection() const {
        return (int)log2(getTotalWeight());
    }


    qint32 getSourceWeightTics(){
        long weight=0;
        weight=getSourceWeight();
        Q_ASSERT(MAX_MAX_FULL_WEIGHT>weight);
        double maxWeightSection = log2(MAX_MAX_FULL_WEIGHT);
        double baseWeightSection = log2(weight);
        int tics = (baseWeightSection*WEIGHT_TICS)/maxWeightSection;
        return tics;
    }

    void setWeightTics(int i){
        if(i==getSourceWeightTics()){ return; }
        double maxWeightSection = log2(MAX_MAX_FULL_WEIGHT);
        double baseWeightSection = (i*maxWeightSection)/WEIGHT_TICS;
        qint32 weight = pow(2,(int)(baseWeightSection+0.3));
        weight = MAX(weight, 1);
        weight = MIN(weight, MAX_MAX_FULL_WEIGHT);
        setSourceWeight(weight);
        qint32 newW = getSourceWeight();
        newW = newW;
        Q_ASSERT(newW==weight);
        Q_ASSERT(abs((long long)(((log2(weight)*WEIGHT_TICS)/maxWeightSection)-i))<=1);
        int tic = getSourceWeightTics();
        tic = tic;
        Q_ASSERT(abs(tic-i)<=1);
    }

    qint32 getUsage(){
        return getCustomValue(USAGE_KEY_STR,USAGE_DEFAULT);
    }
    void setUsage(qint32 v){
        setCustomPluginValue(USAGE_KEY_STR,v);
    }

    qint32 getRelevanceWeight(){
        return getCustomValue(RELEVANCE_WEIGHT_KEY_STR,RELEVANCE_WEIGHT_DEFAULT);
    }

    void setRelevanceWeight(qint32 v){
        setCustomPluginValue(RELEVANCE_WEIGHT_KEY_STR,v);
    }

    qint32 getTotalWeight() const{
        return getCustomValue(TOTAL_WEIGHT_KEY_STR,TOTAL_WEIGHT_DEFAULT);
    }

    qint32 getPositiveTotalWeight() const{
        qint32 tw = getTotalWeight();
        return MAX((DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR - tw),0);
    }

    void setTotalWeight(qint32 v){
        Q_ASSERT(v >=0);
        setCustomPluginValue(TOTAL_WEIGHT_KEY_STR,v);
    }

    qint32 getSourceWeight(){
        if(hasLabel(SOURCE_WEIGHT_KEY_STR)){
            return getCustomValue(SOURCE_WEIGHT_KEY_STR,SOURCE_WEIGHT_DEFAULT);
        }
        if(isASource()){
            return SOURCE_WEIGHT_DEFAULT;
        }
        return 0;

    }
    void setSourceWeight(qint32 v){
        setCustomPluginValue(SOURCE_WEIGHT_KEY_STR,v);
    }

    bool isASource() const {
        if(getItemType() == CatItem::LOCAL_DATA_FOLDER
           || getItemType() == CatItem::PERSON
           || getItemType() == CatItem::ORGANIZING_TYPE
           || hasLabel(STREAM_SOURCE_PATH)
           || hasLabel(SOURCE_WEIGHT_KEY_STR)){
            return true;
        }
        return false;
    }

    bool hasWeightInfo(){
        if(hasLabel(RELEVANCE_WEIGHT_KEY_STR)){
            return false;
        }
        if(hasLabel(SOURCE_WEIGHT_KEY_STR)){
            return false;
        }
        long w = this->getExternalWeight();
        if(w == MEDIUM_EXTERNAL_WEIGHT){
            return true;
        }
        return false;
    }


    qint32 getChosenness(){
        return getCustomValue(CHOSENNESS_KEY_STR,CHOSENNESS_DEFAULT);
    }
    void setChosenness(qint32 v){
        setCustomPluginValue(CHOSENNESS_KEY_STR,v);
    }

    qint32 getVisibility(){
        return getCustomValue(VISIBILITY_KEY_STR,VISIBILITY_DEFAULT);
    }

    void setVisibility(qint32 v){
        setCustomPluginValue(VISIBILITY_KEY_STR, v);
    }

    time_t getRelevanceTime() const {
        if(hasLabel(MODIFICATION_TIME_KEY))
            {return (time_t)getCustomValue(MODIFICATION_TIME_KEY,(time_t)0);}
        return d->m_creation_time;
    }

    time_t getCreationTime() const {
        return MAX(0,d->m_creation_time);
    }

    void setCreationTime(time_t  v, bool explicitlySet=true){
        if(explicitlySet){
            setLabel(TIME_EXPLICITELY_SET_KEY_STR);
        }
        d->m_creation_time = MAX(v,0);
    }

    time_t getModificationTime() const {
        time_t t = (time_t)getCustomValue(MODIFICATION_TIME_KEY,(time_t)0);
        t = MAX(t,0);
        return t;
    }
    void setModificationTime(time_t  v){
        v = MAX(v,0);
        setCustomPluginValue(MODIFICATION_TIME_KEY,v);
    }

    time_t getChildScanTime() const {
        return (time_t)getCustomValue(CHILD_SCAN_TIME_KEY,(time_t)0);
    }
    void setChildScanTime(time_t  v){
        setCustomPluginValue(CHILD_SCAN_TIME_KEY,v);
    }

    bool isUnseenItem(){
        return(isTimelyMessage()
            && !getCustomValue(ITEM_SEEN_KEY,(int)false));
    }
    void setSeen(bool b=true){
        if(!b){
            this->setIsTimeDependant();
        }
        setCustomValue(ITEM_SEEN_KEY,(int)b);
    }

    int getUnseenChildren(){
        return( getCustomValue(UNSEEN_CHILDREN_KEY,0));
    }
    void setUnseenChildren(int b){
        setCustomValue(UNSEEN_CHILDREN_KEY,b);
    }

    void assign_from(const CatItem &s){
        d = s.d;
    }

    bool matches_exactly(CatItem s){
        s = s;
        Q_ASSERT(d->m_path == s.d->m_path);
        Q_ASSERT(this->getDescription()== s.getDescription());
        Q_ASSERT(getName()== s.getName());
        Q_ASSERT(getIcon()== s.getIcon());
        Q_ASSERT(this->getItemId()== s.getItemId());
        //Q_ASSERT(actionTypeId == s.actionTypeId);
        Q_ASSERT(this->getIsAction()== s.getIsAction());
        //Q_ASSERT(defaultChildActionIndex == s.defaultChildActionIndex);
        Q_ASSERT(this->getItemType()== s.getItemType());
        Q_ASSERT(getMatchIndex() == s.getMatchIndex());
        Q_ASSERT(getTagLevel()==s.getTagLevel());
        Q_ASSERT(this->getUseDescription()== s.getUseDescription());
        Q_ASSERT(this->getTakesAnykeys()== s.getTakesAnykeys());
        Q_ASSERT(getUsage()==s.getUsage());
        //Q_ASSERT(relevanceWeight==s.relevanceWeight);
        //Q_ASSERT(totalWeight==s.totalWeight);
        Q_ASSERT(getWeightFromSources()==s.getWeightFromSources());
        //Q_ASSERT(chosenness==s.chosenness);
        Q_ASSERT(getVisibility()==s.getVisibility());

        //has stuff that won't equal
        //Q_ASSERT(data == s.data);
        return true;
    }

    TagLevel getTagLevel() const{
        if(getCustomValue(TAG_LEVEL_KEY_STR,(int)TAG_LEVEL_DEFAULT) !=(int)TAG_LEVEL_DEFAULT){
            return (TagLevel) getCustomValue(TAG_LEVEL_KEY_STR);
        }

        if(this->getItemType() == CatItem::VERB)
            { return CatItem::ATOMIC_ELEMENT; }

        if(this->getItemType() == CatItem::LOCAL_DATA_FOLDER){
            return CatItem::POSSIBLE_TAG; }

        if(this->getItemType() == CatItem::ORGANIZING_TYPE){
            return CatItem::POSSIBLE_TAG; }

        if(this->getItemType() == CatItem::ACTION_TYPE){
            return CatItem::ACTION_TAG; }

        if(this->getItemType() == CatItem::PLUGIN_TYPE){
            return CatItem::BASE_PLUGIN; }

        if(this->getItemType() == CatItem::TAG)
            { return CatItem::KEY_WORD; }
        return TAG_LEVEL_DEFAULT;
    }

    void setTagLevel(TagLevel v){
        setCustomPluginValue(TAG_LEVEL_KEY_STR, (int) v);
    }

    ActionType getActionType() const {
        if( getItemType() ==VERB){
            return VERB_ACTION;
        }
        if((int)getItemType() ==(int)OPERATION){
            return OPERATION_ACTION;
        }
        return NOUN_ACTION;
    }

    QString getActionParentType() const {
        QString actionParName;
        CatItem par = getTypeParent(CatItem::ACTION_TYPE);
        actionParName = par.getName();
        return actionParName;
    }

    QString getActionParentDescription() const {
        QString actionParName;
        CatItem par = getTypeParent(CatItem::ACTION_TYPE);
        actionParName = par.getDescription();
        return actionParName;
    }



    bool getUsePath(){
        return ((getActionType()== CatItem::NOUN_ACTION)
                && !(this->getUseDescription()));
    }

//    void setItemType(ItemType it){
//        itemType = it;
//        if(itemType == CatItem::TAG){
//            tagLevel = KEY_WORD;
//        } else if((int)itemType < (int)CatItem::LOCAL_DATA_DOCUMENT){
//            tagLevel = POSSIBLE_TAG;
//        }
//    }


    void setIsTimeDependant(bool it=true){
        (setCustomPluginValue(IS_TIMELY_ITEM_KEY_STR,(int)it));
    }

    bool getIsTimeDependant(){
        if(this->getItemType() ==CatItem::MESSAGE){
            return true;
        }
        return (bool)getCustomValue(IS_TIMELY_ITEM_KEY_STR, 0);
    }

    void setIsIntermediateSource(bool it=true){
        (setCustomPluginValue(IS_INTERMEDIATE_SOURCE_KEY,(int)it));
    }

    bool getIsIntermediateSource(){
        return (bool)getCustomValue(IS_INTERMEDIATE_SOURCE_KEY, 0);
    }

    void setCustomCommandLine(QString s){
        (setCustomPluginInfo(VERB_COMMAND_STRING_KEY,s));
    }

    CatItem getItemDefaultVerb(){
        if(this->getItemType() ==CatItem::VERB){
            return *this;
        }
        CatItem par = getTypeParent(BaseChildRelation::DATA_CATEGORY_PARENT);
        CatItem act = par.getActionTypeChild();
        return act;
    }

    CatItem getActionParent(){
        if(this->getItemType() ==CatItem::VERB){
            CatItem par = getTypeParent(BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM);
            if(!par.isEmpty()){ return par; }
            par = getTypeParent(BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM);
            if(!par.isEmpty()){ return par; }
            par = getTypeParent(BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD);
            if(!par.isEmpty()){ return par; }
            par = getTypeParent(BaseChildRelation::OPTIONAL_ACTION_CHILD);
            return par;
        }
        CatItem par = getTypeParent(BaseChildRelation::DATA_CATEGORY_PARENT);
        return par;
    }

    QList<CatItem> getActionParents(){
        QList<CatItem> pars;
        if(this->getItemType() ==CatItem::VERB){
            pars = getTypeParents(BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM);
            if(!pars.isEmpty()){ return pars; }
            pars = getTypeParents(BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM);
            if(!pars.isEmpty()){ return pars; }
            pars = getTypeParents(BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD);
            if(!pars.isEmpty()){ return pars; }
            pars = getTypeParents(BaseChildRelation::OPTIONAL_ACTION_CHILD);
            return pars;
        }
        pars = getTypeParents(BaseChildRelation::DATA_CATEGORY_PARENT);
        return pars;
    }

    QList<QString> getActionParentTypes(){
        QList<CatItem> pars;
        QList<QString> res;
        if(this->getItemType() ==CatItem::VERB){
            pars = getTypeParents(BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM);
            if(pars.isEmpty())
                { pars = getTypeParents(BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM);}
            if(pars.isEmpty())
                { pars = getTypeParents(BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD); }

            if(pars.isEmpty())
                { pars = getTypeParents(BaseChildRelation::OPTIONAL_ACTION_CHILD); }

        } else {
            pars = getTypeParents(BaseChildRelation::DATA_CATEGORY_PARENT);
        }
        for(int i=0; i<pars.count(); i++){
            res.append(pars[i].getName());
        }
        return res;
    }

    QList<CatItem> getAssociatedNormalVerbs();
    QList<CatItem> getDataTypesAssocWVerb();

    QList<CatItem> getAllActionTypeParents();

    QList<CatItem> getVerbActionCategories(){
        QList<CatItem> res;
        CatItem par = getTypeParent(BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD);
        res.append(par);
        par = getTypeParent(BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM);
        res.append(par);
        par = getTypeParent(BaseChildRelation::REPLACEMENT_DEFAULT_ACTION_ITEM);
        res.append(par);
        return res;
    }

    QString getCommandLine(){
        if(!getCustomCommandLine().isEmpty()){
            return getCustomCommandLine();
        }
        if(this->getItemType() ==CatItem::VERB){
            return d->m_path;
        }
        CatItem act = getItemDefaultVerb();
        QString cmd;
        if(act.hasLabel(VERB_COMMAND_STRING_KEY)){
            cmd = act.getCustomCommandLine() ;
        } else {
            cmd = act.getArgPath();
        }
        if(cmd.isEmpty()){
            return " ";
        }
        cmd += " ";

        QString arg = getArgPath();
        if(arg.simplified().contains(" ")){
            cmd += QString("\"")+ getArgPath() + QString("\"");
        } else {
            cmd += getArgPath();
        }
        return cmd;
    }

    QString getCustomCommandLine(){
        return getCustomString(VERB_COMMAND_STRING_KEY);
    }

    QString getFillinGenerator(){
        return getCustomString(FILLIN_GENERATOR_KEY);
    }
    void setFillinGenerator(QString k){
        return setCustomPluginInfo(FILLIN_GENERATOR_KEY, k);
    }

    QString getLocalStoragePath(){
        return getCustomString(LOCALLY_STORED_AT_KEY);
    }
    void setLocalStoragePath(QString k){
        return setCustomPluginInfo(LOCALLY_STORED_AT_KEY, k);
    }

    QList<CatItem> getCustomArguments();
    QList<CatItem> getRequiredCustomArguments();

    void addArgumentsToRelations();


    void setArgType(QString v){
        Q_ASSERT(!v.isEmpty());
        setCustomPluginInfo(ARG_TYPE_KEY_STR,v);
    }

    QString getArgType(){
        return getCustomString(ARG_TYPE_KEY_STR,"").trimmed();
    }

    void setArgValue(QString v){
        setCustomPluginInfo(ARG_VALUE_KEY_STR,v);
    }

    QString getArgValue(){
        return getCustomString(ARG_VALUE_KEY_STR).trimmed();
    }


    void setIsVerbArg(bool v){
        setCustomPluginValue(IS_VERB_ARG_KEY_STR, (bool) v);
    }

    bool getIsVerbArg() const{
        return (bool) getCustomValue(IS_VERB_ARG_KEY_STR,(int)false);
    }

    CatItem getArgTypeItem(){
        QString typeName;
        if(hasLabel(ARG_TYPE_KEY_STR)){
            typeName =  getArgType();
        } else {
            typeName = getName();
        }
        return createActionItem(typeName);
    }


    //More custom verb support
    void setArgNumber(int i) {
        setCustomPluginValue(ORDER_OF_ARG,i);
    }

    int getArgNumber() {
        int argNum = getCustomValue(ORDER_OF_ARG,-1);
        return argNum;
    }

    void setArgName(QString n) {
        setCustomPluginInfo(NAME_OF_ARG,n);
    }

    QString getArgName() {
        return getCustomString(NAME_OF_ARG);
    }

    QString argDefaultString(){
        QString res = getCustomString(ARG_DEFAULT_STRING);
        if(!res.isEmpty()){
            return res;
        }
        return getCustomString(ARG_SPEC_STRING);
    }

    QString getStaticArgPath(){
        QString res = getCustomString(STATIC_ARG_KEY);
        if(!res.isEmpty()){ return res; }
        res = getCustomString(STATIC_FILE_KEY);
        if(!res.isEmpty()){ return res;}
        return d->m_path;
    }

//    //The argSig is either entirely within the required arguments
//    //or covers it entirely
//    bool completeMatchArgSig(QList<CatItem> argSig){
//
//        QList<CatItem> argList = getCustomArguments();
//
//        Q_ASSERT(argList.length()>0);
//        bool unmatchedItem = false;
//        for(int i=0; i< argList.length(); i++){
//            if(argSig.length() == 0){
//                if((bool)argList[i].getCustomValue(IS_REQUIRED_ARG_KEY_STR)) {
//                    return false;
//                } else { continue;}
//            }
//            QString argT = argList[i].getArgType();
//            qDebug() << "Type searching for:" << argT;
//            CatItem it = CatItem::createActionItem(argT);
//
//            int a = argSig.indexOf(it);
//            if(a >-1){
//                argSig.removeAt(a);
//            } else {
//                if((bool)argList[i].getCustomValue(IS_REQUIRED_ARG_KEY_STR))
//                    {unmatchedItem = true;}
//            }
//        }
//        return !unmatchedItem;
//    }
//
//    bool matchArgSig(QList<CatItem> argSig){
//        QList<CatItem> args = getCustomArguments();
//
//
//        if(args.length()==0){return false;};
//        bool unmatchedItem = false;
//        for(int i=0; i< args.length(); i++){
//
//            int a = argSig.indexOf(CatItem(args[i].getArgType()));
//            if(a >-1){
//                argSig.removeAt(a);
//            } else {
//                if((bool)args[i].getCustomValue(IS_REQUIRED_ARG_KEY_STR))
//                    {unmatchedItem = true;}
//            }
//            if(argSig.length() == 0){
//                return true;
//            }
//        }
//        return !unmatchedItem;
//
//    }


    bool within(ItemType endFilterType, ItemType startFilterType=CatItem::MIN_TYPE){
        return (int)getItemType() >= (int)startFilterType &&((int)getItemType() <= endFilterType);
    }

    QString getLongText() const {
        if(d->m_data.contains(LONG_TEXT_KEY_STR)){
            return d->m_data[LONG_TEXT_KEY_STR];
        }
        if(d->m_data.contains(DESCRIPTION_KEY_STR)){
            return d->m_data[DESCRIPTION_KEY_STR];
        }
        return "";
    }

    void setLongText(QString v){
        setCustomPluginInfo(LONG_TEXT_KEY_STR,v);
    }

    QString getDescription() const {
        if(d->m_data.contains(TEMPORARY_DESCRIPTION_KEY_STR)){
            return d->m_data[TEMPORARY_DESCRIPTION_KEY_STR];
        }
        if(d->m_data.contains(DESCRIPTION_KEY_STR)){
            return d->m_data[DESCRIPTION_KEY_STR];
        }
        return "";
    }

    void setDescription(QString v){
        if(d->m_path==getName()){
            setName(v);
        }
        setCustomPluginInfo(DESCRIPTION_KEY_STR,v);
    }

    void setAltDescription(QString v){
        setCustomPluginInfo(ALT_DESCRIPTION_KEY_STR,v);
    }

    QString getAltDescription() {
        if(this->hasLabel(TEMPORARY_DESCRIPTION_KEY_STR))
            return getCustomString(TEMPORARY_DESCRIPTION_KEY_STR);
        if(this->hasLabel(ALT_DESCRIPTION_KEY_STR))
            return getCustomString(ALT_DESCRIPTION_KEY_STR);
        if(this->hasLabel(DESCRIPTION_KEY_STR))
            return getCustomString(ALT_DESCRIPTION_KEY_STR);

        QString res = getActionParentType() + " ";

        if(this->hasLabel(FILE_CATALOG_PLUGIN_STR)){
            long size = getCharacteristicValue(FILE_SIZE_CHARACTERISTIC_KEY);
            if(size < 1000){
                res += QString::number(size) + " bits";
            } else  if(size < 1000*1000){
                res += QString::number(size/1000) + " k";
            } else{
                res += QString::number(size/(1000*1000)) + " mb";
            }

            QString mimeType = getMimeDescription();
            mimeType.truncate(20);
            if(!mimeType.isEmpty()){
                res += mimeType + " ";
            }

            time_t t;
            QDateTime d;
            if(hasLabel(MODIFICATION_TIME_KEY)){
                t = getModificationTime();
            } else {
                t = getCreationTime();
            }
            d.fromTime_t(t);
            res += (QString(" ") + contextualTimeString(d));
            return res;
        }
        return "";
    }

    QString getName() const{
        if(d->m_data.contains(TEMPORARY_NAME_KEY_STR) && (!d->m_data[TEMPORARY_NAME_KEY_STR].isEmpty())){
            return d->m_data[TEMPORARY_NAME_KEY_STR];
        }

        if (!d->m_name.isEmpty()){
            return d->m_name;
        }

        if(getUseDescription()){
            getDescription();
        }

        if(!d->m_data.contains(NAME_KEY_STR)){
            return d->m_path;
        }
        QString n = d->m_data[NAME_KEY_STR];
        Q_ASSERT(!n.isEmpty());
        return n;
    }

    bool hasRealName(){
        return d->m_name.length()>0 && d->m_data.contains(NAME_KEY_STR);
    }

    QString getActionName() const;

    QString getArgPath() const;


    void adjustChildrensPath(QString newPath);

    QString getPath() const{return d->m_path;}

    QString getFormattedDescription() const{
        if(!getEmailAddr().isEmpty()){
            QString messageStr = (FROM_PERSON__PHRASE + getEmailAddr()).simplified();
            if(this->hasLabel(TIME_EXPLICITELY_SET_KEY_STR)){
                //contextualTimeString
                time_t t = getCreationTime();
                //QDateTime d(t);
                QDateTime d;
                d.setTime_t(t);
                messageStr = messageStr + " " + contextualTimeString(d);
            }
            return messageStr;

        } else if(this->getUseDescription()){
            QString descript = getDescription();
            if(!descript.isEmpty()){
                return descript;
            } else {
                return getName();
            }
        } else if(this->getTagLevel()== CatItem::KEY_WORD){
            return TAG_ITEM_LABEL + ": " + getName();
        } else if(this->getTagLevel()== CatItem::KEY_WORD){
            return TAG_ITEM_LABEL + ": " + getName();
        } else if(getItemType() == CatItem::VERB){
            return VERB_LABEL;
        } else {
            return d->m_path;
        }
    }

    QString getId(){return d->m_path;}
    QString getLowName() const {
        Q_ASSERT(!d->m_lowName.isEmpty() || d->m_name.isEmpty());
        if(!d->m_lowName.isEmpty()){
            return d->m_lowName;
        } else {
            return d->m_data[TEMP_LOW_PATH_KEY_STR];
        }
    }

    CatItem getAccountItem(){
        CatItem it = getTypeParent(BaseChildRelation::ACCOUNT_SOURCE_PARENT);
        if(!it.isEmpty()){ return it; }

        QString accountPath = this->getCustomString(ITEM_ACCOUNT_PATH_KEY);
        if(accountPath.isEmpty()){ return CatItem();}
        CatItem acct(accountPath);
        acct.setStub(true);
        return acct;
    }

    static CatItem createKeywordItem(QString keyword){

        QString kwText = keyword.toLower().simplified();
        CatItem kwItem((KEYWORD_PREFIX + kwText), kwText);
        Q_ASSERT(kwItem.getName() == kwText);
        kwItem.setTagLevel(CatItem::KEY_WORD);
        kwItem.setItemType(CatItem::TAG);
        kwItem.setIcon(TAG_ICON_NAME);
        kwItem.setLabel(FULL_TEXT_PLUGIN_NAME);
        return kwItem;
    }



    void setAccountItem(QString accountPath){
        setCustomPluginInfo(ITEM_ACCOUNT_PATH_KEY,accountPath);
    }

    //-----------------------more generic getters and setters, Hash Based----------


    QString getTemporaryName(){
        if(!d->m_data.contains(NAME_KEY_STR)){return "";}
        QString n = d->m_data[TEMPORARY_NAME_KEY_STR];
        Q_ASSERT(!n.isEmpty());
        return n;
    }

    void setTemporaryName(QString v){
        Q_ASSERT(!v.isEmpty());
        setCustomPluginInfo(TEMPORARY_NAME_KEY_STR,v);
    }

    QString getPrefixName(){
        if(!d->m_data.contains(PREFIX_NAME_KEY_STR)){
            QStringList sl = getName().split(" ");
            if(sl.count()>0){
                return sl[0];
            } else { return ""; }
        }
        return this->getCustomString(PREFIX_NAME_KEY_STR);
    }

    void setPrefixName(QString v){
        Q_ASSERT(!v.isEmpty());
        setCustomPluginInfo(TEMPORARY_NAME_KEY_STR,v);
    }

    QString getTemporaryDescription(){
        if(!d->m_data.contains(TEMPORARY_DESCRIPTION_KEY_STR)){return "";}
        return d->m_data[TEMPORARY_DESCRIPTION_KEY_STR];
    }
    void setTemporaryDescription(QString v){
        d->m_data[TEMPORARY_DESCRIPTION_KEY_STR] = v;
    }

    QString getTemporaryLongHtml(){
        if(!d->m_data.contains(TEMPORARY_LONG_HTML_KEY_STR)){return "";}
        return d->m_data[TEMPORARY_LONG_HTML_KEY_STR];
    }
    void setTemporaryLongHtml(QString v){
        d->m_data[TEMPORARY_LONG_HTML_KEY_STR] = v;
    }

    bool getUseDescription() const {
        if(getItemType() == CatItem::VERB &&
           !getDescription().isEmpty()){
            return true;
        }
        if(getIsAction()){
            return true;
        }
        return (bool)getCustomValue(USE_DESCRIPTION_KEY_STR, (int)USE_DESCRIPTION_DEFAULT);
    }

    void setUseDescription(bool v){
        setCustomPluginValue(USE_DESCRIPTION_KEY_STR,(int)v);
    }

    bool getUseLongDescription() const {
        return (bool)getCustomValue(USE_DESCRIPTION_LONG_KEY_STR, (int)false);
    }

    void setUseLongDescription(bool v){
        setCustomPluginValue(USE_DESCRIPTION_LONG_KEY_STR,(int)v);
    }

    bool getUseTypeDescription() const {
        return (bool)getCustomValue(USE_TYPE_DESCRIPTION_KEY_STR, (int)false);
    }

    void setUseTypeDescription(bool v){
        setCustomPluginValue(USE_TYPE_DESCRIPTION_KEY_STR,(int)v);
    }

    bool getUseLongName() const {
        return (bool)getCustomValue(USE_LONG_NAME_KEY_STR, (int)false);
    }
    void setUseLongName(bool v=true){
        setCustomPluginValue(USE_LONG_NAME_KEY_STR,(int)v);
    }

    bool getIsSubordinateItem() const {
        return (bool)getCustomValue(IS_SUBORDINATE_KEY, (int)false);
    }
    void setIsSubordinateItem(bool v=true){
        setCustomPluginValue(IS_SUBORDINATE_KEY,(int)v);
    }

    void setDistinguished(DistinguishedType d=CatItem::HIGH_VALUE){
        setCustomPluginValue(DISTINGUISH_KEY_STR,(int)d);
    }

    DistinguishedType getDistinguishedType(){
        return (DistinguishedType)getCustomValue(DISTINGUISH_KEY_STR);
    }

    void setSortingType(CatItem::ItemType t){
        setCustomPluginValue(SORTING_TYPE_KEY, (int)t);
    }

    CatItem::ItemType getSortingType(){
        if(hasLabel(SORTING_TYPE_KEY)){
            return (CatItem::ItemType)getCustomValue(SORTING_TYPE_KEY);
        } else {
            QStringList sl = d->m_path.split("://");
            if(sl.count()>1){
                QString body = sl[1];
                bool ok=false;
                int typeNum = body.toInt(&ok);
                if(typeNum!=0 && ok){
                    CatItem::ItemType type = (CatItem::ItemType)typeNum;
                    return type;
                }
            }
            return getItemType();
        }

    }

    bool getTakesAnykeys(){
        return (bool)getCustomValue(TAKES_ANY_KEYS_KEY_STR,(int)TAKES_ANY_KEYS_DEFAULT);
    }

    void setTakesAnykeys(bool v){
        setCustomPluginValue(TAKES_ANY_KEYS_KEY_STR,(int)v);
    }

    bool getIsTempItem(){
        return (bool)getCustomValue(IS_TEMP_KEY_STR,(int)IS_TEMP_DEFAULT);
    }
    void setIsTempItem(bool v){
        setCustomPluginValue(IS_TEMP_KEY_STR,(int)v);
    }


    MatchType getBestMatchType(){
        return (MatchType)getCustomValue(BEST_MATCH_TYPE,(int)NOT_MATCHED);
    }

    void setBestMatchType(MatchType v){
        MatchType mt = (MatchType)getCustomValue(BEST_MATCH_TYPE,(int)NOT_MATCHED);
        if((int)mt < (int)v){
            setCustomPluginValue(BEST_MATCH_TYPE,(int)v);
        }
    }

    MatchType getMatchType(){
        return d->m_matchType;
    }

    void setMatchType(MatchType v){
        d->m_matchType = v;
    }

    long getItemId() const{
        return stringHash(d->m_path);
    }

    QString getGeneratingSettings(){
//        QString file = getCustomString(GENERATING_SETTINGS_FILE_KEY);
//        if(!file.isEmpty()){
//            return file;
//        }
        return getCustomString(FILLIN_GEN_FILE_KEY);
    }

    void setGeneratingSettings(QString s){
        setCustomPluginInfo(FILLIN_GEN_FILE_KEY, s);
    }



    ItemType getItemType() const{
        if(d->m_data.contains(ITEM_TYPE_KEY_STR)){
            return (ItemType)getCustomValue(ITEM_TYPE_KEY_STR);
        }
        if(d->m_path.startsWith(SHELL_VERB_PREFIX)){
            return CatItem::VERB;
        }
        if(d->m_path.startsWith(ACTION_TYPE_DEF)){
            return CatItem::ACTION_TYPE;
        }
        if(d->m_path.startsWith(TYPE_PREFIX)){
            return CatItem::ORGANIZING_TYPE;
        }
        if(d->m_path.startsWith(OPERATION_PREFIX)){
            return CatItem::OPERATION;
        }
        if(this->hasLabel(IS_OPERATION_KEY_STR)){
            return CatItem::OPERATION;
        }
        if(this->hasLabel(FIREFOX_PLUGIN_NAME) &&
           !hasLabel(FILE_CATALOG_PLUGIN_STR)){
            return CatItem::PUBLIC_DOCUMENT;
        }
        if(this->hasLabel(KEYWORD_NAME)){
            return CatItem::TAG;
        }
        return ITEM_TYPE_DEFAULT;
    }

    FilterRole getFilterRole() const {
        return( (FilterRole)getCustomValue(FILTER_ROLE_KEY,CatItem::UNDEFINED_ELEMENT));
    }

    void setFilterRole(FilterRole b){
        setCustomValue(FILTER_ROLE_KEY,b);
    }

    bool isTimelyMessage() const {
        return ( getCustomValue(ITEM_TYPE_KEY_STR,0) >= CatItem::MESSAGE )
                    &&
               ( getCustomValue(ITEM_TYPE_KEY_STR,0) < CatItem::VALUE    );
    }

    void setItemType(ItemType v){
//        if(v == CatItem::TAG){
//            setTagLevel(KEY_WORD);
//        }
//        else if((int)v < (int)CatItem::LOCAL_DATA_DOCUMENT){
//            setTagLevel(POSSIBLE_TAG);
//        }
        setCustomPluginValue(ITEM_TYPE_KEY_STR,v);
    }

    ItemType getOrganizeingType() const{
        return (ItemType)this->getCustomValue(ORGANIZING_TYPE_KEY, (int)MIN_TYPE);
    }
    void setOrganizingType(ItemType v){
        setCustomPluginValue(ORGANIZING_TYPE_KEY,(int)v);
    }

    static bool matchOrganizingTypes( ItemType organizingType, ItemType itemType);

    QList<ItemType> getOrganizingTypeList(){
        ItemType organizingType = this->getOrganizeingType();
        return getOrganizingTypeList(organizingType);
    }

    static QList<ItemType> getOrganizingTypeList(ItemType organizingType){
        QList<ItemType> res;
        if(organizingType==CatItem::MIN_TYPE ){
            return res;
        }
        res.append(organizingType);
        if(organizingType == CatItem::LOCAL){
            res.append(CatItem::VERB);
            res.append(CatItem::LOCAL_DATA_DOCUMENT);
            res.append(CatItem::LOCAL_DATA_FOLDER);
        }
        if(organizingType == CatItem::PUBLIC_FEED){
            res.append(CatItem::PUBLIC_POST);
            res.append(CatItem::PUBLIC_DOCUMENT);
        }
        if(organizingType == CatItem::TAG){
            res.append(CatItem::PUBLIC_DOCUMENT);
        }
        return res;
    }

    QList<CatItem> getOrganizingTypeItems(){
        QList<CatItem> res;
        if(!hasLabel(TYPE_PARENT_KEY)){
            res.append(*this);
            return res;
        }
        ItemType organizingType = getSortingType();
        QList<ItemType> types = getOrganizingTypeList(organizingType);
        for(int i=0; i< types.count(); i++){
            res.append(createTypeParent(types[i]));
        }
        return res;
    }

    void setOrganizingCharacteristic(QString ck){
        setCustomPluginInfo(ORGANIZING_CHARACTERISTIC_KEY,ck);
    }
    QString getOrganizingCharacteristic(){
        return getCustomString(ORGANIZING_CHARACTERISTIC_KEY);
    }

    void setAuthor(QString ck){
        setCustomPluginInfo(AUTHOR_KEY_STR,ck);
    }
    QString getAuthor() const {
        return getCustomString(AUTHOR_KEY_STR);
    }

    void setEmailAddr(QString ck){
        setCustomPluginInfo(AUTHOR_KEY_STR,ck);
    }
    QString getEmailAddr() const {
        return getCustomString(AUTHOR_KEY_STR);
    }

    void setRelationName(QString ck){
        setCustomPluginInfo(RELATION_NAME_KEY,ck);
    }
    QString getRelationName(){
        return getCustomString(RELATION_NAME_KEY);
    }

    QString getVerbDescription() const{
        return getCustomString(CUSTOM_VERB_DISCRIPTION_KEY_STR);
    }

    virtual bool isCustomVerb(){
        if(getItemType() == CatItem::VERB){
            if(getCustomString(CUSTOM_VERB_DISCRIPTION_KEY_STR).length()>0){
                return true;
            }
        }
        return false;
    }

    virtual bool isOrdinaryVerb(){
        if(getItemType() == CatItem::VERB){
            if(!hasLabel(CUSTOM_VERB_DISCRIPTION_KEY_STR)){
                return true;
            }
        }
        return false;
    }

    bool isOperation(){
        if(this->hasLabel(OPERATION_PREFIX)){
            Q_ASSERT(getItemType()==CatItem::OPERATION);
            return true;
        }
        if(hasLabel(IS_OPERATION_KEY_STR)){
            Q_ASSERT(getItemType()==CatItem::OPERATION);
            return true;
        }
        return false;

    }

    void setIsOperation(bool b=false){
        setCustomPluginValue(IS_OPERATION_KEY_STR,(int)b);
    }

    //Anything with a verb description is custom/takes anykey
    void setVerbDescription(QString v){
        setTakesAnykeys(true);
        setItemType(CatItem::VERB);
        setCustomPluginInfo(CUSTOM_VERB_DISCRIPTION_KEY_STR,v);
    }

//    bool getIsForLater() const{
//        return (bool)getCustomValue(IS_FOR_LATER_KEY_STR);
//    }
//
//    void setIsForLater(bool v){
//        setCustomPluginValue(IS_FOR_LATER_KEY_STR,(int)v);
//    }

    bool hasEmbeddedIcon(){
        return d->variantData.contains(REAL_ICON_KEY);
    }

    QString getOverrideIcon() const {
        return getCustomString(ICON_OVERRIDE_KEY_STR);
    }

    void setOverrideIcon(QString v){
        setCustomPluginInfo(ICON_OVERRIDE_KEY_STR,v);
    }


    QString getIcon() const {
        if(!getOverrideIcon().isEmpty()){
            return getOverrideIcon();
        }
        if(getIsVerbArg()){
            return UI_TEXT_PLACEHOLDER_ICON;
        }
        if(getItemType()== CatItem::LOCAL_DATA_FOLDER){
            return FOLDER_ICON_NAME;
        }
        if(getItemType()== CatItem::TAG){
            return TAG_ICON_NAME;
        }
        if(getTagLevel()== CatItem::KEY_WORD){
            return KEYWORD_ICON_NAME;
        }
        return getCustomString(ICON_KEY_STR);
    }

    void setIcon(QString v){
        setCustomPluginInfo(ICON_KEY_STR,v);
    }

    int getItemDisplayHeight() const {
        if(hasLabel(CHILD_ITEMS_HEIGHT_KEY)){
            return getCustomValue(CHILD_ITEMS_HEIGHT_KEY, UI_DEFAULT_ITEM_HEIGHT);
        } else {
            CatItem typeParent = getTypeParent(ACTION_TYPE);
            if(typeParent.hasLabel(CHILD_ITEMS_HEIGHT_KEY)){
                return typeParent.getCustomValue(CHILD_ITEMS_HEIGHT_KEY, UI_DEFAULT_ITEM_HEIGHT);
            }
        }
        CatItem::ItemType iType = getItemType();
        if(iType == PUBLIC_DOCUMENT || iType == PUBLIC_POST || iType == MESSAGE){
            return UI_POSTMASSAGE_ITEM_HEIGHT;
        }
        return UI_DEFAULT_ITEM_HEIGHT;
    }

    void getItemDisplayHeight(int v){
        setCustomValue(CHILD_ITEMS_HEIGHT_KEY,v);
    }

    qint32 getPluginId() const{
        QString path = d->m_path;

        if(path.startsWith(HTTP_PREFIX) ||
           path.startsWith(HTTPS_PREFIX)){
            return FIREFOX_PLUGIN_ID;
        }

        if(!hasLabel(PLUGIN_ID_KEY_STR))
            { return  FILE_CATALOG_PLUGIN_ID;}
        return getCustomValue(PLUGIN_ID_KEY_STR);
    }

    void setPluginId(qint32 v){
        setCustomValue(PLUGIN_ID_KEY_STR,v);
    }

    void setGetWeightFromPlugin(bool v){
        setCustomValue(GET_WEIGHT_FROM_PLUGIN_KEY_STR,v);
    }

    bool doesGetWeightFromPlugin(){
        return getCustomValue(GET_WEIGHT_FROM_PLUGIN_KEY_STR);
    }

    bool getIsAction() const {

        if(this->getItemType()== CatItem::ACTION_TYPE){
            return true;
        }
        if(this->hasLabel(ACTION_NAME)){
            return true;
        }

        return (bool) getCustomValue(IS_ACTION_KEY_STR, (int)IS_ACTION_DEFAULT);
    }
    void setIsAction(bool v){
        setCustomValue(IS_ACTION_KEY_STR,v);
    }

    //These are for incremental updating
    //Each plugin could also set a "bookmark" for where it updated last but
    //that would be plugin-specific and so taken with the custom-string function
    UserEvent::LoadType getLastUpdateType(){
        return (UserEvent::LoadType) getCustomValue(LAST_UPDATE_TYPE_KEY_STR,(int)UserEvent::JUST_FOUND);
    }

    void setUpdated(UserEvent::LoadType t, int depth=1){
        setCustomValue(LAST_UPDATE_TYPE_KEY_STR, (int)t);
        time_t now = appGlobalTime();
        setCustomValue(LAST_UPDATE_TIME_KEY_STR, (int)now);
        setCustomPluginValue(LAST_UPDATE_DEPTH_PASSES_KEY_STR,depth);
    }

    time_t getLastUpdateTime(){
        return (time_t)getCustomValue(LAST_UPDATE_TIME_KEY_STR,d->m_creation_time);
    }

    QString getSubstitutionIndexingPath(){
        return getCustomString(EXTENSION_SUBSTITUTE_PATH_KEY);
    }

    void setSubstitutionIndexingPath(QString s){
        setCustomPluginInfo(EXTENSION_SUBSTITUTE_PATH_KEY,s);
    }

    QString getLastUpdatePath(){
        return getCustomString(LAST_UPDATE_CHILD_PATH_KEY_STR);
    }

    void setLastUpdatePath(QString s){
        setCustomPluginInfo(LAST_UPDATE_CHILD_PATH_KEY_STR,s);
    }

    int getLastUpdateDepth(){
        return this->getCustomValue(LAST_UPDATE_DEPTH_PASSES_KEY_STR, 0);
    }

    void setLastUpdateDepth(int v){
        setCustomPluginValue(LAST_UPDATE_DEPTH_PASSES_KEY_STR,v);
    }

    int getMaxUpdatePasses(int passes = DEFAULT_FULLTEXT_SUBFILE_PASSES){
        return this->getCustomValue(MAX_UPDATE_CHILD_PASSES_KEY_STR, passes);
    }

    void setMaxUpdatePasses(int v){
        setCustomPluginValue(MAX_UPDATE_CHILD_PASSES_KEY_STR,v);
    }

    bool isStub(){ return (bool)getCustomValue(IS_STUB_KEY_STR); };
    void setStub(bool s=true){
        setCustomPluginValue(IS_STUB_KEY_STR,s);
    }

    bool getIsSourceRefresh(){ return (bool)getCustomValue(IS_SOURCE_REFRESH); };
    void setSourceRefresh(bool s=true){ setCustomPluginValue(IS_SOURCE_REFRESH,s); }

    bool shouldUpdate(int childDepth, UserEvent::LoadType lt, float throttle=1){
        Q_ASSERT(throttle >=1);
        time_t now = appGlobalTime();
        time_t then = getLastUpdateTime();
        int period =  UPDATE_DELAY_SECS;
        if(isStub()){ period = STUB_UPDATE_PERIOD; }
        if((now - then)*throttle <= period && getLastUpdateDepth()>=childDepth &&
           (int)getLastUpdateType()>(int)lt) {
            return false ;
        }
        setUpdated(lt,childDepth);
        return true;
    }

    QList<int> getMatchIndex() const{
        return d->m_matchIndex;
    }
    void setMatchIndex(QList<int> il){
        d->m_matchIndex = il;
    }


    QString getUserName(){
        return getCustomString(STREAM_USERNAME_PATH);
    }
    void setUserName(QString s){
        setCustomPluginInfo(STREAM_USERNAME_PATH,s);
    }

    QString getPassword(){
        return getCustomString(STREAM_PASSWORD_PATH );
    }
    void setPassword(QString s){
        setCustomPluginInfo(STREAM_PASSWORD_PATH,s);
    }

    QString getPinnedString(){
        return getCustomString(ITEM_PIN_KEY);
    }
    void setPinned(QString s){
        setCustomPluginInfo(ITEM_PIN_KEY,s);
    }

    bool isPinned(){
        return hasLabel(ITEM_PIN_KEY);
    }

    void setUnpinned(){
        d->m_data.remove(ITEM_PIN_KEY);
    }

    bool getIsDepricated(){
        return (bool)getCustomValue(IS_DEPRICATED_KEY_STR,(int)IS_TEMP_DEFAULT);
    }
    void setIsDepricated(bool v=true){
        setCustomPluginValue(IS_DEPRICATED_KEY_STR,(int)v);
    }

    QString getLoginString(){
        return getCustomString(STREAM_LOGIN_PATH);
    }
    void setLoginString(QString s){
        this->setCustomPluginInfo(STREAM_LOGIN_PATH,s);
    };


    QString getRequestUrl(){
        return getCustomString(STREAM_SOURCE_PATH);
    };
    void setRequestUrl(QString r){
        if(getDescription().isEmpty()){
            setDescription(r);
        }
        setCustomPluginInfo(STREAM_SOURCE_PATH,r);
    };


    void setFilterStr(QString s){
        setCustomPluginInfo(STREAM_FILTER_SOURCE,s);
    }

    QString getFilterFilePath(){
        return getCustomString(STREAM_FILTER_FILESOURCE);
    };

    void setFilterFilePath(QString s){
        setCustomPluginInfo(STREAM_FILTER_FILESOURCE,s);
    }

    QString getJavascriptFilterFilePath(){
        return getCustomString(JAVA_FILTER_FILESOURCE,JAVA_SCRIPT_DEFAULT_PATH);
    }

    void setJavascriptFilterFilePath(QString s){
        setCustomPluginInfo(JAVA_FILTER_FILESOURCE,s);
    }

    QString getPreviewCSSPath(){
        return getCustomString(CSS_FILTER_PATH_KEY,PREVIEW_CSS_DEFAULT_PATH);
    }

    void setPreviewCSSPath(QString s){
        setCustomPluginInfo(CSS_FILTER_PATH_KEY,s);
    }

    QString getOperationHelperPhrase(){
        return getCustomString(OP_HELP_PHRASE_KEY,AS_PHRASE);
    }

    void setOperationHelperPhrase(QString s){
        setCustomPluginInfo(OP_HELP_PHRASE_KEY,s);
    }

    void setHidden(bool h=true){
        setCustomPluginValue(ITEM_HIDDEN_KEY, (int)h);
    }

    bool getIsHidden(){
        return (bool)getCustomValue(ITEM_HIDDEN_KEY);
    }


    void setChildTypeToTake(BaseChildRelation::ChildRelType h){
        setCustomPluginValue(CHILD_TYPE_OP_TAKES, (int)h);
    }

    BaseChildRelation::ChildRelType getchildTypeToTake(){
        return (BaseChildRelation::ChildRelType)getCustomValue(CHILD_TYPE_OP_TAKES);
    }

    //These are TEMP values till we can put the info in the child relation
    void setOrderedBySibling(bool h=true){
        setCustomPluginValue(ORDER_BY_SIBLING_KEY, (int)h);
    }

    bool isOrderedBySibling(){
        return (bool)getCustomValue(ORDER_BY_SIBLING_KEY);
    }

    void setSiblingOrder(int o){
        setCustomPluginValue(SIBLING_ORDER_KEY, (int)o);
    }

    int getSiblingOrder() const{
        return (int)getCustomValue(SIBLING_ORDER_KEY,-1);
    }




    //QHash<QByteArray, QByteArray> m_extraParams;

    //sig = 0-2: 0 none, 1 some, 2: none or some
    int getTagSig(){return getCustomValue(SOURCE_TAG_SIG);}
    int getTimeSig(){return getCustomValue(SOURCE_TIME_SIG);}
    int getKeysSig(){return getCustomValue(SOURCE_KEY_SIG);}
    int getContactSig(){return getCustomValue(SOURCE_CONTACT_SIG);}
    int getLoginSig(){return getCustomValue(SOURCE_LOGIN_SIG);}

    int getListMargin(){
        return getCustomValue(ITEM_NAME_MARGIN_OVERIDE_KEY, UI_LIST_INNER_MARGIN);
    }

    static QString itemCacheDir();


};


//This has to be updated for each member of CatItem, what a drag...
inline QDataStream &operator<<(QDataStream &out, const CatItem &item) {

    out << item.d->m_path;
    out << item.d->m_name;
    out << (int)item.d->m_matchType;
    qint32 t = (long)item.d->m_creation_time;
    out << t;
    out << item.d->m_data;

    //Children now always stored seperately
    //out << item.d->children_detached_list;
    QList<DetachedChildRelation> dummyList;
    out << dummyList;
    //ignoring temp members!

    return out;
}

inline QDataStream &operator>>(QDataStream &in, CatItem &item) {

    in >> item.d->m_path;
    in >> item.d->m_name;
    int type;
    in >> type;
    item.d->m_matchType = (CatItem::MatchType)type;
    qint32 t ;
    in >> t;
    item.d->m_creation_time = t;
    in >> item.d->m_data;
    in >> item.d->children_detached_list;
    item.d->m_lowName = item.d->m_name.toLower();
    item.d->m_matchType = CatItem::USER_KEYS;
        //ignoring temp members!

    return in;
}

//This has to be updated for each member of CatItem, what a drag...
inline QTextStream &operator<<(QTextStream &out, const CatItem &item) {

    QByteArray b;
    QDataStream s(b);
    s << item;
    QString str(b);
    out << str;
    out << ' ';

    return out;
}

inline QTextStream &operator>>(QTextStream &in, CatItem &item) {

    QString str;
    in >> str;

    QByteArray b;
    QDataStream s(str.toAscii());
    s >> item;
    return in;
}

#endif // INNER_ITEM_H
