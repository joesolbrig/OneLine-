#ifndef ITEMARGLIST_H
#define ITEMARGLIST_H

#include <QList>

#include "input_data.h"
#include "input_list.h"
#include "catalog.h"

enum ParseState {
    START,
    OPERATION,
    VERB,
    NOUN,
    OP_VERB,
    OP_NOUN,
    VERB_FIRST_ACTION, //This also cover noun-first custom verb situations
    NOUN_FIRST_ACTION,
    DONE
};

enum ItemType {

    USER_STRING,
    EXISTING_OP,
    EXISTING_NOUN,
    EXISTING_STDVERB,
    EXISTING_CSTMVERB,
    OP_COMMAND


};



static const QList<QList<ParseState>> transitionMatrix = {
    {START, }

}

class ItemArgList : ItemFilter
{
    enum ParseState {
        START,
        OPERATION,
        VERB,
        NOUN,
        OP_VERB,
        OP_NOUN,
        VERB_FIRST_ACTION, //This also cover noun-first custom verb situations
        NOUN_FIRST_ACTION,
        OPVERB_FIRST_ACTION,
        DONE
    };

private:

    int itemPositionIndex;
    QList<InputData> inputData;


    bool awaitingFieldResolution();

    bool canOpTerminate();
    bool hasVerb();

    int opIndex;

    int verbIndex;
    int implicitArgIndex; //nth arg stand-in, usually 'default'

    ParseState m_parseState;


public:
    ItemArgList();

    //An existing item, of course
    bool acceptItem(CatItem it, Catalog* store) {
        ItemType type = it.getArgType();

        switch(m_parseState){
            case START: return true;

            case OPERATION: return (type != CatItem::OPERATION_ACTION);
            case VERB: return (type != CatItem::VERB_ACTION);
            case NOUN: return (type !=CatItem::NOUN_ACTION )
                        || store->existVerbWithSignature(nounList());
            case OP_VERB: return (type == CatItem::NOUN);
            case OP_NOUN: return (type ==CatItem::VERB_ACTION)
                        || store->existVerbWithSignature(nounList());
            case VERB_FIRST_ACTION: return (type ==CatItem::NOUN_ACTION)
                        && store->existVerbWithSignature(nounList());
            case NOUN_FIRST_ACTION: return (type != CatItem::OPERATION_ACTION);
            case DONE: return false;
        }
    }

    void addSlot(){

        ItemType type = at().getArgType();

        switch(m_parseState){
            case START:
                if(type == CatItem::VERB_ACTION){
                    verbIndex = itemPositionIndex;
                    m_parseState = VERB;

                } else if (type ==CatItem::NOUN_ACTION ){
                    m_parseState = NOUN;

                } else if (type ==CatItem::OPERATION_ACTION){
                    opIndex = itemPositionIndex;
                    m_parseState = OPERATION;
                }

            case OPERATION:
                Q_ASSERT(type != CatItem::OPERATION_ACTION);
                if(type == CatItem::VERB_ACTION){
                    m_parseState = OP_VERB;
                    verbIndex = itemPositionIndex;

                } else if (type ==CatItem::NOUN_ACTION ){
                    m_parseState = OP_NOUN;

                }

            case VERB: Q_ASSERT(type != CatItem::VERB_ACTION);
                if (type ==CatItem::NOUN_ACTION ){
                    m_parseState = VERB_FIRST_ACTION;

                } else if (type ==CatItem::OPERATION_ACTION){
                    opIndex = itemPositionIndex;
                    m_parseState = DONE;
                }

            case NOUN: if(type == CatItem::VERB_ACTION){
                    verbIndex = itemPositionIndex;
                    if(at().isCustomVerb()){
                        m_parseState = VERB_FIRST_ACTION;
                    } else {
                        m_parseState = NOUN_FIRST_ACTION;

                    }
                } if (type ==CatItem::OPERATION_ACTION){
                    opIndex = itemPositionIndex;
                    m_parseState = DONE;
                }

            case OP_VERB: if (type ==CatItem::NOUN_ACTION ){
                    m_parseState = VERB_FIRST_ACTION;
                }
            case OP_NOUN: if (CatItem::VERB_ACTION){
                    verbIndex = itemPositionIndex;
                    if(at().isCustomVerb()){
                        m_parseState = VERB_FIRST_ACTION;
                    } else {
                        m_parseState = DONE;
                    }
                }
            case VERB_FIRST_ACTION:
                //allow but ignore nouns...
                if (type ==CatItem::OPERATION_ACTION){
                    opIndex = itemPositionIndex;
                    m_parseState = DONE;
                }
            case NOUN_FIRST_ACTION: return (type != CatItem::OPERATION_ACTION);
                Q_ASSERT(type ==CatItem::OPERATION_ACTION);
                opIndex = itemPositionIndex;
                m_parseState = DONE;
            case DONE: Q_ASSERT(false);
        }

        InputData d;
        inputData.append(d);
        itemPositionIndex = inputData.count()-1;

    }

    bool transition(CatItem::ActionType type,  ParseState& parseState){

    }

    bool startTransition(CatItem::ActionType type,  ParseState& parseState){
        if(type == CatItem::VERB_ACTION){
            verbIndex = itemPositionIndex;
            m_parseState = VERB;

        } else if (type ==CatItem::NOUN_ACTION ){
            m_parseState = NOUN;

        } else if (type ==CatItem::OPERATION_ACTION){
            opIndex = itemPositionIndex;
            m_parseState = OPERATION;
        }

    }

