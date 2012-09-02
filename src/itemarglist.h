#ifndef ITEMARGLIST_H
#define ITEMARGLIST_H

#include <QList>

#include "input_data.h"
#include "input_list.h"
//#include "catalog.h"

//#include <QTranslator>
#include "history_item.h"


class InputList :public ItemFilter {

private:

    QList<InputData> inputData;

    int m_itemPosition;
    int m_opIndex;
    int m_verbIndex;
    int m_nounsBeginIndex;
    int m_nounsEndIndex;
    QString m_searchContext;

    QString m_errorState;

    VerbItem m_theNullItem;
    VerbItem m_nonCustomVerb;

    bool m_slotAdded;
    int m_slotRemoved;

    //Custom verbs
    int m_customVerbUserInsertPoint;
    bool m_enterArgValue;

    int m_searchLevel;

    QList<CatItem> m_subclassificationItems;

public:
    enum FieldType {
        NORMAL_DUAL_SELECT,
        USER_TEXT_OR_EXPLICIT_SELECT,
        LIST_SELECT_ONLY,
        USER_TEXT
    };

    void setSearchLevel(int level){ m_searchLevel = level;}
    int getSearchLevel(){ return m_searchLevel;}

    bool hasVerb(){ return (m_verbIndex !=-1); }

    void setDefaultValues(){
        VerbItem& itsVerb = getVerb();
        itsVerb.setDefaultValues();
        //rescanSlot();
    }

    QString getArgNameAt(int i){
        if(i == m_verbIndex){
            return "";
        } else {
            return inputData[i].getOrganizingArg().getName();
        }
    }

    QString getArgValue(int i, bool fullPath=false){
        QString value;
        if(fieldInputType(i)!= USER_TEXT){
            if(fullPath){
                value = inputData[i].getCurrentValue().getPath();
            } else {
                value = inputData[i].getCurrentValue().getName();
            }
        } else {
            value = inputData[i].getUserKeys();
        }
        return value;
    }

    QString getArgLabel(int pos=-1){
        if(pos==-1){
            pos = m_itemPosition;
        }
        if(pos==m_opIndex){
            return TAG_PHRASE;
        }
        QString argName = inputData[pos].getOrganizingArg().getName();
        QString label = getVerb().getFieldLabel(argName);
        return label;
    }

    QString getArgLabel(CatItem it){
        QString label = getVerb().getFieldLabel(it.getName());
        return label;
    }

    QString getArgTypePathAt(int i){
        if(i == m_opIndex){return "";}

        QString n = getArgNameAt(i);
        if(n.isEmpty()){ return ""; }
        return getVerb().getFieldTypePath(n);
    }

    QString getArgTypeAt(int i){
        if(i == m_opIndex){return "";}

        QString n = getArgNameAt(i);
        if(n.isEmpty()){ return ""; }
        return getVerb().getFieldType(n);
    }

    QString getArgType(CatItem it){
        QString n = it.getName();
        if(n.isEmpty()){ return ""; }
        return getVerb().getFieldType(n);
    }

    CatItem getArgItemAt(int i){
        if(m_itemPosition == m_verbIndex){
            if(getVerb().isCustomVerb()){
                return (inputData[i].getOrganizingArg());
            } else {
                return inputData[i].getCurrentItem();
            }
        } else {
            return (inputData[i].getOrganizingArg());
        }
    }

    bool argVisibleAt(int i){
        CatItem arg = inputData[i].getOrganizingArg();
        QString argName = arg.getArgName();
        return getVerb().currentFieldVisible(argName);
    }


    QString currentFieldPlugin(int i){
        if(i == m_opIndex){return "";}

        QString n = getArgNameAt(i);
        if(n.isEmpty()){ return ""; }
        return getVerb().currentFieldPlugin(n);
    }

private:

    bool matchFieldToArg(CatItem arg, InputData& inputD){
        QString argType = arg.getArgType();
        argType = CatItem::getCannonicalFieldName(argType);
        QList<CatItem> items = getPossibleArgTypes(inputD);
        for(int i=0; i<items.count(); i++){
            CatItem it = items[i];
            Q_ASSERT(!it.isEmpty());
            QString actionType = it.getMimeType();
            actionType = CatItem::getCannonicalFieldName(actionType);
            if(!argType.isEmpty() && actionType==ANY_ITEM_NAME){
                return true;
            }
            if (!argType.isEmpty() && argType == actionType) {
                return true;
            }
        }
        if(!inputD.getCurrentItem().isEmpty()){
            return false;
        }

        QString userKeys =  inputD.getUserKeys();

        if(userKeys.isEmpty()) { return true; }

        if(argType == NUMBER_NAME){
            if(isNumberField(userKeys))
                { return true; }
        } else if(argType == WEB_ADDRESS_NAME){
            if(isWebAdrField(userKeys))
                { return true; }
        } else if(argType == EMAIL_ADDRESS_NAME
                  || argType==RFC_EMAIL_MESSAGE_NAME
                  || argType==YOUR_EMAIL_ADDRESS_NAME){
            if(isEmailAdrField(userKeys))
                { return true; }
        } else if(argType == LONG_TEXT_NAME){
            if(isLongText(userKeys))
                { return true; }
        } else if(argType == SHORT_TEXT_NAME){
            if(isShortText(userKeys) )
                { return true; }
        }
        return false;
    }

public:


    VerbItem& getVerb(){
        Q_ASSERT(m_theNullItem.isEmpty());

        if(!hasVerb()){ return m_theNullItem; }

        if(!inputData[m_verbIndex].getOrganizingArg().isEmpty()){
            return inputData[m_verbIndex].getOrganizingArg();
        } else {
            if(!(inputData[m_verbIndex].getOrganizingArg().getActionType()==CatItem::VERB_ACTION ||
                 inputData[m_verbIndex].getCurrentItem().getActionType()==CatItem::VERB_ACTION)){
                return m_theNullItem;
            }
            m_nonCustomVerb = inputData[m_verbIndex].getCurrentItem();
            return m_nonCustomVerb;
        }
    }
public:

    InputList(): ItemFilter() {
        clearAll();
    }

    bool usingCustomVerb(){
        return  getVerb().isCustomVerb();
    }

    bool isOperationChosen(){
        if(m_opIndex==-1){ return false; }
        return inputData[m_opIndex].isOrganizingArgChosen();
    }

    bool customVerbChosen(){
        if(!hasVerb()){ return false; }
        if(!usingCustomVerb()){return false;}
        Q_ASSERT(getVerb().getActionType()==CatItem::VERB_ACTION );
        return inputData[m_verbIndex].isOrganizingArgChosen();
    }

    void setCustomVerbChosen(){
        if(!hasVerb()){ return ; }
        if(!getVerb().isCustomVerb()){
            return;
        }
        Q_ASSERT(getVerb().getActionType()==CatItem::VERB_ACTION );
        QHash<int, CatItem> anticipatedArgs = getArgMatchesInOrder(getVerb());
        QHash<QString, EntryField> anticipatedValues = asFieldValueHashEx(anticipatedArgs);
        getVerb().getFieldValues(anticipatedValues);
        inputData[m_verbIndex].setOrganizingArgChosen();
        setDefaultValues();
        addDefaultArguments();
    }

    bool atPossibleVerbSlot(){
        return (m_verbIndex == m_itemPosition || m_verbIndex==-1);
    }

    int verbPosition(){ return m_verbIndex;}

    bool keepUserFields(){
        if(!usingCustomVerb()){
            return false;
        }
        if(m_verbIndex ==0 ){
            return false;
        }
        if(m_nounsBeginIndex==1 && m_nounsEndIndex==1){
            return false;
        }
        return true;
    }

    bool isSelectionEmpty(){
        Q_ASSERT(!inputData.isEmpty());
        Q_ASSERT(m_itemPosition < inputData.count() );
        if(!inputData[m_itemPosition].getCurrentItem().isEmpty()){
            return false;
        }
        if(!inputData[m_itemPosition].getOrganizingArg().isEmpty()){
            return false;
        }
        return true;
    }

    QHash<int, CatItem>  getArgMatchesInOrder(VerbItem& vi){

        //VerbItem vi(verb);
        QList<CatItem> args = vi.getArguments();
        QList<CatItem> argAccu = args;
        QHash<int, CatItem> res;

        for(int i=0; i<inputData.count();i++){
            if(m_opIndex==i){continue;}
            if(i == m_itemPosition && vi.getMatchType()!=CatItem::DATA_INFERENCE)
                { continue; }
            for(int j=0; j<args.count();j++){
                if(!argAccu.contains(args[j])){ continue;}
                if(matchFieldToArg(args[j], inputData[i])){
                    res[i] = (args[j]);
                    argAccu.removeAll(args[j]);
                    break;
                }
            }
        }
        return res;
    }

    QHash<int, CatItem> getArgMatchesWithNewField(int pos, QString n) {
        QHash<int, CatItem> res;
        int prevArgPos=-1;
        for(int i=0; i<inputData.count();i++) {
            if(m_opIndex==i){ continue; }
            CatItem arg = getArgItemAt(i);
            if(arg.getName()==n && pos!=i){ prevArgPos = i; }
            res[i] = arg;
        }

        if( prevArgPos <0 ){ return res; }

        QList<CatItem> args = getRemainingCustomArgs();
        for(int j=0; j<args.count();j++) {
            if(matchFieldToArg(args[j], inputData[prevArgPos])) {
                res[prevArgPos] = args[j];
                break;
            }
        }
        return res;
    }

    CatItem argChosenType(){
        CatItem & it = inputData[m_itemPosition].getOrganizingArg();
        return it.getTypeParent(CatItem::ACTION_TYPE);
    }



    QString currentFieldExpression(){
        return getArgTypePathAt(m_itemPosition);
    }

    QString currentFieldPlugin(){
        return currentFieldPlugin(m_itemPosition);
    }

    bool hasItem(){ return !inputData[m_itemPosition].getCurrentItem().isEmpty(); }
    bool canAcceptOperation(){
        return (m_opIndex ==-1 )
                && m_itemPosition== this->inputData.length()-1;
    }

    QString getAppState(){ return m_searchContext; }

    void setAppState(QString s){
        m_searchContext = s;
    }
    QString previewUrl(){
        if(usingCustomVerb()){
            if(m_nounsBeginIndex <=m_itemPosition &&  m_nounsEndIndex){
                QString argName = getArgNameAt(m_itemPosition);
                return getVerb().getArgUrl(argName);
            }
        }
        return QString();
    }

    //int getItemCount(){ return inputData[m_itemPosition].getListItemCount()(); }

    CatItem getFilterItem(){
        return inputData[m_itemPosition].getFilterItem();
    }

    CatItem getParentItem(){
        return inputData[m_itemPosition].getParentItem();
    }


    bool verbNeedMoreInfo(){
        if(!usingCustomVerb()){
            return false;
        }
        if(getVerb().fieldsSatisfied()){
            return false;
        }
        return true;
    }

