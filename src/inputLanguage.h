#ifndef INPUT_LANGUAGE_H
#define INPUT_LANGUAGE_H


#include <QList>
#include <QHash>
#include <QString>
#include <QChar>
#include <QRegExp>
#include <QUrl>


#include "item.h"
#include "list_item.h"
//#include "input_list.h"
//#include "globals.h"


struct ExtraFieldReference{
    QString fieldName;
    QString accessValue;
    QString funct;
    QString initialValue;
};


struct EntryField {
        EntryField(){
            required=false;
            inOrder=false;
            encodeUrl=false;
            hidden= false;
            limit=-1;
            order=-1;
        }
        EntryField(QString n, QString v){
            name =n;
            value = v;
            required=false;
            inOrder=false;
            encodeUrl=false;
            hidden= false;
            limit=1000000;
            order=-1;
        }

        bool isPathExpr(){
            return !pathExpression.isEmpty();
        }

	QString name;
	QString value;
        QString path;
	QString type;
        QString regexExpr;
        QString pluginSource;
        QString label;
        CatItem valueItem;
	bool required;
	bool inOrder;
        bool encodeUrl;
        bool hidden;
        QList<QString> pathExpression;
        int order;
        int limit;

        ExtraFieldReference valRef;

};

class VerbItem :public CatItem {
private:
	
    //Input
    QString m_baseString;

    //Parsing constant(s)
    static const QString m_symbols;
    //Parse States;
    int m_pos;
    bool isDone;
    bool isError;
    bool m_in_main_clause;
    bool m_default_longtext_produced;
    bool m_usePath;
    bool m_htmlFormatString;
    bool m_useInternalValue;
    QString m_fieldNameToFormat;
    int m_beginFormattedFieldPos;
    int m_endFormattedFieldPos;

    int m_entryOrderCounter;


    //Results
    QString m_descriptionString;
    QString m_labelCollector;

    //Persistant/result States
    QHash<QString, EntryField> m_fieldsByName;
    QHash<QString, QString> m_pathExpressions;
    QList<QString> m_fieldNamesInOrder;
    QList<QString> m_fieldLabels;

    int m_implictArgIndex;

    bool m_limitField;
	
	
public:
    VerbItem(): CatItem(){
        m_usePath = false;
        m_htmlFormatString = false;
        m_useInternalValue = false;
        m_implictArgIndex = -1;
        m_limitField = true;
        //qDebug() << "creating empty verb";
    }

    VerbItem(CatItem it, bool loadChildren = true, bool useInternalValue=false): CatItem(it){
        m_usePath = false;
        m_htmlFormatString = useInternalValue;
        m_useInternalValue = false;
        m_implictArgIndex = -1;
        m_limitField = true;
        if(it.isCustomVerb()){
            QString verbGen = it.getVerbDescription();
            setInput(verbGen);
            qDebug() << "creating custom verb with: " << verbGen;
            if(loadChildren ){
                loadEntries();
            }
        }
    }

    void setUseInternalValues(bool u=true){
        m_useInternalValue = u;
    }

    void addHash(QHash<QString, QString> hash){
        QHash<QString, QString>::iterator i = hash.begin();
        for(;i!=hash.end();i++){
            EntryField e;
            e.name = i.key();
            e.value = i.value();
            e.valueItem = CatItem();
            addEntry(e);
        }
    }

    void loadEntries(){
        QList<CatItem> childs = getChildren();
        for(int i=0; i < childs.count(); i++){
            if(childs[i].hasLabel(FIELD_TYPE_LABEL)){
                QString v = childs[i].getCustomString(FIELD_VALUE_KEY );
                if(!v.isEmpty()){
                    EntryField e;
                    e.name = childs[i].getName();
                    e.value = v;
                    e.valueItem = childs[i];
                    addEntry(e);
                }
            }
        }
    }

    //Generates a catItem from the filled-in fields
    CatItem creatItem(bool newItem = true){
        CatItem res;
        res.assign_from(*this);
        QString newPathSpec = getCustomString(ITEM_GEN_PATH_KEY);
        if(!newPathSpec.isEmpty() && newItem){
            QString newPath = fillInFields(newPathSpec);
            res.setPath(newPath);
//            QString newFieldsSpec = getCustomString(ITEM_GEN_FIELDS_KEY);
//            if(!newFieldsSpec.isEmpty()){
//                QStringList fsl = newFieldsSpec.split('%');
//                for(int i=0; i < fsl.count(); i++){
//                    QStringList l2 = fsl[i].split('=');
//                    if(l2.length() ==2){
//                        QString v =  fillInFields(l2[1]);
//                        this->setCustomPluginInfo(l2[0],v);
//                    }
//
//                }
//            }
        }
        return res;
    }