    bool opTransition(CatItem::ActionType type,  ParseState& parseState){
        Q_ASSERT(type != CatItem::OPERATION_ACTION);
        if(type == CatItem::VERB_ACTION){
            m_parseState = OP_VERB;
            verbIndex = itemPositionIndex;

        } else if (type ==CatItem::NOUN_ACTION ){
            m_parseState = OP_NOUN;

        }

    }

    bool verbTransition(CatItem::ActionType type,  ParseState& parseState){
        if (type ==CatItem::NOUN_ACTION ){
            m_parseState = VERB_FIRST_ACTION;

        } else if (type ==CatItem::OPERATION_ACTION){
            opIndex = itemPositionIndex;
            m_parseState = DONE;
        }

    }

    bool nounTransition(CatItem::ActionType type,  ParseState& parseState){
        if(type == CatItem::VERB_ACTION){
            verbIndex = itemPositionIndex;
            if(at().isCustomVerb()){
                m_parseState = VERB_FIRST_ACTION;
            } else {
                m_parseState = NOUN_FIRST_ACTION;

            }
        } if (type ==CatItem::OPERATION_ACTION){
            opIndex = itemPositionIndex;
            m_parseState = DONE;
        }
    }

    bool OpVerbtransition(CatItem::ActionType type,  ParseState& parseState){
        if (type ==CatItem::NOUN_ACTION ){
            m_parseState = VERB_FIRST_ACTION;
        }
    }

    bool OpNountransition(CatItem::ActionType type,  ParseState& parseState){
        if (CatItem::VERB_ACTION){
            verbIndex = itemPositionIndex;
            if(at().isCustomVerb()){
                m_parseState = VERB_FIRST_ACTION;
            } else {
                m_parseState = DONE;
            }
        }
    }

    bool transition(CatItem::ActionType type,  ParseState& parseState){

    }

            case START:

            case OPERATION:

            case VERB: Q_ASSERT(type != CatItem::VERB_ACTION);

            case NOUN:

            case OP_VERB:
            case OP_NOUN:
            case VERB_FIRST_ACTION:
                //allow but ignore nouns...
                if (type ==CatItem::OPERATION_ACTION){
                    opIndex = itemPositionIndex;
                    m_parseState = DONE;
                }
            case NOUN_FIRST_ACTION: return (type != CatItem::OPERATION_ACTION);
                Q_ASSERT(type ==CatItem::OPERATION_ACTION);
                opIndex = itemPositionIndex;
                m_parseState = DONE;
            case DONE: Q_ASSERT(false);


    
    bool canAddSlot(){
        return (!at().curResult.isEmpty() || !at().organizingArg.isEmpty());
    }

    //
    InputData& at(int i =-1) {
        if(i == -1){
            return InputData[itemPositionIndex];
        } else {
            return InputData[i];
        }
    }


    bool insertItem(CatItem* it);
    bool insertVerbAndImplicitArg(CatItem* it, int argNum=0);


    void clearItem();
    void clearKeys();

    bool takesAnyKey();

    QList<CatItem> customArgs();

    //used in fulltext plugin...
    CatItem getContextType(QString label);
    //used in fulltext plugin...
    CatItem getContextItem(QString label);

    //This was totally untested, so
    QList<ListItem> getDisplayChildren(CatItem parentItem);
    bool currentItemIsEmpty();

    QString getCommandLine();



    void setUserKeys(QString t) { inputData[itemPositionIndex].userKeys = t; }
    //syntax

    CatItem argChosenType(){
        CatItem & it = inputData[itemPositionIndex].organizingArg;
        return it.getTypeParent(CatItem::ACTION_TYPE);
    }

    QList<CatItem> getRemainingCustomArgs();
    CatItem getCustomVerbDefaultArg(){
        Q_ASSERT(false);
//        QList<CatItem> remainingCustomArgs = organizingVerb().getCustomArguments();
//        for(int i=0;i<customVerbIndex;i++){
//            if( inputData[i].curResult.getActionType() == CatItem::OPERATION_ACTION
//                || inputData[i].organizingArg.getActionType() == CatItem::OPERATION_ACTION ){
//                continue;
//            }
//            remainingCustomArgs.removeOne(inputData[i].organizingArg);
//        }
//        return remainingCustomArgs[0];
    }

    int getCount(){
        return inputData.count();
    }

    QString asSerializedString(){
        Q_ASSERT(inputData.count() >0);
        if (inputData.count() ==1){
            return inputData[0].curResult.getPath();
        } else {
            QString pre(COMPOUND_ITEM_PREFIX);
            QString res;
            for(int i=0;i<inputData.count();i++){
                res += QString("{") + inputData[0].curResult.getPath() + "}/";
            }
            res = addPrefix(pre,res);
            return res;
        }

    }

    QString keysOrItem(int i){
        if(!inputData[i].curResult.isEmpty()){
            return inputData[i].curResult.getName();
        } else{
            return inputData[i].userKeys;
        }
    }


    void moveRight() {
        if(itemPositionIndex < inputData.count()-1){
            itemPositionIndex++;
            return true;
        } else {return false;}

    }
    void moveLeft(){
        if(itemPositionIndex > 0){
            itemPositionIndex--;
            return true;
        } else {return false;}
    }

    bool atEnd(){
        Q_ASSERT(itemPositionIndex < inputData.count() );
        Q_ASSERT(inputData.count() >0);
        return((itemPositionIndex == inputData.count()-1));
    }


};

#endif // ITEMARGLIST_H