    FieldType fieldInputType(int pos=-1){
        if(m_opIndex >-1 && inputData[m_opIndex].isOrganizingArgChosen()){
            if(inputData[m_opIndex].getOrganizingArg().getPath()==
               OP_REMOVE_PATH){
                return LIST_SELECT_ONLY;
            }
            return USER_TEXT_OR_EXPLICIT_SELECT;
        }
        if(atCustomField()){ return USER_TEXT;}
        if(pos==-1){pos = m_itemPosition;}
        if(!customVerbChosen()){ return NORMAL_DUAL_SELECT; }
        QString argType = getArgTypeAt((pos));
        Q_ASSERT(!argType.isEmpty());
        Q_ASSERT(!inputData[pos].getOrganizingArg().isEmpty());
        if(!CatItem::atUserDataField(argType)){ return NORMAL_DUAL_SELECT; }
        if(pos == m_verbIndex){ return NORMAL_DUAL_SELECT; }
        return USER_TEXT;
    }


    bool atFieldChoicePoint(){
        return false;
//        if(!usingCustomVerb()){return false;}
//        //if(atUserDataField()){return false;}
//        if(!getUserKeys().isEmpty()){return false;}
//        if(getRemainingCustomArgs(false).count()>1){
//            return true;
//        }
//        return false;
    }

    bool atItemChoiceField(int pos){
        if(pos!=m_itemPosition){ return false;}
        if(!usingCustomVerb()){ return false;}
        if(inputData[pos].getOrganizingArg().isEmpty()){
            return false;
        }
        if(!inputData[pos].isOrganizingArgChosen()){
            return false;
        }
        QString argType = getArgTypeAt((pos));
        Q_ASSERT(!argType.isEmpty());
        if(argType == WEB_ADDRESS_NAME){ return true; }
        if(argType == FILE_NAME){ return true; }
        if(argType == FOLDER_NAME){ return true; }
        return false;
    }

    bool atCustomArgChoicePoinst(int pos){
        if(pos!=m_itemPosition){ return false;}
        if(!usingCustomVerb()){ return false;}
        if(inputData[pos].isOrganizingArgChosen()){
            return false;
        }
        Q_ASSERT(argsToSubstitute().count()>1);
        return true;
    }

    //Only beyond any default args
    QList<CatItem> argsToSubstitute(){
        if(slotPosition() != slotCount()-1)
            { return QList<CatItem>(); }

        if(!usingCustomVerb())
            { return QList<CatItem>(); }

        if(m_itemPosition == m_verbIndex)
            { return QList<CatItem>(); }

        if(!getArgItemAt(m_itemPosition).isEmpty())
            { return QList<CatItem>(); }

        return getRemainingCustomArgs();
    }

    QList<CatItem> getRemainingCustomArgs(bool dummyCurrent=true){
//        QHash<QString, EntryField> anticipatedValues = asFieldValueHash();
//        QHash<QString, EntryField> extra = anticipatedValues;
//        int l = inputData.count()-1;
//        if(dummyCurrent && !inputData[l].getOrganizingArg().isEmpty() && getArgValue(l).isEmpty()){
//            extra[getArgNameAt(l)].value = "dummy";
//        }
//        getVerb().setFields(extra);
        QList<CatItem> args = getVerb().getUnfilledArguments();

        //getVerb().setFields(anticipatedValues);
        if(!dummyCurrent){
            QString key = getArgNameAt(m_itemPosition);
            CatItem curItem = getVerb().itemFromField(key);
            args.push_front(curItem);
        }
        return args;
    }

    QList<CatItem> getCustomArgs(){
        QHash<QString, EntryField> anticipatedValues = asFieldValueHash();
        getVerb().getFieldValues(anticipatedValues);
        QList<CatItem> args = getVerb().getArguments();
        return args;
    }

    bool shouldExecuteNow(){
        if(!getRemainingCustomArgs().isEmpty()){
            return false;
        }
        if(getVerb().getCustomValue(EXECUTE_IF_FULLFILLED_KEY)==0){
            return false;
        }
        return true;
    }

    QList<CatItem> nounList(){
        QList<CatItem> res;
        if(m_nounsBeginIndex!=-1){
            for(int i=m_nounsBeginIndex;  i <= m_nounsEndIndex; i++){
                CatItem base = inputData[i].getCurrentItem();
                if(base.isEmpty()){
                    base = inputData[i].getOrganizingArg();
                    if(!inputData[i].getUserKeys().isEmpty()){
                        base.setName(inputData[i].getUserKeys());
                    }
                }
                if(!base.isEmpty()){ res.append(base); }
            }
        }
        return res;
    }

    int slotCount(){ return inputData.count(); }
    int slotPosition(){ return m_itemPosition; }

    int nounBeginSlot(){ return m_nounsBeginIndex;}
    int nounEndSlot(){ return m_nounsEndIndex;}

    int fullfilledFieldsEnd(){
        return m_itemPosition;
//        return m_nounsEndIndex;
//        if(!atUserDataField()){
//            return m_nounsEndIndex;
//        }
//
//        int lastFullFilled=m_nounsBeginIndex;
//        for(int i= m_nounsBeginIndex; i<=m_nounsEndIndex;i++){
//            const QString argName = getArgNameAt(i);
//            if(getVerb().atFieldGroupEnd(argName) && i>m_nounsBeginIndex){
//                lastFullFilled = i-1;
//            }
//        }
//        return lastFullFilled;
    }


    int operationSlot(){ return m_opIndex;}


    void setLabel(QString l){
        inputData[m_itemPosition].setLabel(l);
    }

    void setArgValue(QString key, QString value){
        Q_ASSERT(usingCustomVerb());
        Q_ASSERT(m_itemPosition>-1);
        int oldPos = m_itemPosition;
        for(int i=m_nounsBeginIndex; i <= m_nounsEndIndex && i>=0; i++){
            const QString argName = getArgNameAt(i);
            if(argName == key){
                inputData[i].setUserKeys(value);
                //rescanSlot();
                return;
            }
        }
        getVerb().setField(key, value);
        addDefaultArguments();




//        if(!inputData[m_itemPosition].getOrganizingArg().isEmpty()){
//            addSlot();
//        }
//
//        CatItem newArgItem = getVerb().itemFromField(key);
//        setItem(newArgItem);
//        setUserKeys(value);
        m_itemPosition = oldPos;
    }

    void setItem(CatItem item){
        qDebug() << "itemargList - setItem: " << item.getPath() << ":" << m_itemPosition;
        inputData[userInSlot()].setExpandedSearchStr("");

        m_slotAdded=false;
        if(m_opIndex==m_itemPosition){
            if(isOperationChosen()){
                inputData[m_itemPosition].setCurrentItem(item);
                qDebug() << "setItem arg set: " << inputData.count();
                return;
            } else if(item.getActionType() != (int)CatItem::OPERATION_ACTION){
                inputData[m_itemPosition].setOrganizingArg(CatItem());
                qDebug() << "setItem removing m_opIndex: " << inputData.count() << ":" << item.getPath();
                Q_ASSERT(m_opIndex>-1);
                m_opIndex=-1;
            }
        }
        if(!customVerbChosen()){ //m_customVerbUserInsertPoint==m_itemPosition && m_customVerbUserInsertPoint=-1;
            clearCustomVerb();
        }

        if(usingCustomVerb() && m_customVerbUserInsertPoint!=m_itemPosition){
            if(!inputData[m_itemPosition].getOrganizingArg().isEmpty()
                && !item.isCustomVerb()){
                //inputData[m_itemPosition].setOrganizingArgChosen(true);
                inputData[m_itemPosition].setCurrentItem(item);
            }
        } else if(item.isCustomVerb() && hasVerb()){
            clearSlotEntry();
        }
        
        if(item.hasLabel(PLACE_HOLDER_ITEM_KEY)){
            clearSlotEntry();
        } else if(item.getIsVerbArg() ){
            Q_ASSERT(m_itemPosition != m_verbIndex);
            inputData[m_itemPosition].setOrganizingArg(item);
        } else if(item.getActionType() == CatItem::OPERATION_ACTION ){
            inputData[m_itemPosition].setCurrentItem(CatItem());
            inputData[m_itemPosition].setOrganizingArg(item);
            Q_ASSERT(inputData[m_itemPosition].getOrganizingArg().getActionType()
                     ==CatItem::OPERATION_ACTION);
            Q_ASSERT(!isOperationChosen());
            m_opIndex = m_itemPosition;
        } else if(item.getActionType() == CatItem::VERB_ACTION) {
            if(item.isCustomVerb()) {
                inputData[m_itemPosition].setCurrentItem(CatItem());
                addCustomVerb(item);
            } else {
                inputData[m_itemPosition].setCurrentItem(item);
            }
        } else {
            if(m_itemPosition == m_verbIndex || m_verbIndex==-1){
                clearCustomVerb();
            }
            inputData[m_itemPosition].setCurrentItem(item);
        }
        qDebug() << "rescanSlot: " << m_itemPosition;
        rescanSlot();
    }

    void setItem(QString itemPath){
        Q_ASSERT(checkIntegrity());
        CatItem item = inputData[m_itemPosition].getItemByPath(itemPath);
        setItem(item);
        Q_ASSERT(m_itemPosition>=0);
    }

    void removeItem(CatItem item){
        inputData[m_itemPosition].removeItem(item);

    }

    bool iconJustAdded(){
        return m_slotAdded;
    }

    bool iconJustRemoved(){
        return (m_slotRemoved!=-1);
    }

    void addCustomVerb(CatItem verb){
        Q_ASSERT(!isOperationChosen());
        clearCustomVerb();
        if(m_opIndex==m_itemPosition){
            qDebug() << "addCustomVerb removing m_opIndex: " << inputData.count() << ":" << verb.getPath();
            m_opIndex=-1;
        }

        VerbItem vi(verb);
        QHash<int,CatItem> argMatchList = getArgMatchesInOrder(vi);

        for(int i=0;i<inputData.count();i++){
            if(m_opIndex==i){continue;}
            if(!argMatchList.contains(i)){continue;}
            inputData[i].setOrganizingArg(argMatchList[i]);
        }
        Q_ASSERT(m_itemPosition == inputData.count()-1);
        InputData id;
        id.setUserKeys(inputData[m_itemPosition].getUserKeys());
        inputData[m_itemPosition].setUserKeys("");

        id.setOrganizingArg(vi);
        //id.setCurrentItem(vi);
        int pos=0;
        if(m_opIndex==0){ pos++; }
        inputData.insert(pos,id);
        m_verbIndex = pos;
        m_itemPosition++;
        if(m_nounsBeginIndex>-1){
            m_nounsBeginIndex++;
            m_nounsEndIndex++;
        }
        if(m_opIndex>m_verbIndex){
            Q_ASSERT(m_opIndex>-1);
            m_opIndex++;
        }
        m_slotAdded = true;

        inputData[m_verbIndex].setOrganizingArg(vi);
        inputData[m_verbIndex].setOrganizingArgChosen(false);

        for(int i=0;i<inputData.count();i++){
            setFieldToCustomVerb(i);
        }
        m_customVerbUserInsertPoint = m_itemPosition;

        QHash<QString, EntryField> asValues = asFieldValueHashEx(argMatchList);
        getVerb().getFieldValues(asValues);
    }