    void setDefaultValues(){
        qDebug() << "setDefaultValues";
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            Q_ASSERT(m_fieldsByName.contains(m_fieldNamesInOrder[i]));
            EntryField& e = m_fieldsByName[m_fieldNamesInOrder[i]];



            if(!e.valRef.funct.isEmpty()){
                QString refField = e.valRef.fieldName;
                qDebug() << "setting valRef: " << refField;
                if(!m_fieldsByName.contains(refField)){
                    qDebug() << "syntax error - reference to " << refField << ", which isn't here";
                    Q_ASSERT(false);
                    continue;
                }
                EntryField& rEntry = m_fieldsByName[refField];
                if(rEntry.valueItem.isEmpty()){
                    qDebug() << "warn - empty value for " << refField ;
                    continue;
                }
                QString val = rEntry.valueItem.getCustomString(e.valRef.accessValue);
                QString initialVal = val;
                qDebug() << "from " << e.valRef.accessValue << " got initial: " << val;
                if(e.valRef.funct ==EQUAL_FUNCTION){
                    //nothing
                } else if(e.valRef.funct == QUOTE_FUNCTION){
                    if(e.valRef.accessValue=="name" || e.valRef.accessValue=="Item_Name"){
                        val = SHORT_QUOTE_PREFIX + val;
                    } else {
                        val = LONG_QUOTE_PREFIX + val;
                    }
                } else if(e.valRef.funct == DIR_FUNCTION){
                    if(e.valRef.accessValue=="home"){
                        val = QDir::homePath();
                    } else if(e.valRef.accessValue==SCRIPT_HOME_KEY_DIRS){
                        val = SCRIPT_DEFAULT_PATH;
                    }
//                    else if(gDirs->contains(e.valRef.accessValue) ){
//                        //empty string return 0 in toInt..
//                        int index = e.valRef.initialValue.toInt();
//                        if(gDirs->value(e.valRef.accessValue).length()>0){
//                            val = gDirs->value(e.valRef.accessValue)[index];
//                        }
//                    }
                } else {
                    qDebug() << "syntax error - unknown function: " << e.valRef.funct << " at" << e.name;
                    continue;
                }
                e.value = val;
                e.valRef.initialValue = initialVal;
                qDebug() << "got final val: " << val;
            }
        }
    }

    void setInput(QString s){
            m_baseString = s;
            clearForParse();
            safeGetEntry();
    }

    //Reparses but after some fields filled in...
    QString getFormattedDescription(QString fieldNameToFormat ="", bool htmlFormat=false,
                                    int* beginFieldPos=0, int* endFieldPos=0, bool usePath=true,
                                    bool limitField=true){
        m_limitField = limitField;
        int oldUsePath = m_usePath;
        m_usePath = usePath;

        bool oldFormat = m_htmlFormatString;
        m_htmlFormatString =htmlFormat;
        m_fieldNameToFormat = fieldNameToFormat;
        m_beginFormattedFieldPos = 0;
        m_endFormattedFieldPos = 0;
        clearForParse();
        safeGetEntry();
        m_htmlFormatString = oldFormat;
        if(beginFieldPos && m_beginFormattedFieldPos!=0){
            Q_ASSERT(m_endFormattedFieldPos!=0);
            *beginFieldPos = m_beginFormattedFieldPos;
        }
        if(endFieldPos && m_endFormattedFieldPos!=0){
            *endFieldPos = m_endFormattedFieldPos;
        }

        m_usePath = oldUsePath;
        return m_descriptionString.simplified();
    }

    QString getCommandLine(){
        QString staticCommand = this->getCustomCommandLine();
        if(!staticCommand.isEmpty()){
            return staticCommand;
        }

        QString command = getCustomCommandLine();
        if(command.isEmpty()){
            QString s = fillInFields(command, true);
            return s;
        }
        return this->getParentPath();
    }

    QString getFilledinValue(QString str){
        QString s = fillInFields(str, true);
        return s;
    }



    QString getArgUrl(QString argName){
        QString urlStr = getCustomString(ARG_URL_PREFIX + argName);
        if(urlStr.isEmpty())
            { return QString(); }
        QString s = fillInFields(urlStr, true);
        return s;
    }

    void setImplicitArg(QString n){
        Q_ASSERT(m_fieldsByName.contains(n));
        m_implictArgIndex = m_fieldsByName[n].order;
        Q_ASSERT(m_fieldsByName[m_fieldNamesInOrder[m_implictArgIndex]].name ==n);
    }


    QString getImplicitArg(){
        Q_ASSERT(m_implictArgIndex >=0);
        Q_ASSERT(m_fieldNamesInOrder.count() >0);
        Q_ASSERT(m_fieldsByName.count() >0);
        return m_fieldsByName[m_fieldNamesInOrder[m_implictArgIndex]].name;
    }

    bool getHasImplicitArg(){
        return m_implictArgIndex >= 0;
    }

    QString getFieldTypePath(QString argName){
        return addPrefix(ACTION_TYPE_PREFIX, m_fieldsByName[argName].type);
    }

    QString getFieldType(QString argName){
        return m_fieldsByName[argName].type;
    }

    QString getFieldName(QString argName){
        return m_fieldsByName[argName].name;
    }

    QString getFieldLabel(QString argName){
        return m_fieldsByName[argName].label;
    }

    QString currentFieldPlugin(QString argName){
        return m_fieldsByName[argName].pluginSource;
    }

    bool currentFieldVisible(QString argName){
        return !m_fieldsByName[argName].hidden;
    }

    ListItem getImplicitItem(){
        Q_ASSERT(m_fieldNamesInOrder.count() >0);
        Q_ASSERT(m_fieldsByName.count() >0);
        ListItem li(m_fieldsByName[m_fieldNamesInOrder[m_implictArgIndex]].valueItem);
        return li;
    }

    void clearForParse(){
        m_pos = 0;
        isDone = false;
        isError = false;
        m_in_main_clause = true;
        m_descriptionString.clear();
        m_labelCollector.clear();
    }

    bool atFieldGroupEnd(QString name){
        EntryField e = m_fieldsByName[name];
        return e.inOrder;
    }

    EntryField atField(int i){
        QString name = m_fieldNamesInOrder[i];
        return m_fieldsByName[name];
    }

    EntryField atField(QString name){
        return m_fieldsByName[name];
    }

    QList<CatItem> getUnfilledArguments(){
        QList<CatItem> res;
        bool earlierArgsNotFound = false;
        int minArgs=0;
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            Q_ASSERT(m_fieldsByName.contains(m_fieldNamesInOrder[i]));
            EntryField e = m_fieldsByName[m_fieldNamesInOrder[i]];
            if(e.required){
                minArgs++;
            }
            if(e.value.isEmpty() && e.valueItem.isEmpty()){
                if(earlierArgsNotFound && e.inOrder)
                    { continue; }
                if(e.required)
                    { earlierArgsNotFound = true; }
                CatItem it = itemFromFieldEntry(e);
                res.append(it);
            }
        }
        return res;
    }

    bool argFullfilled(QString str){
        EntryField e = m_fieldsByName[str];
        return (!e.value.isEmpty());
    }

    bool hasField(QString s){
        if(!m_fieldsByName.contains(s))
            { return false; }

        if(m_fieldsByName[s].value.isEmpty())
            { return false; }

        return true;
    }

    CatItem getValueItem(QString s){
        if(!m_fieldsByName.contains(s))
            { return CatItem(); }

        return m_fieldsByName[s].valueItem;
    }

    QList<CatItem> getArguments(){
        QList<CatItem> res;
        bool earlierArgsNotFound = false;
        int minArgs=0;
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            Q_ASSERT(m_fieldsByName.contains(m_fieldNamesInOrder[i]));
            EntryField e = m_fieldsByName[m_fieldNamesInOrder[i]];
            if(e.required){
                minArgs++;
            }
            if(earlierArgsNotFound && e.inOrder)
                { continue; }
            if(e.value.isEmpty()){
                if(e.required){
                    earlierArgsNotFound = true;
                }
            }
            CatItem it = itemFromFieldEntry(e);
            res.append(it);
        }
        return res;
    }

    CatItem itemFromFieldOrder(int order){
        QString argName = m_fieldNamesInOrder[order];
        return itemFromField(argName);
    }

    CatItem itemFromField(QString argName){
        if(!m_fieldsByName.contains(argName)){ return CatItem(); }
        EntryField field = m_fieldsByName[argName];
        Q_ASSERT(field.name == argName);
        return itemFromFieldEntry(field);

    }

    CatItem itemFromFieldEntry(EntryField field){

        CatItem it(getPath() + "::arg"
                   + QString::number(field.order), field.name);

        it.setArgNumber(field.order);
        it.setArgValue(field.value);
        it.setArgName(field.name);
        if(!field.type.isEmpty()){
            it.setArgType(field.type);
        } else {
            it.setArgType(SHORT_TEXT_NAME);
        }

        it.setTakesAnykeys(true);
        it.setUseDescription(true);
        it.setItemType( CatItem::ACTION_TYPE);
        it.setDescription(CUSTOM_FIELD_DESCRIPTION_PREFIX + field.name);
        it.setCustomPluginValue(IS_REQUIRED_ARG_KEY,
                                (int)field.required);
        it.setIsVerbArg(true);
        return it;
    }

    CatItem actionTypeItemFromField(int order){
        QString argPath = m_fieldNamesInOrder[order];
        QString argType = m_fieldsByName[argPath].type;

        if(argType.isEmpty()){
            argType = SHORT_TEXT_NAME;
        }
        CatItem it = createActionItem(argType);
        return it;
    }

    QList<CatItem> getRequiredArguments(){
        QList<CatItem> res;
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            Q_ASSERT(m_fieldsByName.contains(m_fieldNamesInOrder[i]));
            EntryField e = m_fieldsByName[m_fieldNamesInOrder[i]];
            if(!e.required)
                { continue; }
            CatItem it = itemFromFieldOrder(i);
            res.append(it);
        }
        return res;
    }

    QList<CatItem> getActionTypeSignature(){
        QList<CatItem> res;
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            Q_ASSERT(m_fieldsByName.contains(m_fieldNamesInOrder[i]));
            EntryField e = m_fieldsByName[m_fieldNamesInOrder[i]];
            if(!e.required)
            { continue; }
            CatItem it = actionTypeItemFromField(i);
            res.append(it);
        }
        return res;
    }

    QHash<QString, CatItem> getArgHash(){
        QHash<QString, CatItem> res;
        for(int i=0; i < m_fieldNamesInOrder.count(); i++){
            CatItem it = itemFromFieldOrder(i);
            res[m_fieldNamesInOrder[i]] = (it);
        }
        return res;
    }

    QHash<QString, EntryField> getValueHash(){
        QHash<QString, EntryField> res;
        for(int i=0; i < m_fieldNamesInOrder.count(); i++){
            res[m_fieldNamesInOrder[i]] = m_fieldsByName[m_fieldNamesInOrder[i]];
        }
        return res;
    }

    bool fieldsSatisfied(QString* err=0){
        for(int i=0; i < m_fieldNamesInOrder.count();i++){
            EntryField e = m_fieldsByName[m_fieldNamesInOrder[i]];
            if(e.required && e.value.isEmpty()) {
                if(err){
                    *err += e.name + ARGUMENT_IS_MISSING_POSTFIX_MESSAGE;
                }
                return false;

            }
        }
        return true;
    }




    QString fillInFields(QString s, bool usePath=false){
            VerbItem b(s, this, usePath, false);
            if (b.isError){
                    return "";
            }
            return b.m_descriptionString;
    }

    void setFields(QHash<QString, EntryField> valHash){
        QHash< QString, EntryField>::iterator b = this->m_fieldsByName.begin();
        for(;b != m_fieldsByName.end();b++){
            m_fieldsByName[b.key()].value.clear();
            m_fieldsByName[b.key()].path.clear();
            m_fieldsByName[b.key()].valueItem = CatItem();
        }

        QHash< QString, EntryField>::iterator i = valHash.begin();
        for(;i != valHash.end();i++){
            QString value = i.value().value;
            CatItem it = i.value().valueItem;
            setFieldIfInitialized(i.key(),value, it.getPath(), it);
        }
    }

    void getFieldValues(QHash<QString, EntryField> valHash){
        QHash< QString, EntryField>::iterator i = valHash.begin();
        for(;i != valHash.end();i++){
            QString value = i.value().value;
            CatItem it = i.value().valueItem;
            setFieldIfInitialized(i.key(),value, it.getPath(), it);
        }
    }