    void addExtraArguments(){
        VerbItem& vi = getVerb();
        QList<CatItem> verbArgs = vi.getArguments();
        QList<CatItem> verbAccu = verbArgs;

        for(int i=0; i<inputData.count();i++){
            if(m_opIndex==i){continue;}
            if(m_verbIndex==i){continue;}
            CatItem itemArg = inputData[i].getOrganizingArg();
            for(int j=0; j<verbArgs.count();j++){
                CatItem& vArg = verbArgs[j];
                QString vKey = vArg.getArgName();
                if(!verbAccu.contains(vArg)){ continue;}
                if(matchFieldToArg(vArg, inputData[i]) || itemArg.getName() == vKey){
                    if(itemArg.isEmpty()){
                        inputData[i].setOrganizingArg(vArg);
                    }
                    if(CatItem::atUserDataField(vArg.getArgType())){
                        if(inputData[i].getUserKeys().isEmpty()){
                            inputData[i].setUserKeys(vArg.getArgValue());
                        } else {
                            QString val = inputData[i].getUserKeys();
                            vi.setField(vKey,val);
                        }
                    } else if(!vi.getValueItem(vArg.getArgName()).isEmpty()){
                        if(inputData[i].getCurrentItem().isEmpty()){
                            inputData[i].setCurrentItem(
                                    vi.getValueItem(vArg.getArgName()));
                        } else {
                            CatItem curItem = inputData[i].getCurrentItem();
                            vi.setField(vArg.getArgName(),"", curItem.getPath(),curItem);
                        }
                    }
                    verbAccu.removeAll(vArg);
                    Q_ASSERT(!inputData[i].getOrganizingArg().isEmpty());
                    break;
                }
            }
        }

    }

    void addDefaultArguments(bool addArg=false){
        VerbItem& vi = getVerb();
        QList<CatItem> verbArgs = vi.getArguments();
        QList<CatItem> verbAccu = verbArgs;

        if(addArg && this->inputData.count()< verbArgs.count()+1){
            InputData d;
            inputData.append(d);
            m_itemPosition++;
        }
        Q_ASSERT(inputData.count()<= verbArgs.count()+1);

        int argIndex=0;
        int nounStartPos=argIndex;
        int posStart=m_itemPosition;
        for(; argIndex<inputData.count();argIndex++){
            if(m_opIndex==argIndex){
                nounStartPos++;
                continue;
            }
            if(m_verbIndex==argIndex){
                nounStartPos++;
                continue;
            }
            CatItem itemArg = inputData[argIndex].getOrganizingArg();
            bool matched=false;

            for(int j=0; j<verbArgs.count();j++){
                CatItem& vArg = verbArgs[j];
                if(!verbAccu.contains(vArg)){ continue;}
                QString vKey = vArg.getArgName();
                if((itemArg.isEmpty() && matchFieldToArg(vArg, inputData[argIndex]))
                    || itemArg.getArgName() == vKey){
                    if(itemArg.isEmpty()){
                        inputData[argIndex].setOrganizingArg(vArg);
                    }
                    if(CatItem::atUserDataField(vArg.getArgType())){
                        if(inputData[argIndex].getUserKeys().isEmpty()){
                            inputData[argIndex].setUserKeys(vArg.getArgValue());
                        }
                    } else if(!vi.getValueItem(vKey).isEmpty()){
                        if(inputData[argIndex].getCurrentItem().isEmpty()){
                            inputData[argIndex].setCurrentItem( vi.getValueItem(vKey));
                        }
                    }
                    if(!inputData[argIndex].getCurrentItem().isEmpty()
                        || (!inputData[argIndex].getUserKeys().isEmpty())){
                        if(posStart>=argIndex-1){
                            posStart=argIndex;
                        }
                    }

                    verbAccu.removeAll(vArg);
                    Q_ASSERT(!inputData[argIndex].getOrganizingArg().isEmpty());
                    matched =true;
                    break;
                }
            }
            Q_ASSERT(matched || verbArgs.count()==0);
        }
        argIndex=MIN(inputData.count()-1,argIndex);
        Q_ASSERT(argIndex<inputData.count());
        addExtraArg(verbAccu, argIndex);
        m_nounsBeginIndex = nounStartPos;
        m_nounsEndIndex = argIndex;
        Q_ASSERT(m_nounsEndIndex < inputData.count());
        Q_ASSERT(m_nounsBeginIndex > m_verbIndex);
    }

    void addExtraArg(QList<CatItem> verbAccu, int& argIndex){
        while(verbAccu.count()>0){

            CatItem vArg = verbAccu[0];
            Q_ASSERT(!vArg.isEmpty());

            EntryField fi = getVerb().atField(vArg.getArgName());
//            if(CatItem::atUserDataField(vArg.getArgType()) && fi.value.isEmpty()){
//                break;
//            }
            if(!fi.required){
                verbAccu.pop_front();
                break;
            }
            argIndex++;
            Q_ASSERT(argIndex<=inputData.count());
            if(argIndex==inputData.count()){
                inputData.append(InputData());
            }


            inputData[argIndex].setOrganizingArg(vArg);
            if(CatItem::atUserDataField(vArg.getArgType())){
                inputData[argIndex].setUserKeys(fi.value);
            } else {
                inputData[argIndex].setCurrentItem(fi.valueItem);
            }
            verbAccu.pop_front();
            Q_ASSERT(!inputData[argIndex].getOrganizingArg().isEmpty());
            if(fi.inOrder){ break; }
        }

    }

//    void setVerbArg(int i){
//        CatItem itemArg = inputData[i].getOrganizingArg();
//        CatItem vArg = getVerb().itemFromField(itemArg.getName());
//        if(CatItem::atUserDataField(vArg.getArgType())){
//            if(inputData[i].getUserKeys().isEmpty()){
//                inputData[i].setUserKeys(vArg.getArgValue());
//            }
//        } else if(!vi.getValueItem(vArg.getArgName()).isEmpty()){
//            if(inputData[i].getCurrentItem().isEmpty()){
//                inputData[i].setCurrentItem(
//                        vi.getValueItem(vArg.getArgName()));
//            }
//        }
//    }

//    void setOrganizingArgChosen(){
//        inputData[m_itemPosition].setOrganizingArgChosen();
//    }

    void refreshOperation(CatItem item){
        inputData[m_opIndex].setOrganizingArg(item);;
    }

    bool isCustomVerbChoosableByArgSig(){
        if(slotCount()==1)
            { return false; }

        if(m_verbIndex == -1)
            {return true;}

        if(m_verbIndex != m_itemPosition)
            { return false; }

        return true;
    }

    CatItem setDefaultVerbIfNeeded(){
        CatItem verb = getVerb();
        if((verb.isEmpty() )
            && (m_nounsBeginIndex == m_nounsEndIndex)
            && (m_nounsBeginIndex != -1)
            &&  ((m_nounsEndIndex ==m_itemPosition+1)
                 ||(m_nounsEndIndex ==m_itemPosition-1))){
            CatItem it =  inputData[m_nounsBeginIndex].getStandardVerb();
            setItem(it);
            return it;
        } else {
            return verb;

        }
    }

    CatItem getDefaultActionItem() {
        if(m_nounsBeginIndex!=-1 && m_nounsBeginIndex== m_nounsEndIndex ){
            if(getVerb().isEmpty()){
                CatItem navParent = inputData[m_nounsBeginIndex].navParent();
                if(navParent.getItemType()==CatItem::VERB){
                    return navParent;
                }
                return inputData[m_nounsBeginIndex].getCurrentItem().getItemDefaultVerb();
            }
        } else if(m_itemPosition==m_verbIndex && m_nounsBeginIndex==-1){
            if(!usingCustomVerb()){
                return CatItem::createRunPseudoItem();
            }
        }
        return CatItem();
    }

    CatItem getItemDefaultVerb() {
        if(m_nounsBeginIndex!=-1 && m_nounsBeginIndex== m_nounsEndIndex ){
            return inputData[m_nounsBeginIndex].getCurrentItem().getItemDefaultVerb();
        }
        return CatItem();
    }

    QList<CatItem> getNormalAssociation(){
        if((getVerb().isEmpty() || m_verbIndex == m_itemPosition)
            && (m_nounsBeginIndex == m_nounsEndIndex)
            && (m_nounsBeginIndex != -1)
            &&  (m_nounsEndIndex !=m_itemPosition)){
            return inputData[m_nounsBeginIndex].getCurrentItem().getAssociatedNormalVerbs();
        }
        return QList<CatItem>();
    }

    //This is a call-back function, called by the database
    CatItem::ItemType getOrganizeingType(){
        if(isOperationChosen()){
            return CatItem::TAG;
        }

        return (getFilterItem().getOrganizeingType());
    }

    //call-back function - decides what items get shown
    bool acceptItem(CatItem* item) {

        if( item->getIsHidden()){ return false;}
        if( item->getIsIntermediateSource()){ return false;}

        CatItem parentOrFilterItem = getFilterItem();

        //This match is fairly "approximate"
        if(getUserKeys().length() < GENERIC_KEYS_LIMIT && item->getName()!=getUserKeys()){
            //Don't show things like comments unless

            if(parentOrFilterItem.isEmpty() && item->getIsSubordinateItem()){ return false; }

            if(item->getIsDepricated()){ return false; }
            if(item->getItemType() == CatItem::OPERATION && slotCount()==1){ return false; }
            if(!item->isUserItem()){ return false;}
            if(item->getTagLevel() == CatItem::KEY_WORD &&
                    item->getRelevanceWeight()< HIGH_EXTERNAL_WEIGHT){
                return false;}
        }

        //Skip tags...
        if((int)item->getTagLevel() >= (int)CatItem::KEY_WORD ){

            //If we're in it
            if(item->getLowName().simplified()  == getUserKeys().toLower().simplified()){
                return false;}

            //If it was just created implicitly...
            CatItem::MatchType mt = item->getBestMatchType();
            if((int)mt <= (int)CatItem::EXTERNAL_INFO){
                time_t now = appGlobalTime(); //This is in seconds...
                if(now - item->getCreationTime() < TAG_DELAY_PERIOD){ return false;}}
        }

        if(item->getIsVerbArg()){
            if(!usingCustomVerb()){ return false; }
            if(m_verbIndex == m_itemPosition) { return false; }
        }

        if(!parentOrFilterItem.isEmpty() ){
            CatItem::ItemType organizingType = parentOrFilterItem.getOrganizeingType();


            //For the tagging operation
            if(organizingType == CatItem::TAG && parentOrFilterItem.getItemType()==CatItem::OPERATION){
                if(item->getTagLevel()!=CatItem::KEY_WORD){
                    return false;
                }
            }
            if(item->getItemType() == CatItem::ACTION_TYPE){
                if(organizingType == CatItem::TAG ){
                    return true;
                } else { return false;  }
            }

            if(item->getItemType() == CatItem::ORGANIZING_TYPE )
                { return (organizingType== CatItem::ORGANIZING_TYPE)||
                  (organizingType== CatItem::TAG); }

            if(CatItem::matchOrganizingTypes(organizingType, item->getItemType()))
                { return true; }
            if(organizingType== CatItem::TAG && item->getItemType()==CatItem::LOCAL_DATA_FOLDER)
                { return true; }

            if(organizingType!=CatItem::MIN_TYPE){
                if(item->getSortingType() !=CatItem::MIN_TYPE){
                    return (item->getSortingType() ==organizingType);
                }  else if(item->getItemType() !=organizingType) {
                    return false;
                }}
            if(parentOrFilterItem.getItemType() == CatItem::VERB){
                if(!(bool)parentOrFilterItem.getCustomValue(VERB_TAKES_URIS)){
                    if(item->getItemType() > CatItem::LOCAL_DATA_DOCUMENT
                       && !(item->hasLabel(FILE_CATALOG_PLUGIN_STR))){
                        return false;
                    }}

                QList<QString> actionCats = parentOrFilterItem.getActionParentTypes();
                if(actionCats.isEmpty()){
                    return false;
                }
                QString actionType = item->getMimeType();
                if(actionCats.contains(actionType)){
                    return true;
                }
                if(!usingCustomVerb()){
                    return false;
                }
            }

            if(!parentOrFilterItem.hasLabel(CHARACTERISTIC_SOURCE_LABEL)){
                if(item->hasParent(parentOrFilterItem))
                    { return true; }
            }
        } else {
            if(item->getIsSubordinateItem()){
                return false;
            }

            if(item->getItemType() == CatItem::ORGANIZING_TYPE
               || item->getItemType() == CatItem::ACTION_TYPE){
                return false;
            }
        }

        if(customVerbChosen()){
            if(item->getActionType() == CatItem::VERB_ACTION){
                return false;
            }
            QString argType = getArgTypeAt((m_itemPosition));
            if(!argType.isEmpty()){
                if(argType == WEB_ADDRESS_NAME){
                    if(!item->hasLabel(FIREFOX_PLUGIN_NAME)){
                        return false; } }
                if(argType == FILE_NAME){
                    if(!item->hasLabel(FILE_CATALOG_PLUGIN_STR)){
                        return false; } }
                if(argType == FOLDER_NAME){
                    if(!item->hasLabel(FILE_CATALOG_PLUGIN_STR)){
                        return false; } }
            }
        }

        if(inputData.count() ==1 ||
           (m_itemPosition == 0 && usingCustomVerb())
            ||
            (m_itemPosition == 1 && usingCustomVerb() && !customVerbChosen())) { return true; }
        if(!hasVerb()){ return true; }

        CatItem actionCategory = item->getTypeParent(BaseChildRelation::DATA_CATEGORY_PARENT);

        CatItem::ActionType type = item->getActionType();
        switch(type){
            case CatItem::OPERATION_ACTION:{
                if(m_itemPosition == m_opIndex)
                    {return true;}
                if(m_opIndex!=-1){
                    return false;
                }
                return (m_itemPosition ==inputData.count()-1);}
            case CatItem::VERB_ACTION:
                {return m_itemPosition == m_verbIndex ||
                 (usingCustomVerb() && !customVerbChosen());}

            case CatItem::NOUN_ACTION:{
                QList<CatItem> actionTypes = getVerb().getVerbActionCategories();
                return (verbTakesArgs()) && actionTypes.contains(actionCategory);}
            case CatItem::UNDEFINED_ACTION:
            default: Q_ASSERT(false); return false;
        }
    }

    QList<CatItem::ItemType> getOrganizingTypeList(){
        CatItem parentOrFilterItem = getFilterItem();
        if(parentOrFilterItem.isEmpty()){
            return QList<CatItem::ItemType>();
        }
        return parentOrFilterItem.getOrganizingTypeList();
    }

    bool verbTakesArgs(){
        Q_ASSERT(hasVerb());
        if(m_itemPosition >=m_nounsBeginIndex && m_itemPosition <= m_nounsEndIndex)
            { return true; }

        if(!usingCustomVerb()){
            return (m_verbIndex < m_nounsEndIndex || (m_nounsEndIndex==-1));
        } else {
            return getVerb().getArguments().count() > (m_nounsEndIndex - m_nounsBeginIndex);
        }
    }

    ListItem previewItem(){
        //return ListItem(getArgItemAt(m_itemPosition));
        ListItem li(inputData[m_itemPosition].getCurrentItem());
        return li;
    }


    bool canRun(QString* err=0){
        if(m_opIndex != -1){
            if(m_nounsBeginIndex!=-1 || m_verbIndex!=-1){
                return !getUserKeys().isEmpty();
            } else { return false;}
        }
        if(m_verbIndex!=-1){
            if(usingCustomVerb()){
                return getVerb().fieldsSatisfied(err);
            } else {return true;}
        } else if(m_nounsEndIndex != -1){
            if(m_nounsBeginIndex == m_nounsEndIndex){
                if(!inputData[m_nounsBeginIndex].getCurrentItem().isEmpty()) {
                    if(inputData[m_nounsBeginIndex].getCurrentItem().getItemDefaultVerb().isEmpty()){
                        *err = NO_ASSOCIATED_APP_MESSAGE;
                        return false;
                    }
                    return true;
                }
            }
        }
        //Empty message
        return false;
    }

    CatItem::ActionType itemTypeAt(){
        if(!inputData[m_itemPosition].getOrganizingArg().isEmpty()){
            return inputData[m_itemPosition].getOrganizingArg().getActionType();
        } else if(!inputData[m_itemPosition].getCurrentItem().isEmpty()){
            return inputData[m_itemPosition].getCurrentItem().getActionType();
        } else {
            return CatItem::UNDEFINED_ACTION;
        }
    }

    void rescanSlot(){

        CatItem::ActionType type = itemTypeAt();

        if(type == CatItem::VERB_ACTION){
            m_verbIndex = m_itemPosition;
        } else if(m_itemPosition == m_verbIndex ){
            m_verbIndex = -1;
        }

        //We assign more than one state...
        if (type ==CatItem::NOUN_ACTION ){
            addNounToSlot(m_itemPosition);
        } else if(m_itemPosition >= m_nounsBeginIndex && m_itemPosition <= m_nounsEndIndex){
            removeNounFromSlot(m_itemPosition);
        }
        if (type == CatItem::OPERATION_ACTION){
            m_opIndex = m_itemPosition;
            Q_ASSERT(inputData[m_itemPosition].getOrganizingArg().getActionType()==CatItem::OPERATION_ACTION);
        } else if(m_itemPosition ==m_opIndex && m_opIndex>-1 && !isOperationChosen()){
            qDebug() << "rescanSlot removing m_opIndex: " << m_itemPosition;
            m_opIndex = -1;
        }

        if(usingCustomVerb()){
            setFieldToCustomVerb(m_itemPosition);
        }
        Q_ASSERT(checkIntegrity());
    }

//Always call this inside an ASSERT
#ifndef QT_NO_DEBUG
    bool checkIntegrity(){
        Q_ASSERT(m_itemPosition>=0);
        Q_ASSERT(m_itemPosition < inputData.count());
        Q_ASSERT(m_verbIndex < m_nounsBeginIndex || m_verbIndex> m_nounsEndIndex
                 || (m_verbIndex == m_nounsBeginIndex && m_nounsBeginIndex==-1));
        Q_ASSERT(m_opIndex < m_nounsBeginIndex || m_opIndex> m_nounsEndIndex
                 || (m_opIndex == m_nounsBeginIndex && m_nounsBeginIndex==-1));
        Q_ASSERT(m_opIndex == 0 || (m_opIndex == inputData.count()-1)
                 || (m_opIndex ==-1));

        QSet<QString> argSet;

        for(int i=0; i< this->inputData.count();i++){
            VerbItem& vi = getVerb();
            if(usingCustomVerb()){
                //this->asFieldValueHash();
                QString key = getArgNameAt(i);
                if(inputData[i].getOrganizingArg().getIsVerbArg()){
                    Q_ASSERT(i>=m_nounsBeginIndex && i<=m_nounsEndIndex);
                }
                CatItem argItem = inputData[i].getOrganizingArg();
                if(customVerbChosen() && i != m_verbIndex){
                    Q_ASSERT(!argItem.isEmpty());
                    CatItem argWithinVerbItem = vi.itemFromField(argItem.getArgName());
                    Q_ASSERT(argWithinVerbItem.getArgName() == argItem.getName());

                }
                //QString value = this->getArgValue(i);
                if(!key.isEmpty()){
                    Q_ASSERT(!argSet.contains(key));
                    argSet.insert(key);
                    //Q_ASSERT(value == vh[key].value);
                }
            }

            if(i>=m_nounsBeginIndex && i<=m_nounsEndIndex){
                //We can have empty arguments to custom verbs...
                Q_ASSERT(inputData[i].getCurrentItem().isEmpty()
                         || inputData[i].getCurrentItem().getActionType()==CatItem::NOUN_ACTION);
            } else if(i==m_verbIndex){
                Q_ASSERT(inputData[i].getOrganizingArg().getActionType()==CatItem::VERB_ACTION ||
                         inputData[i].getCurrentItem().getActionType()==CatItem::VERB_ACTION);
                if(inputData[i].getOrganizingArg().isCustomVerb()){
                    Q_ASSERT(i==0);
                }
            } else if(i==m_opIndex){
                Q_ASSERT(inputData[i].getOrganizingArg().getActionType()==CatItem::OPERATION_ACTION);
                Q_ASSERT(i==0 || i==inputData.count()-1);
            } else if (i!=inputData.count()-1){
                Q_ASSERT(usingCustomVerb());
            }
        }
        return true;
    }
#endif

    void setFieldToCustomVerb(int i){
        QString key = getArgNameAt(i);
        if(!inputData[i].getCurrentItem().isEmpty()){
            CatItem val = inputData[i].getCurrentItem();
            getVerb().setField(key, val.getPath(),val.getPath(),val);
        } else {
            QString val = inputData[i].getUserKeys();
            getVerb().setField(key, val);
        }
    }

    void addNounToSlot(int i){

        if(m_nounsBeginIndex==-1){
            m_nounsBeginIndex = i;
        } else {
            m_nounsBeginIndex = MIN(i, m_nounsBeginIndex);
        }
        m_nounsEndIndex = MAX(i, m_nounsEndIndex);
    }


    void removeNounFromSlot(int i){
        if(m_nounsBeginIndex == m_nounsEndIndex){
            Q_ASSERT(m_nounsBeginIndex ==i);
            m_nounsBeginIndex = -1;
            m_nounsEndIndex = -1;
        } else if(i == m_nounsBeginIndex) {
            Q_ASSERT(m_nounsEndIndex > m_nounsBeginIndex);
            m_nounsBeginIndex++;
        } else if(i == m_nounsEndIndex){
            Q_ASSERT(m_nounsEndIndex > m_nounsBeginIndex);
            m_nounsEndIndex--;
        } else{
            Q_ASSERT(false);
            m_errorState = ILLEGAL_ORDER_MESSAGE;
        }
    }

//    bool tryClearOrganizingArg(){
//        if(!usingCustomVerb()){ return false; }
//
//        if(m_itemPosition == m_verbIndex){
//            clearCustomVerb();
//        } else {
//            if(!getArgItemAt(m_itemPosition).isEmpty()){
//                inputData[m_itemPosition].setOrganizingArg( CatItem());
//                Q_ASSERT(getArgItemAt(m_itemPosition).isEmpty());
//            } else {
//                m_slotRemoved = m_verbIndex;
//                clearCustomVerb();
//                if(inputData.length()>1){
//                    inputData.removeAt(m_slotRemoved);
//                }
//                m_itemPosition= MAX(0,m_itemPosition-1);
//            }
//        }
//        rescanSlot();
//        return true;
//    }