//    void setFields(QHash<QString, EntryField> valHash){
//        QHash< QString, EntryField>::iterator b = this->m_fieldsByName.begin();
//        for(;b != m_fieldsByName.end();b++){
//            m_fieldsByName[b.key()].value.clear();
//            m_fieldsByName[b.key()].path.clear();
//            m_fieldsByName[b.key()].valueItem = CatItem();
//        }
//
//        QHash< QString, QString >::iterator i = valHash.begin();
//        for(;i != valHash.end();i++){
//            setField(i.key(),i.value());
//        }
//    }

    void setField(QString name, QString v, QString p = "", CatItem it=CatItem()){
        if(m_fieldsByName.contains(name)) {
            if(!v.isEmpty()){
                m_fieldsByName[name].value = v;
            }
            if(!it.isEmpty()){
                m_fieldsByName[name].valueItem = it;
                m_fieldsByName[name].path = it.getPath();
            }
            if(!p.isEmpty()){
                m_fieldsByName[name].path = p;
            }
        }
    }

    void setField(QString name, CatItem it){

        Q_ASSERT(m_fieldsByName.contains(name));
        Q_ASSERT(!it.isEmpty());
        m_fieldsByName[name].valueItem = it;
        m_fieldsByName[name].path = it.getPath();

    }

    void setFieldIfInitialized(QString name, QString v, QString p = "", CatItem it=CatItem()){
        if(m_fieldsByName.contains(name)) {
            if(!v.isEmpty()){
                m_fieldsByName[name].value = v;
            }
            if(!it.isEmpty()){
                m_fieldsByName[name].valueItem = it;
                m_fieldsByName[name].path = it.getPath();
            }
            if(!p.isEmpty()){
                m_fieldsByName[name].path = p;
            }
        }
    }

    void setUnasignedField(EntryField e){

        if(!m_fieldsByName.contains(e.name)){
            e.order = m_fieldNamesInOrder.count();
            m_fieldsByName[e.name] =e;
            m_fieldNamesInOrder.append(e.name);
        } else {
            if(m_fieldsByName[e.name].value.isEmpty()){
                m_fieldsByName[e.name].value = e.value;
            }
            if(m_fieldsByName[e.name].valueItem.isEmpty()){
                m_fieldsByName[e.name].valueItem = e.valueItem;
            }
        }
    }

    void clearField(QString name, QString v, QString p = "", CatItem it=CatItem()){
        if(m_fieldsByName.contains(name)) {
            m_fieldsByName[name].value = v;
            m_fieldsByName[name].path = p;
            m_fieldsByName[name].valueItem = it;

        }
    }

    void removeInput(QString name){
        if(m_fieldsByName.contains(name)) {
            m_fieldsByName[name].value = "";
            m_fieldsByName[name].path = "";
        }
    }

    bool error(){
        return isError;

    }
	
    VerbItem(QString s): CatItem(s){
        m_usePath = false;
        m_htmlFormatString = false;
        m_useInternalValue = false;
        m_implictArgIndex = 0;
        m_limitField = true;
        setInput(s);
    }

    QList<EntryField > getPathExpressions(){
        QList<EntryField > res;
        for(int i=0; i< m_fieldNamesInOrder.count(); i++){
            if(atField(i).isPathExpr()){
                res.append(atField(i));
            }
        }
        return res;
    }

    static bool atPathField(QString s){
        return s[0]!='.' && s.contains('.');
    }

    static QList<QString> toPath(QString s){
        return s.split('.');
    }