    void clearCustomVerb(){
        m_customVerbUserInsertPoint = -1;
        if(usingCustomVerb() && inputData.count()>1){
            QString vKey = inputData[m_verbIndex].getUserKeys();
            m_itemPosition = MAX(m_itemPosition-1,0);
            if(m_nounsBeginIndex>-1)
                { m_nounsBeginIndex--; }
            if(m_nounsEndIndex>-1)
                { m_nounsEndIndex--; }
            if(m_opIndex>m_verbIndex) {
                m_opIndex--;
                Q_ASSERT(m_opIndex>-1);
            }
            inputData.removeAt(m_verbIndex);
            m_verbIndex = -1;
            inputData[m_itemPosition].setUserKeys(vKey);
        }
        for(int i=0;i<inputData.count();i++){
            if(i!=m_opIndex){
                VerbItem vi;
                inputData[i].setOrganizingArg(vi);
            }
        }
        rescanSlot();
    }

    void clearItem(){
        if(m_itemPosition == inputData.count()-1) {
            clearSlotEntry();
            inputData[m_itemPosition].clearAll();
        }

        if(inputData[m_itemPosition].isOrganizingArgChosen()){
            if(inputData[m_itemPosition].getCurrentItem().isEmpty()){
                inputData[m_itemPosition].setOrganizingArgChosen(false);
            }
            inputData[m_itemPosition].setCurrentItem(CatItem());
        } else {
            if(m_itemPosition <= m_nounsEndIndex && m_itemPosition >=m_nounsBeginIndex){
                QString argName = getArgNameAt(m_itemPosition);
                getVerb().removeInput(argName);
            } else if(m_itemPosition == m_verbIndex){
                m_verbIndex =-1;
            }
            inputData[m_itemPosition].setCurrentItem(CatItem());
            inputData[m_itemPosition].setOrganizingArg(  CatItem());
        }
        rescanSlot();
        Q_ASSERT(checkIntegrity());
    }

    void clearUnMatchItem(){
        inputData[m_itemPosition].clearUnmatchedItem();
        rescanSlot();
    }

    void clearSlotEntry(){
        m_itemPosition = inputData.count()-1;
        if(m_verbIndex == m_itemPosition)
            { m_verbIndex = -1; }
        if(m_nounsEndIndex==m_itemPosition ){
            if(m_nounsEndIndex > m_nounsBeginIndex){
                Q_ASSERT(m_nounsBeginIndex >-1);
                m_nounsEndIndex--;
            } else {
                Q_ASSERT(m_nounsBeginIndex == m_nounsEndIndex);
                m_nounsBeginIndex = -1;
                m_nounsEndIndex =-1;
            }
        }
        if(m_itemPosition == m_opIndex){
            qDebug() << "clearSlotEntry removing m_opIndex: " << inputData.count() ;
            m_opIndex = -1;
        }
        if(!usingCustomVerb()){
            inputData[m_itemPosition].setOrganizingArg(CatItem());
        }
        inputData[m_itemPosition].setCurrentItem(CatItem());
    }

    bool canAddSlot(QString* errorMsg){
        *errorMsg = "";
        if(m_itemPosition ==m_opIndex){
            return !inputData[m_opIndex].isOrganizingArgChosen();
        }
        if(usingCustomVerb()){ //If you've got a custom verb, you've it's args plus an operation
            if(!customVerbChosen()){ return true;}

            int maxLength = getVerb().getArguments().length();
            maxLength = (m_opIndex !=-1)? maxLength+1 : maxLength;
            if((m_nounsEndIndex - m_nounsBeginIndex) >= maxLength){
                if(errorMsg){
                    *errorMsg = NO_MORE_ARGUMENTS;
                }
                return false;
            }
            if(errorMsg){
                *errorMsg = ITEM_MISSING_ERROR;
            }
            if(fieldInputType()==USER_TEXT){
                return !getUserKeys().isEmpty();
            } else if(fieldInputType()==USER_TEXT_OR_EXPLICIT_SELECT ){
                return !getUserKeys().isEmpty() || !inputData[inputData.count()-1].getCurrentItem().isEmpty();
            } else {
                return !inputData[inputData.count()-1].getCurrentItem().isEmpty();
            }
            //If noun is a first, then verb, you have a space for an operation
        } else if(inputData.length()<=1){
            return true;
        } else if(inputData[inputData.count()-1].getUserKeys().isEmpty()
            && inputData[inputData.count()-1].getCurrentItem().isEmpty()){
            return false;
        } else if(m_nounsEndIndex!=-1 && m_nounsEndIndex < m_verbIndex) {
            return m_opIndex < m_verbIndex;

        //If a "normal" verb is first, you can add as many arguments as you like...
        } else if(m_verbIndex !=-1 && m_verbIndex < m_nounsEndIndex ){
            return true;
        } else if(m_verbIndex ==-1 && inputData.count() >1 ){
            return false;
        }
        return true;

    }

    void addSlot(){
        rescanSlot(); //This should be "idempotent"
        if(m_opIndex!= -1 ){
            //Q_ASSERT();
            if(!inputData[m_opIndex].isOrganizingArgChosen()){
                inputData[m_opIndex].setOrganizingArgChosen(true);
                inputData[m_opIndex].clearListItems();
                Q_ASSERT(checkIntegrity());
                Q_ASSERT(isOperationChosen());
                return;
            }
        } else if(usingCustomVerb() &&
              !customVerbChosen()){
            setCustomVerbChosen();
            Q_ASSERT(checkIntegrity());
            Q_ASSERT(customVerbChosen());
            return;
        }

        if(inputData[inputData.count()-1].getCurrentValue().isEmpty()
                && inputData[inputData.count()-1].getOrganizingArg().isEmpty()){
            addNounToSlot(inputData.count()-1);
        }
        if(usingCustomVerb() ){
            Q_ASSERT(m_itemPosition == inputData.count()-1);
            inputData[m_itemPosition].setOrganizingArgChosen();
        }

        if(usingCustomVerb() ){
            addDefaultArguments(true);
            inputData[m_itemPosition].setOrganizingArgChosen();
        } else {
            InputData d;
            inputData.append(d);
            m_itemPosition = inputData.count()-1;
        }
        //Q_ASSERT(!usingCustomVerb());
        Q_ASSERT(checkIntegrity());
    }

    InputData& at(int i =-1) {
        if(i == -1){
            return inputData[m_itemPosition];
        } else {
            return inputData[i];
        }
    }

    //bool insertVerbAndImplicitArg(CatItem* it, int argNum=0);

    void clearAll(){
        inputData.clear();
        InputData id;
        inputData.append(id);

        qDebug() << "clearall removing m_opIndex" ;
        m_opIndex =-1;
        m_verbIndex =-1;
        m_nounsBeginIndex =-1;
        m_nounsEndIndex =-1;
        m_slotRemoved =-1;
        m_slotAdded =false;
        m_itemPosition = inputData.count()-1;
        m_searchLevel=0;
        m_customVerbUserInsertPoint=-1;
    }

    void clearKeys(){ inputData[m_itemPosition].setUserKeys(""); }


    //bool takesAnyKey();


    //used in fulltext plugin...
    CatItem getContextType(QString label){
        if(inputData[m_itemPosition].navParent().hasLabel(label)){
            return inputData[m_itemPosition].navParent();
        }
        if(m_itemPosition > m_nounsBeginIndex && m_nounsBeginIndex > -1 && m_verbIndex ==-1){
            if(inputData[m_nounsBeginIndex].getCurrentItem().hasLabel(label)){
                return inputData[m_nounsBeginIndex].getCurrentItem();
            }
        }
        return CatItem();
    }

    //used in fulltext plugin...
    CatItem getContextItem(QString label){
        return getContextType(label);
    }

    CatItem getParentContextItem(){
        return inputData[m_itemPosition].contextItem();
    }

    QList<ListItem> getOrganizingFilterItems(int itemsAllowed, int charsAvail, int minWeight = 0){
        if((!customVerbChosen() && m_itemPosition==1)
            || !m_itemPosition==m_nounsBeginIndex
            || inputData.length()==1){
            return inputData[m_itemPosition].getFilterItems(itemsAllowed, charsAvail, minWeight );
        } else {
            return QList<ListItem>();
        }
    }

    QList<ListItem> getOrganizingSubFilterItems(int itemsAllowed, int charsAvail, int minWeight = 0){
        if(!usingCustomVerb() && inputData.length()==1){
            return inputData[m_itemPosition].getFilterSubItems(itemsAllowed, charsAvail, minWeight );
        } else {
            return QList<ListItem>();
        }
    }

    void setOrganzingItems(QList<ListItem> items){
        inputData[m_itemPosition].addOrganizingItems(items);
    }

    //Ignores operations
    QString getCommandLine(){
        QString cmd;
        if(m_verbIndex != -1){
            cmd = getVerb().getCommandLine();
            if(!usingCustomVerb() && m_nounsBeginIndex>-1){
                for(int i= m_nounsBeginIndex; (i<= m_nounsEndIndex);i++){
                    if(inputData[i].getCurrentItem().getArgPath() == RUN_PSEUDO_ITEM_PATH){
                        break;
                    } else{
                        cmd += QString(" ") + inputData[i].getCurrentItem().getArgPath();
                    }
                }
            }
        } else if (m_nounsBeginIndex> -1){
            if(m_nounsBeginIndex < m_nounsEndIndex){
                return "";
            }
            CatItem it = inputData[m_nounsBeginIndex].getCurrentItem();
            CatItem navParent = inputData[m_nounsBeginIndex].navParent();
            if(navParent.getItemType()==CatItem::VERB){
                return navParent.getCommandLine() + " " + it.getPath();
            } else {
                return it.getCommandLine();
            }
        }
        return cmd;
    }

    //Ignores operations
    CatItem getStatementObject(){
        if(m_verbIndex != -1){
            if(m_nounsBeginIndex<0){
                return getVerb();
            } else {
                return asStateItem();
            }
        }
        if(m_nounsBeginIndex>-1){
            return inputData[m_nounsBeginIndex ].getCurrentItem();
        }
        return CatItem();
    }

    QHash<QString, CatItem> getUserInputFields(){
        QHash<QString, CatItem> res;
        if(usingCustomVerb()){
            res = getVerb().getArgHash();
        }
        return res;
    }

    QList<QString> getKeyWords() {
        return inputData.last().getKeyWords();
    }

public:

    QHash<QString, EntryField> asFieldValueHashEx(QHash<int, CatItem> fieldSlotHash,bool fullPath=true){
        QHash<QString, EntryField> res;
        for(int i=0;i<inputData.count();i++){
            if(fieldSlotHash.contains(i)){
                if(!inputData[i].getCurrentValue().isEmpty()){
                    if(fullPath){
                        res[fieldSlotHash[i].getName()].value = inputData[i].getCurrentValue().getPath();
                        res[fieldSlotHash[i].getName()].valueItem = inputData[i].getCurrentItem();
                    } else {
                        res[fieldSlotHash[i].getName()].value = inputData[i].getCurrentValue().getName();
                        res[fieldSlotHash[i].getName()].valueItem = inputData[i].getCurrentItem();
                    }
                } else {
                    res[fieldSlotHash[i].getName()].value = inputData[i].getUserKeys();
                }
            }
        }
        return res;
    }

    QHash<QString, EntryField> asFieldValueHash(bool includeTag=false, bool fullPath=true){
        QHash<QString, EntryField> res;
        QString key;
        for(int i=0;i<inputData.count();i++){
            if(!inputData[i].getOrganizingArg().isEmpty()
                    &&
                    !inputData[i].getOrganizingArg().isCustomVerb()){
                if(i == m_opIndex && !includeTag){
                    continue;
                }

                //key = inputData[i].getOrganizingArg().getName();
                key = getArgNameAt(i);
                if(!key.isEmpty() ) {
                    res[key].value = getArgValue(i,fullPath);
                    res[key].valueItem = inputData[i].getCurrentItem();
                }
            }
        }
        return res;
    }

//    QHash<QString, QString> getUserFields(bool includeTag){
//        QHash<QString, QString> userFields;
//        for(int i=0;i <inputData.count(); i++){
//            if(i == m_opIndex && !includeTag){
//                continue;
//            }
//
//            if(!inputData[i].getCurrentItem().isEmpty())
//                { continue; }
//
//            QString val = inputData[m_verbIndex].getUserKeys();
//            QString argName = getArgNameAt(i);
//            if(argName.isEmpty()){continue;}
//
//            userFields[argName] = val;
//        }
//        return userFields;
//    }

    CatItem asStateItem(bool includeTag = true){
        QHash<QString, EntryField> userInputFields = asFieldValueHash(includeTag);
        QHash< QString, EntryField>::iterator i = userInputFields.begin();
        QHash< QString, QString> res;
        for(;i != userInputFields.end();i++){
            res[i.key()] = i.value().value;
        }

        CatItem it( addPrefix(HISTORY_PREFIX ,
            (asSerializedString(includeTag) + QString::number(stringHashHash(res) ))));
        it.setName(asFormattedString());
        CatItem histSrc(HISTORY_SRC_PATH);
        it.setCustomPluginInfo(HISTORY_USER_INPUT_KEY, picklHashToEscdString(res));
        return it;
    }

    void loadHistoryEvent(HistoryEvent he, QHash<QString, CatItem> theRealItems){
        clearAll();
        QList<QString> fields = splitEscdString(he.itemExpr);
        for(int i=0;i <fields.count(); i++){
            CatItem it = theRealItems[fields[i]];
            if(it.getTakesAnykeys()){
                if(!inputData[i].getOrganizingArg().isEmpty())
                    { this->addSlot(); }
            } else {
                if(!inputData[i].getCurrentValue().isEmpty())
                    { this->addSlot(); }
            }
            setItem(it);
        }
    }

    QString asSerializedString(bool includeTag){
        Q_ASSERT(inputData.count() >0);
        if (inputData.count() ==1){
            return inputData[0].getCurrentItem().getPath();
        } else {
            QStringList paths;
            bool atOrgArt=true;
            for(int i=0; i <inputData.count();i++){
                if(i == m_opIndex && !includeTag){
                    continue;
                }
                if(atOrgArt){
                    VerbItem arg = inputData[i].getOrganizingArg();
                    if(!arg.isEmpty()){
                        paths.append(arg.getPath());
                    }
                    atOrgArt =false;
                } else{
                    VerbItem item = inputData[i].getCurrentItem();
                    if(!item.isEmpty()){
                        paths.append(item.getPath());
                    } else {
                        QString argName = this->getArgNameAt(i);
                        QString type = getVerb().getFieldType(argName);
                        CatItem item(addPrefix(USERTEXT_PREFIX,type));
                        paths.append(item.getPath());
                    }
                }
            }
            QString res = combineStrings(paths);
            res = addPrefix(COMPOUND_ITEM_PREFIX,res);
            return res;
        }
    }


    //void executeItemByHotkey(QKeyEvent* hotkey);

    CatItem hotkeyActivated(QChar k){
        return inputData[m_itemPosition].getItemByKey(k);
    }
    void executeItem();


    void popSlot(){
        //Remove only EXTRAS, ALWAYS keep the base
        if(m_customVerbUserInsertPoint == m_itemPosition){
            clearCustomVerb();
        }

        if(inputData.count() >1){
            clearSlotEntry();
            inputData.removeLast();
            m_itemPosition = MIN(m_itemPosition, inputData.count()-1);

        } else {
            clearItem();
            clearKeys();
        }
        if(inputData.count()==1){
            m_nounsBeginIndex =-1;
            m_nounsEndIndex =-1;
            if(inputData[m_itemPosition].getOrganizingArg().isCustomVerb()){
                inputData[m_itemPosition].setOrganizingArgChosen(false);
            }
        }
        Q_ASSERT(m_itemPosition>=0);
    }

    void popLastItem(){
        //Remove only EXTRAS, ALWAYS keep the base
        if(inputData.count() >1){
            clearSlotEntry();
            inputData.removeLast();
            m_itemPosition = MIN(m_itemPosition, inputData.count()-1);

        }
    }

    CatItem currentItem() {
        Q_ASSERT(m_itemPosition < inputData.count() &&(m_itemPosition>-1));
        return inputData[m_itemPosition].getCurrentItem();
    }

    CatItem& currentItemRef() {
        Q_ASSERT(m_itemPosition < inputData.count() &&(m_itemPosition>-1));
        return inputData[m_itemPosition].getCurrentItem();
    }

    int iconItemCount(){
        if(inputData.count()==1 && inputData[m_itemPosition].getCurrentItem().isEmpty()
            && inputData[m_itemPosition].getOrganizingArg().isEmpty()){
            return 0;
        }

        if(usingCustomVerb()){
            return 1;
        } else if(inputData.count()==1
                  && !getDefaultActionItem().isEmpty()){
            return 2;
        } else {
            return inputData.count();
        }
    }

    CatItem iconItem(int i){
        if(i < inputData.count()){
            return itemAtSlot(i);
        } else {
            Q_ASSERT(i=1);
            return getDefaultActionItem();
        }
    }

    CatItem itemAtSlot(int i){
        if(!inputData[i].getOrganizingArg().isEmpty()){
            return inputData[i].getOrganizingArg();
        }
        return inputData[i].getCurrentItem();
    }


//    CatItem getSlotIconItem() {
//        Q_ASSERT(m_itemPosition < inputData.count() &&(m_itemPosition>-1));
//        if(usingCustomVerb()){
//            if(m_itemPosition == 0){
//                inputData[m_itemPosition].getOrganizingArg();
//            } else {
//                return CatItem();
//            }
//        }
//
//
//        if(!inputData[m_itemPosition].getCurrentItem().isEmpty()){
//            return inputData[m_itemPosition].getCurrentItem();
//        } else if(!inputData[m_itemPosition].getOrganizingArg().isEmpty()){
//            return inputData[m_itemPosition].getOrganizingArg();
//        } else {
//            CatItem it("dummy");
//            it.setIsVerbArg(true);
//            return it;
//
//        }
//    }

//    CatItem getOrganVerbIcon() {
//        Q_ASSERT(usingCustomVerb());
//        m_slotAdded = false;
//        return getVerb();
//    }


    bool atCustomField(){
        if(inputData[m_itemPosition].getOrganizingArg().isEmpty()
                || m_itemPosition == m_verbIndex)
            { return false; }

        QString pluginPath = currentFieldPlugin(m_itemPosition);
        qDebug() << "plugin path" << pluginPath;
        if(pluginPath.isEmpty()){ return false;}

        QString typePath = getArgTypeAt((m_itemPosition));
        if(typePath.isEmpty()){ return false; }
        if(typePath == LONG_TEXT_NAME){ return false; }
        if(typePath == SHORT_TEXT_NAME){ return false; }
        if(typePath == EMAIL_ADDRESS_NAME){ return false; }
        if(typePath == YOUR_EMAIL_ADDRESS_NAME){ return false; }
        if(typePath == WEB_ADDRESS_NAME){ return false; }
        if(typePath == FILE_NAME){ return false; }
        if(typePath == FOLDER_NAME){ return false; }

        return true;
    }

    bool moveRight(){
        if(m_itemPosition < inputData.count()-1){
            m_itemPosition++;
            return true;
        } else {return false;}
    }

    bool moveLeft(){
        if(m_itemPosition > 0){
            m_itemPosition--;
            return true;
        } else {return false;}
    }

    bool atEnd(){
        Q_ASSERT(m_itemPosition < inputData.count() );
        Q_ASSERT(inputData.count() >0);
        return((m_itemPosition == inputData.count()-1));
    }

    void setFilterItem(ListItem filterItem){
        inputData[m_itemPosition].filterByItem(filterItem);
    }

    int getListItemCount()
        { return inputData[m_itemPosition].getListItemCount(); }

    //List items...
    void addSingleListItem(CatItem it, int pos)
        { inputData[m_itemPosition].addSingleListItem(it,pos); }

    void addListItemList(QList<CatItem> il ) {
        CatItem toAdd;
        toAdd = inputData[m_itemPosition].addListItemList(il);
        if(!getUserKeys().isEmpty() && fieldInputType()== NORMAL_DUAL_SELECT){
            if(il.length()==1 && toAdd== il[0] && toAdd.isCustomVerb()){
                toAdd.setMatchType(CatItem::DATA_INFERENCE);
                //inputData[m_itemPosition].setOrganizingArgChosen();
            }
            setItem(toAdd);
        }
        rescanSlot();
    }

    void appendListItemList(QList<CatItem> il) {
        inputData[m_itemPosition].intersperseListItemList(il);
    }

    void filterItems() {
        inputData[m_itemPosition].cullItemsForInsert();
        rescanSlot();
    }

    bool isExpanded(){ return inputData[userInSlot()].isExpanded(); }
    void setExpanded(bool e=true){ inputData[m_itemPosition].setExpanded(e);}

    QString getExpandedSearchStr(){return inputData[m_itemPosition].getExpandedSearchStr();}
    void setExpandedSearchStr(QString  str){
        inputData[m_itemPosition].setExpandedSearchStr(str);
    }

    void pushChildren(CatItem& parItem) {
        inputData[m_itemPosition].pushChildren(parItem);
        //inputData[m_itemPosition].addOrganizingIcons(QList<ListItem>());
        rescanSlot();
    }

    int popParent() {
        int r = inputData[m_itemPosition].popParent();
        CatItem curItem = inputData[m_itemPosition].getCurrentItem();
        setItem(curItem);
        rescanSlot();
        return r;
    }