protected:

    VerbItem(QString s, VerbItem* b, bool usePath, bool limit=true){
        m_htmlFormatString = false;
        m_useInternalValue = false;
        m_fieldsByName = b->m_fieldsByName;
        m_fieldNamesInOrder = b->m_fieldNamesInOrder;
        m_usePath = usePath;
        m_implictArgIndex = b->m_implictArgIndex;
        m_fieldNameToFormat = b->m_fieldNameToFormat;
        m_beginFormattedFieldPos = b->m_beginFormattedFieldPos;
        m_endFormattedFieldPos = b->m_endFormattedFieldPos;
        m_limitField = limit;
        setInput(s);
    }
    void safeGetEntry(){
        try {
                getEntry();
        } catch(QString exp){
                isError = true;
                qDebug() << "error received: " << exp;
        }
    }

    void getEntry(){
        m_in_main_clause = true;
        bool foundOne = false;
        m_entryOrderCounter = 0;
        //qDebug() << "parsing: " << m_baseString;
        while(true){
            QString chars = slurpRemainingString();
            m_descriptionString +=chars;
            m_labelCollector +=chars;

            if(atControlChar('[')){
                next();
                foundOne = true;
                getBracketField();
            } else if(atControlChar('$')){
                next();
                EntryField e = getField();
                if(entryFullfilled(e) || m_htmlFormatString){
                    int outPos = m_descriptionString.length();
                    m_descriptionString += fieldAsString(e,outPos);
                }
                foundOne = true;
            } else if(!done() && !foundOne){
                throwError(QString("Unexpected (illegal) symbol: '%1' ").arg( at()));
            } else {return;}
        }
    }

    void getBracketField(){
        bool satisfied = true;
        bool fieldEntered= false;

        QString accum;
        QString result;
        EntryField workingField;

        m_in_main_clause = true;
        while(true){
            bool oneUsed = false;
            accum += slurpRemainingString();
            if(atControlChar('$') ){
                oneUsed = true;
                next();
                workingField = getField();
                if(entryFullfilled(workingField)){
                    int outPos = m_descriptionString.length() + result.length() + accum.length();
                    accum += fieldAsString(workingField,outPos);
                    satisfied = true;
                } else {
                    if(m_htmlFormatString) {
                        int outPos = m_descriptionString.length() + result.length() + accum.length();
                        accum += fieldAsString(workingField,outPos);

                    }
                    satisfied = false;
                }
            }
            if(atControlChar('|') ){ //trickier part
                oneUsed = true;
                next();
                m_in_main_clause = false;
                if(satisfied && !fieldEntered && !workingField.hidden){
                    result = accum;
                    fieldEntered = true;
                } else {
                    satisfied = true;
                }
                accum = "";
            }
            if(atControlChar(']')){
                oneUsed = true;
                next();
                if(satisfied && result.isEmpty() && !workingField.hidden){
                    result = accum;
                }
                if(!result.isEmpty()) {
                    m_descriptionString +=result;
                    m_labelCollector +=result;
                }
                m_in_main_clause = true;
                return;
            }
            if(done() || (!oneUsed && atSymbol())){
                throwError("unterminated bracket in expression or other illegal character");
            }
        }
    }

    void processExtendedType(EntryField& e,QString type){
        QList<QString> parts = type.split('.');
        e.type = type;
        e.valRef.funct = parts[0];
        e.valRef.fieldName = parts[1];
        if(parts.length()==2){
            e.type = SHORT_TEXT_NAME;
        } else if(parts.length()==3){
            e.valRef.accessValue = parts[2];
            e.type = getCannonicalFieldName(parts[2]);
        } else if(parts.length()>3){
            e.valRef.accessValue = parts[2];
            e.type = parts[3];
        }
    }

    EntryField getField(){
        EntryField e;
        e.name =  slurpRemainingWord();

        if(atPathField(e.name)){
            QList<QString> expr = toPath(e.name);
            if(!expr[0].isEmpty()){
                e.pathExpression = expr;
                m_pathExpressions[expr[0]]= e.name;
            } else {
                qDebug() << "syntax error at: " << e.name;
            }
        }

        QString type = slurpFieldType();
        if(atPathField(type)){
            QList<QString> expr = toPath(e.name);
            if(expr[0].isEmpty()){
                qDebug() << "syntax error at: " << e.name;
                Q_ASSERT(false);
            } else {
                processExtendedType(e, type);
            }
        } else {
            e.type = type;
        }
        //qDebug() << "get field type" << type;
        if(type == URL_ENCODE_NAME){
            e.type = SHORT_TEXT_NAME;
            e.encodeUrl = true;
        }
        e.pluginSource = slurpPluginName();
        e.limit = slurpFieldLimiter();
        e.regexExpr = slurpRegexExpr();
        e.label = m_labelCollector;
        m_labelCollector.clear();

        if(atControlChar('^')){
            next();
            e.hidden = true;
        }
        if(atControlChar('+')){
            next();
            e.required = true;
            if(atControlChar('+')){
                next();
                e.inOrder = true;
            }
        } else if(atControlChar('-')){
            next();
            e.inOrder = false;
        }
        if(m_in_main_clause) { 
            e = addEntry(e);
        }

        e.order = m_entryOrderCounter;
        m_entryOrderCounter++;
        //qDebug() << "input lang - got field: " << e.name <<":" << e.value;
        return e;

    }

    int slurpFieldLimiter(){
        int res = 0;
        if(atControlChar('#') ){
            next();
            while(!done() && at().isDigit()){
                res = res*10 + (int)(at().toAscii() - ('0'));
                next();
            }
        }
        return res;

    }

    QString slurpFieldType(){
        if(atControlChar(':') ){
            next();
            return slurpRemainingWord();
        } else  if(!m_default_longtext_produced){
            return LONG_TEXT_NAME;
            m_default_longtext_produced = true;
        } else {
            return SHORT_TEXT_NAME;
        }
    }

    QString slurpRegexExpr(){
        //let's you
        if(atControlChar(';') ){
            next();
            return "";
        }

        if(!atControlChar('\'') ){ return ""; }
        next();
        int lookahead = m_pos;
        while(m_baseString[lookahead]!='\''){
            if(lookahead < m_baseString.count()){
                qDebug() << "syntax error unclosed single quote(') at:" << m_pos;
                return "";
            }
            lookahead++;
        }
        QString s = m_baseString.mid(m_pos, lookahead - m_pos);
        //qDebug() << "slurped regex: " << s;
        m_pos = lookahead;
        return s;

    }
    
    QString slurpPluginName(){
        if(atControlChar(':') ){
            next();
            return slurpRemainingWord();
        }
        return "";
    }

    bool atControlChar(QChar c){
        if(done()){
            return false;
        }
        Q_ASSERT(m_pos >=0 && (m_pos < m_baseString.length()));
        return (m_baseString[m_pos] == c ); //&& (m_pos==0 || m_baseString[m_pos-1]!='\\')
    }

    QChar at(){
        Q_ASSERT(m_pos >=0 && (m_pos < m_baseString.length()));
        return (m_baseString[m_pos]);
    }


    void next(){
        //qDebug() << "at char: " << m_baseString[m_pos];
        m_pos++;

    }

    bool withinWord(QString str, int offset){

        if(offset == str.length())
            { return false; }
        return
                str[offset].isLetter()
                || str[offset].isDigit()
                || str[offset] ==FIELD_TYPE_SEPERATOR
                || str[offset] =='_'
                || str[offset] =='.'
                || str[offset] =='/'
                || (offset>0 && str[offset-1]=='\\');
    }

    bool done(){ return (m_pos >= m_baseString.length() ); }
    void throwError(QString e){

        int prev = MAX(0,m_pos-10);
        e += QString("after: ") + QString::number(m_pos) + ": "
             + m_baseString.mid(prev, m_pos-prev);
        qDebug() << "throwing " << e;
        isError = true;
        throw e;
    }

    EntryField addEntry(EntryField e){

        if(!m_fieldsByName.contains(e.name)){
            e.order = m_fieldNamesInOrder.count();
            m_fieldsByName[e.name] =e;
            m_fieldNamesInOrder.append(e.name);
        } else {
            if(m_fieldsByName[e.name].value.isEmpty()){
                m_fieldsByName[e.name].value = e.value;
            }
            if(!e.valueItem.isEmpty()){
                m_fieldsByName[e.name].valueItem = e.valueItem;
            }
        }
        //qDebug() << "field added:" << e.name;
        return m_fieldsByName[e.name];
    }

    bool entryFullfilled(EntryField e){
        return (m_fieldsByName.contains(e.name) && !m_fieldsByName[e.name].value.isEmpty());
    }

    QString fieldAsString(EntryField e, int outPos){
        Q_ASSERT(m_fieldsByName.contains(e.name));

        QString res;
        if(!m_fieldNameToFormat.isEmpty() &&
            (e.name == m_fieldNameToFormat)){
            //res = m_fieldsByName[e.name].value;
            res = EXTENSION_STRING;
            //res = htmlFontFormat(res,UI_SUBACTIVE_FONT);
            m_beginFormattedFieldPos = outPos;
            m_endFormattedFieldPos = outPos + res.length();
        } else if(m_usePath && !m_fieldsByName[e.name].path.isEmpty()){
            if(getItemType()==CatItem::MESSAGE || getItemType()==CatItem::PERSON){
                res = m_fieldsByName[e.name].name;
            } else {
                res = m_fieldsByName[e.name].path;
            }
        } else if(m_fieldsByName.contains(e.name)) {
            res = (m_fieldsByName[e.name].value);
        } else if(m_useInternalValue){
            if(this->hasLabel(e.name)){
                res = this->getCustomString(e.name);
            }
        }

        if(!e.regexExpr.isEmpty()){
            QRegExp r(e.regexExpr);
            res = r.indexIn(res);
        }

        if(m_limitField){
            int l=0;
            if(e.limit >0) {
                l = e.limit;
            } else if(m_htmlFormatString){
                l =  (UI_FIELD_DISPLAY_LIMIT);
            }
            if(e.type== WEB_ADDRESS_NAME){
                if(res.length() > l + ELIPSIS_STRING_CHARS.length()-1){
                    res = ELIPSIS_STRING_CHARS + res.right(l); }
            } else if(e.type == FILE_NAME){
                    if(res.length() > l + ELIPSIS_STRING_CHARS.length()-1){
                        res = quickCondenseFile(res,l); }
            } else {
                if(res.length() > l + ELIPSIS_STRING_CHARS.length()-1){
                    res = res.left(l)+ ELIPSIS_STRING_CHARS; }
            }

            if(res.isEmpty() && m_htmlFormatString){
                res = UI_MISSING_FIELD_INTICATOR; }
        }

        if(this->m_htmlFormatString){ res = makeItallic(res);}

        if(e.encodeUrl){
            res = QUrl::toPercentEncoding(res);
            //qDebug() << "encoding...";
        }
        //qDebug() << "filled-in field: " << res;
        return res;
    }


    QString slurpRemainingWord(){
        int lookahead = m_pos;
        while(lookahead < m_baseString.count() &&
              withinWord(m_baseString, lookahead)){
            lookahead++;
        }
        //QString s = m_baseString.mid(m_pos, lookahead - m_pos);
        //qDebug() << "slurped remaining word: " << s;
        int len = lookahead - m_pos;
        QString s;
        s.reserve(len);
        for(int i=0;i<len;i++){
            if((m_baseString[i]!='\\' )
                &&( m_pos +i+1==len
                    || (m_baseString[m_pos +i]!='$' && m_baseString[m_pos +i+1]!='$'))){
                s.append(m_baseString[m_pos + i]);
            }
        }
        m_pos = lookahead;
        return s;
    }

    QString quickCondenseFile(QString path, int length){
        ListItem li(path);
        li.createComponentList();
        QString res = li.getFormattedPath(length);
        res = crudeHtmlFragmentToPlainText(res);
        return res;
    }

    QString slurpRemainingString(){
        int lookahead = m_pos;
        while (true){
            if(m_baseString.count() <= lookahead){
                break;
            }
            if(atSymbol(lookahead)){
                //qDebug() << "moved to symbol: " << m_baseString[lookahead];
                break;
            }
            lookahead++;
        }
        //QString s = m_baseString.mid(m_pos, lookahead - m_pos);
        int len = lookahead - m_pos;
        QString s;
        s.reserve(len);
        for(int i=0;i<len;i++){
            //to allow '$$var' to translate to '$var'
            //and '\\' to translate to '\'
            if((m_baseString[i]!='\\' )
                &&( i==len-1
                    ||
                  (!(m_baseString[i]=='$' && m_baseString[i+1]=='$')))
               ){
                s.append(m_baseString[m_pos+i]);
            }
        }
        m_pos = lookahead;
        return s;
    }

    bool atSymbol(int lookahead= -1){
        if(lookahead==-1){
            lookahead = m_pos;
        }
        if(lookahead == m_baseString.length()){ return true; }
        if((m_symbols.contains(m_baseString[lookahead]))
            && (!(lookahead>0 && m_baseString[lookahead-1] =='\\')) ){
            return true;
        } else return false;
    }

};

#endif