    QList<ListItem> getFormattedListItems(int& selected, int itemsHigh, int charsAvail) {
        QList<ListItem> res;

        if(customVerbChosen()){
            for(int i=nounBeginSlot(); (i>0) && i<= nounEndSlot();i++){
                QString fieldName = this->getArgNameAt(i);
                QString fieldValue = this->getArgValue(i);
                QString fieldType = this->getArgTypeAt(i);
                if(!fieldValue.isNull()){
                    fieldName = htmlFontFormat(fieldName,UI_FIELD_FORMAT_STR) + fieldName;
                } else {
                    fieldName = htmlFontFormat(fieldName,UI_FIELD_FORMAT_STR) +
                                htmlFontFormat(fieldType, UI_FIELD_TYPE_FORMAT_STR);
                }
                CatItem it(fieldName);
                //it.setUseLongName();
                ListItem li(it);
                res.append(li);
            }
        }


        QList<ListItem> raw = inputData[m_itemPosition].getFormattedListItems(
                selected, itemsHigh, charsAvail);
        for(int i=0; i < raw.count(); i++){
            ListItem tempItem = raw[i];
            if(acceptItem(&tempItem)){
                res.append(tempItem);
            }
        }

        for(int i=0; i < res.count(); i++){
            ListItem litem = res[i];
            if(litem.getUseLongDescription()){
                QString relativizedPath =
                        litem.getFormattedPath(charsAvail * PATH_AS_NAME_CONTRACTION,true);
                litem.setTemporaryName(relativizedPath);
                QStringList sl = relativizedPath.split(PATH_SEP);
                if(sl.length()>1){sl.pop_back();}
                QString modRelPath = sl.join(PATH_SEP) + PATH_SEP + litem.getNameForEditLine();
                litem.setFormattedNameOveride(modRelPath);
                litem.setFormattedPathOveride(litem.getLongText());
            } else if(litem.getMatchType() == CatItem::USER_WORDS){
                litem.setTemporaryName(litem.getName() + ": " + getUserKeys());
            } else if(((litem.getActionType() == CatItem::VERB_ACTION || litem.getActionType() == CatItem::OPERATION_ACTION)
                        && (litem.getMatchType()== CatItem::DATA_INFERENCE))){
                QString actionDescription =
                        asFormattedString(litem,0,0,false);
                if(!actionDescription.isEmpty()){
                    litem.setFormattedPathOveride(actionDescription);
                }
            }
            if(m_itemPosition >0){
                litem.setCustomPluginValue(VERB_TAKES_NO_ARGS_KEY, (int)true);
            }
            res[i] = litem;
        }
        return res;
    }

    QList<CatItem> getListItems()
        { return inputData[m_itemPosition].getListItems(); }

    QList<CatItem> getStubbedItems() {
        QList<CatItem> rawItems =  inputData[m_itemPosition].getListItems();
        QList<CatItem> res;
        for(int i=0; i< rawItems.count();i++){
            if(rawItems[i].isStub()){
                res.append(rawItems[i]);
            }
        }
        return res;
    }

    ListItem& getListItem(int i )
        { return inputData[m_itemPosition].getListItem(i); }

    int getCurrentItemIndex()
        { return inputData[m_itemPosition].getCurrentItemIndex(); }

    ListItem getItemByPath(QString path)
        { return inputData[m_itemPosition].getItemByPath(path); }

    ListItem& getItemByPathRef(QString path)
    { return inputData[m_itemPosition].getItemByPathRef(path); }

    ListItem findItemInParent(QString path)
        { return inputData[m_itemPosition].findItemInParent(path); }

    void popToParentPosition(QString path)
        { inputData[m_itemPosition].popToParentPosition(path); }

    bool moveCurSelectionUp(){
        int i =  inputData[m_itemPosition].getCurrentItemIndex();
        if(i==-1){i=0;}
        int s =  inputData[m_itemPosition].getListItemCount();
        Q_ASSERT(i<s);
        if(i ==s-1){ return false; }
        i++;
        inputData[m_itemPosition].setCurrentItemByIndex(i);
        return true;
    }

    bool moveCurSelectionDown(){
        int i =  inputData[m_itemPosition].getCurrentItemIndex();
        if(i==-1){i=0;}
        int s =  inputData[m_itemPosition].getListItemCount();
        Q_ASSERT(i>=0);
        if(i ==0 || (i==s-1)){ return false; }
        i--;
        inputData[m_itemPosition].setCurrentItemByIndex(i);
        return true;
    }


    bool isExploringChildren()
        { return inputData[m_itemPosition].isExploringChildren(); }

    bool showFormattedItem(){
        if(fieldInputType()== USER_TEXT){ return false;}

        if(usingCustomVerb()){ return true; }

        return (!inputData[m_itemPosition].getCurrentItem().isEmpty()
                 ||
                 !inputData[m_itemPosition].getOrganizingArg().isEmpty());
    }

    int userInSlot(){
        if(this->usingCustomVerb() && !customVerbChosen()){
            return m_verbIndex;
        } else { return m_itemPosition; }
    }

    QString getUserKeys(int pos=-1){
        if(pos==-1){pos = userInSlot();}
        return inputData[pos].getUserKeys();
    }

    void setUserKeys(QString t) {
        if(inputData[userInSlot()].isExpanded()){
            inputData[userInSlot()].setExpandedSearchStr(t);
        } else {
            inputData[userInSlot()].setUserKeys(t);
        }
        setSearchLevel(0);
        rescanSlot();
    }

    void appendUserkeys(QString s) {
        if(this->isExpanded()){
            QString str = inputData[userInSlot()].getExpandedSearchStr();
            inputData[userInSlot()].setExpandedSearchStr(str + s);
        } else {
            inputData[userInSlot()].appendUserText(s);
        }
        setSearchLevel(0);
        rescanSlot();
    }

    void backspaceKey(int n=1){
        if(this->isExpanded()){
            QString keys = inputData[userInSlot()].getExpandedSearchStr();
            keys.truncate(keys.length()-1);
            inputData[userInSlot()].setExpandedSearchStr( keys);
        } else {
            setSearchLevel(0);
            inputData[m_itemPosition].cullItemsForInsert();
            inputData[m_itemPosition].clearListItems();
            inputData[userInSlot()].popKey(n);
            if(getUserKeys().isEmpty() && m_itemPosition==0 ){
                inputData[m_itemPosition].clearItem();
            }
        }
    }

    void modifyUserText(QString newKeys, int cursorLoc){
        //inputData[userInSlot()].modifyUserText(newKeys, cursorLoc);
        if(inputData[userInSlot()].isExpanded()){
            inputData[userInSlot()].setExpandedSearchStr(newKeys);
        } else {
            inputData[userInSlot()].modifyUserText(newKeys, cursorLoc);
        }
        rescanSlot();
    }

    bool selectingByKeys(){
        if(inputData.length()> 1 && usingCustomVerb() && m_itemPosition!=m_verbIndex){
            return false;
        }
        if(getUserKeys().length() >0){
            return true;
        }
        return false;

    }


    QString getPathText(int spaceAvailable){
        return inputData[m_itemPosition].getExplorePath(spaceAvailable);
    }


//    ListItem getStateMessageItem(int spaceAvailable){
//        QString msg = getPathText();
//        msg = msg.simplified();
//        if(!msg.isEmpty() && !getUserKeys().isEmpty())
//            { msg += QString(" ") + (UI_SEPARATOR_STRING) + " "; }
//        msg +=  getUserKeys();
//
//        if(msg.length()>spaceAvailable-10){
//            msg = DOTS_CHARS + msg.right(spaceAvailable-3);
//        }
//        ListItem msgItem("builtin://dummy");
//        msgItem.setName(msg);
//        msgItem.setFilterRole(CatItem::MESSAGE_ELEMENT);
//        return msgItem;
//    }

    bool currentItemMatchesAllStrings(){
        return (!inputData[m_itemPosition].getOrganizingArg().isEmpty() &&
                (inputData[m_itemPosition].getOrganizingArg().getTakesAnykeys()));
    }

    QString mainItemText(bool skipOp = false){

        QString formatedStr;
        if(usingCustomVerb() ) {
            VerbItem vi = getVerb();
            ListItem litem(vi);
            formatedStr = asFormattedString(litem);
        } else {
            formatedStr += basicItemText();
        }

        if(!skipOp){
            formatedStr = addOperationStr(formatedStr);
        }
        return (formatedStr.simplified());
    }

    QString basicItemText(){
        if(inputData[m_itemPosition].getCurrentItem().isEmpty()
            && inputData[m_itemPosition].getOrganizingArg().isEmpty()) {
            return inputData[m_itemPosition].getUserKeys();
        } else if(inputData[m_itemPosition].getCurrentItem().getMatchType()==CatItem::USER_WORDS){
            return htmlFontFormat(inputData[m_itemPosition].getCurrentItem().getName(),
                                          EMPHESIS_COLOR) + ": " +
                           inputData[m_itemPosition].getUserKeys();
        } else if(inputData.length()==1 && !inputData[m_itemPosition].getCurrentItem().isEmpty()){
            return inputData[m_itemPosition].getCurrentItem().getNameForEditLine();
        } else {
            return asFormattedStandardExpr();
        }
    }

    QString simpleItemText(){
        if(inputData[m_itemPosition].getCurrentItem().isEmpty()) {
            return inputData[m_itemPosition].getUserKeys();
        } else {
            return inputData[m_itemPosition].getCurrentItem().getNameForEditLine();
        }
    }

    QString asFormattedString(CatItem substituteItem= CatItem(),
                              int* beginFieldPos=0, int* endFieldPos=0, bool usePath=true){
        Q_ASSERT(m_itemPosition < inputData.count() &&(m_itemPosition>-1));

        bool atCustomVerb =false;
        if(usingCustomVerb() && substituteItem.isEmpty())
            { atCustomVerb = true; }

        if(usingCustomVerb() && substituteItem.getIsVerbArg())
            { atCustomVerb = true; }

        if( substituteItem.isCustomVerb())
            { atCustomVerb = true; }

        QString formatedStr;
        if(inputData.length()==1 && !usingCustomVerb()){
            if(substituteItem.isEmpty()){
                formatedStr += inputData[0].getCurrentItem().getNameForEditLine();
            } else {
                formatedStr += substituteItem.getNameForEditLine();
            }
        } else if(atCustomVerb){
            formatedStr += asFormattedCustomExpr(substituteItem,beginFieldPos,endFieldPos, usePath);
        } else {
            formatedStr += asFormattedStandardExpr(substituteItem);
        }
        if(substituteItem.getItemType() == CatItem::OPERATION){
            addOperationStr(formatedStr,substituteItem);
        } else {
            addOperationStr(formatedStr);
        }

        return (formatedStr);
    }



    QString addOperationStr(QString stmt, CatItem substituteItem=CatItem()){
        if(m_opIndex==-1){ return stmt; }
        QString tagName;
        if((m_itemPosition==m_opIndex ||
            m_opIndex==-1)  &&
                (substituteItem.getItemType()== CatItem::OPERATION ||
                substituteItem.getItemType()== CatItem::TAG)){
            tagName +=  substituteItem.getName();
        } else {
            tagName +=  formatItemByPos(m_opIndex);
        }
        if(substituteItem.getItemType()!= CatItem::OPERATION){
            QString opName = inputData[m_opIndex].getOrganizingArg().getName();
            Q_ASSERT(!opName.isEmpty());
            if(opName.simplified() == TAG_PHRASE.simplified()){
                tagName = (m_opIndex==0)? AS_PHRASE + tagName + stmt : stmt + AS_PHRASE + tagName;
            }
            tagName = (opName + " " + tagName).simplified();
        }
        return tagName;
    }



    QString asFormattedCustomExpr(CatItem substituteItem= CatItem(),
                                  int* beginFieldPos=0, int* endFieldPos=0, bool usePath=true){
        QString formatField ="";
        if(beginFieldPos!=0){
            Q_ASSERT(substituteItem.isEmpty());
            if(usingCustomVerb() && m_itemPosition !=m_verbIndex){
                formatField = inputData[m_itemPosition].getOrganizingArg().getName();
            }
        }
        QString formatedStr;
        if(substituteItem.isEmpty()){
            substituteItem = getVerb();
        }
        if(substituteItem.isCustomVerb()){
            VerbItem subVerb(substituteItem);
            QHash<int, CatItem> anticipatedArgs = getArgMatchesInOrder(subVerb);
            QHash<QString, EntryField> anticipatedValues = asFieldValueHashEx(anticipatedArgs);
            VerbItem vi(substituteItem);
            vi.getFieldValues(anticipatedValues);
            vi.setDefaultValues();
            //Use name instead of path in this case...
            formatedStr =  vi.getFormattedDescription(formatField,true, beginFieldPos, endFieldPos,usePath);
        } else if(substituteItem.getIsVerbArg()){
            QHash<int, CatItem> anticipatedArgs = getArgMatchesWithNewField(
                    m_itemPosition,substituteItem.getName());
            QHash<QString, EntryField> anticipatedValues = asFieldValueHashEx(anticipatedArgs);
            VerbItem vi(getVerb());
            vi.getFieldValues(anticipatedValues);
            vi.setDefaultValues();
            formatedStr =  vi.getFormattedDescription(formatField,true, beginFieldPos, endFieldPos);
        }
        return formatedStr;
    }

    QString asFormattedStandardExpr(CatItem substituteItem= CatItem()){
        QString formatedStr;
        for(int i=0; i< inputData.count();i++){
            if(i==m_opIndex) { continue; }
            if(i == m_verbIndex && i-1==m_nounsBeginIndex && i>0)
                { formatedStr += WITH_PHRASE; }
            if(i-1 == m_verbIndex && i==m_nounsBeginIndex && i>0)
                { formatedStr += TO_PHRASE; }
            if(i==m_nounsBeginIndex){
                formatedStr += OPEN_PHRASE;
            } else if(i==m_verbIndex && m_verbIndex < m_nounsBeginIndex){
                formatedStr += USE_PHRASE; }

            if(i== m_verbIndex && !substituteItem.isEmpty()){
                formatedStr +=  substituteItem.getName();
            } else {
                formatedStr +=  formatItemByPos(i);
            }
        }
        return formatedStr;

    }

    QString formatItemByPos(int j){
        QString formatedStr;
        if(j<0){
            return "";
        } else if(inputData[j].getCurrentItem().isEmpty()){
            if(!isLongText(inputData[j].getUserKeys())) {
                formatedStr +=  inputData[j].getUserKeys() + " ";
            } else {
                formatedStr +=  inputData[j].getUserKeys().left(UI_LONG_TEXT_LIMIT)
                                + "... ";
            }
        } else if(j == m_itemPosition) {
            return inputData[j].getCurrentItem().getNameForEditLine() + " ";
        } else {
            return inputData[j].getCurrentItem().getName() + " ";
        }
        return formatedStr;
    }

    QString getExplanationText(int* beginFieldPos=0, int* endFieldPos=0){
        //if((item_count() > 1 || currentItemMatchesAllStrings()));
        QString cmd;
        if(inputData[m_itemPosition].getCurrentItem().getMatchType() == CatItem::USER_WORDS){
            return inputData[m_itemPosition].getCurrentItem().getLongText();
        }

        if(usingCustomVerb()){
            cmd = asFormattedString(CatItem(),beginFieldPos, endFieldPos);
            //cmd = makeBold(cmd);
        } else if(m_opIndex!=-1) {
            if(m_verbIndex==-1){
                cmd = getOperationPart().getName();
                for(int i= m_nounsBeginIndex; i<= m_nounsEndIndex && i!=-1; i++){
                    cmd += QString(" ") + inputData[i].getCurrentItem().getName();
                }
            } else {
                cmd = getOperationPart().getName() + " " + getCommandLine();
            }
        } else {
            cmd = getCommandLine();
            if(cmd.isEmpty()){
                return NO_ACTION_MESSAGE;
            } else {
                return cmd;
            }
        }

        return cmd;
    }


//    QString keysOrItem(int i){
//        if(!inputData[i].getCurrentItem().isEmpty()){
//            return inputData[i].getCurrentItem().getName();
//        } else{
//            return inputData[i].getUserKeys();
//        }
//    }

    //This is for verb matching the various args in the list
    // It would get pathological if the list had more than 3-4 members
    // But can't have more than three...
    QList< QList < CatItem > > getCustomVerbSignature(){
        Q_ASSERT(m_itemPosition >=0);

        QList< QList < CatItem > > types;
        if(m_nounsBeginIndex==-1){
            if(inputData[m_itemPosition].getCurrentItem().isEmpty()){
                types.append(getPossibleArgTypes(m_itemPosition));
                return types;
            } else  { return QList< QList < CatItem > >();}
        }

        int begin = m_nounsBeginIndex;
        int end = m_nounsEndIndex;

        if(!inputData[m_itemPosition].getCurrentItem().isEmpty()
            || !inputData[m_itemPosition].getUserKeys().isEmpty()){
            if(m_itemPosition == begin-1){ begin --; }
            if(m_itemPosition == end+1){ end++; }}

        if(end==-1){ return QList< QList < CatItem > >(); }
        if(begin==-1){ return QList< QList < CatItem > >(); }

        for(int i=begin; i<= end;i++){
            QList< CatItem > slotTypes = getPossibleArgTypes(i);
            if(slotTypes.count()==0){
                return QList< QList < CatItem > >();
            }
            types.append(slotTypes);
        }
        return getPermuteList(types);
    }


    // turn {{x,y},{w,z},{a,b}} into {{x,w,a},{x,w,b}...}
    // Lots of recursion here, of course...
    QList< QList<CatItem> > getPermuteList(QList< QList<CatItem> > toPermute){
        QList< QList<CatItem> > res;
        if(toPermute.count() == 0 ){
            return res;
        }
        if( toPermute.count() == 1){
            for(int i=0; i< toPermute[0].count();i++){
                QList <CatItem>  ci;
                ci.append(toPermute[0][i]);
                res.append(ci);
            }
            return res;
        }

        Q_ASSERT(toPermute[0].count() >0);

        QList< QList<CatItem> > sub = getPermuteList(toPermute.mid(1));
        for(int i=0; i< toPermute[0].count();i++){
            for(int j=0; j< sub.count();j++){
                QList <CatItem>  ci;
                ci.append(toPermute[0][i]);
                ci.append(sub[j]);
                res.append(ci);
            }
        }
        return res;
    }

    bool isLabeledFromStart(){
        return (m_opIndex ==0);
    }

    bool isLabeledFromEnd(){
        return (m_opIndex ==inputData.count()-1 && inputData.count() >1);
    }

    QList < CatItem >  getPossibleArgTypes(int i){
        return getPossibleArgTypes(inputData[i]);
    }

    QList < CatItem >  getPossibleArgTypes(InputData inputDatum){
        QList<CatItem> l;

        if(!inputDatum.getCurrentItem().isEmpty()){
            QString typeName = inputDatum.getCurrentItem().getItemTypeAsString();
            l.append(CatItem::createActionItem(typeName));
            QList<CatItem> parents = inputDatum.getCurrentItem().getAllActionTypeParents();
            for(int j=0;j < parents.count(); j++){
                l.append(parents[j].getArgTypeItem());
            }
            l.append(ITEM_ARG_ITEM);
            if(inputDatum.getCurrentItem().isUpdatableSource()){
                l.append(SOURCE_TYPE_ITEM);
            }
            if(inputDatum.getCurrentItem().hasLabel(FILE_CATALOG_PLUGIN_STR)){
                l.append(FILE_TYPE_ITEM);
                if(inputDatum.getCurrentItem().getItemType() == CatItem::LOCAL_DATA_FOLDER){
                    l.append(FOLDER_TYPE_ITEM);
                }
            }
            return l;
        }

        QString userKeys = inputDatum.getUserKeys();

        if(isWebAdrField(userKeys)){
            l.append(WEBADDRESS_TYPE_ITEM);
            return l;
        }
        if(isNumberField(userKeys)){
            l.append(NUMBER_TYPE_ITEM);
            return l;
        }

        if(isEmailAdrField(userKeys)){
            l.append(EMAIL_ITEM);
            return l;
        }
        if(isLongText(userKeys)){
            l.append(LONG_TEXT_ITEM);
        }
        if(isShortText(userKeys) || l.count()==0){
            l.append(SHORT_TEXT_ITEM);
        }
        return l;
    }

    CatItem getOperationPart(){
        if(this->m_opIndex==-1){
            return CatItem();
        }
        CatItem op = inputData[m_opIndex].getOrganizingArg();
        return op;
    }

    CatItem getOperationTarget(){
        if(this->m_opIndex==-1){
            return CatItem();
        }
        CatItem opTarget = inputData[m_opIndex].getCurrentItem();
        return opTarget;
    }

//    CatItem opArgItem(){
//        Q_ASSERT(inputData.count() >0);
//        if (inputData.count() ==1){
//            return inputData[0].getCurrentItem().getPath();
//        } else {
//            QString pre(COMPOUND_ITEM_PREFIX);
//            QString res;
//            for(int i=0;i<inputData.count();i++){
//                res += QString("{") + inputData[0].getCurrentItem().getPath() + "}/";
//            }
//            res = addPrefix(pre,res);
//            return res;
//        }
//
//        return inputData[m_opIndex].getCurrentItem();
//    }

    QString opArgKeys(){
        if (inputData[m_opIndex].getCurrentItem().isEmpty()){
            return inputData[m_opIndex].getUserKeys();
        }  else if(m_opIndex < inputData.count()-1){
            return inputData[m_opIndex+1].getUserKeys();
        } else {return "";}
    }

    CatItem getActionPart(){
        if(getVerb().isEmpty()){
            return inputData[m_nounsEndIndex].getCurrentItem().getItemDefaultVerb();
        }
        return getVerb();
    }

    CatItem& lastItem() {
        Q_ASSERT(m_itemPosition < inputData.count() &&(m_itemPosition >-1));
        return inputData.last().getCurrentItem();
    }

    CatItem userItem(){
        VerbItem vi = this->getVerb();
        return vi.creatItem();
    }


};

#endif // ITEMARGLIST_H
