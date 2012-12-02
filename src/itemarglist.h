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

    void setSearchLevel(int level);
    int getSearchLevel();
    bool hasVerb();
    void setDefaultValues();
    QString getArgNameAt(int i);
    QString getArgValue(int i, bool fullPath=false);
    QString getArgLabel(int pos=-1);
    QString getArgLabel(CatItem it);
    QString getArgTypePathAt(int i);
    QString getArgTypeAt(int i);
    QString getArgType(CatItem it);
    CatItem getArgItemAt(int i);
    bool argVisibleAt(int i);
    QString currentFieldPlugin(int i);
    bool matchFieldToArg(CatItem arg, InputData& inputD);
    VerbItem& getVerb();
    InputList();

    bool usingCustomVerb();
    bool isOperationChosen();
    bool customVerbChosen();
    void setCustomVerbChosen();
    bool atPossibleVerbSlot();
    int verbPosition();
    bool keepUserFields();
    bool isSelectionEmpty();
    QHash<int, CatItem>  getArgMatchesInOrder(VerbItem& vi);
    QHash<int, CatItem> getArgMatchesWithNewField(int pos, QString n) ;
    CatItem argChosenType();
    QString currentFieldExpression();
    QString currentFieldPlugin();
    bool hasItem();
    bool canAcceptOperation();

    QString getAppState();
    void setAppState(QString s);
    QString previewUrl();
    int getItemCount();
    CatItem getFilterItem();
    CatItem getParentItem();
    bool verbNeedMoreInfo();
    FieldType fieldInputType(int pos=-1);
    bool atFieldChoicePoint();
    bool atItemChoiceField(int pos);
    bool atCustomArgChoicePoinst(int pos);

    //Only beyond any default args
    QList<CatItem> argsToSubstitute();
    QList<CatItem> getRemainingCustomArgs(bool dummyCurrent=true);
    QList<CatItem> getCustomArgs();
    bool shouldExecuteNow();
    QList<CatItem> nounList();
    int slotCount();
    bool atFirstSlot();
    int slotPosition();
    int nounBeginSlot();
    int nounEndSlot();
    int fullfilledFieldsEnd();
    int operationSlot();
    void setLabel(QString l);
    void setArgValue(QString key, QString value);
    void setItem(CatItem item);
    void setItem(QString itemPath);
    void removeItem(CatItem item);
    bool iconJustAdded();
    bool iconJustRemoved();
    void addCustomVerb(CatItem verb);
    void addExtraArguments();
    void addDefaultArguments(bool addArg=false);
    void addExtraArg(QList<CatItem> verbAccu, int& argIndex);
    void refreshOperation(CatItem item);
    bool isCustomVerbChoosableByArgSig();
    CatItem setDefaultVerbIfNeeded();
    CatItem getDefaultActionItem();
    CatItem getItemDefaultVerb();
    QList<CatItem> getNormalAssociation();

    //This is a call-back function, called by the database
    CatItem::ItemType getOrganizeingType();
    //call-back function - decides what items get shown
    bool acceptItem(CatItem* item);
    QList<CatItem::ItemType> getOrganizingTypeList();

    bool verbTakesArgs();
    ListItem previewItem();


    bool canRun(QString* err=0);

    CatItem::ActionType itemTypeAt();

    void rescanSlot();

//Always call this inside an ASSERT
#ifndef QT_NO_DEBUG
    bool checkIntegrity();
#endif
    void setFieldToCustomVerb(int i);
    void addNounToSlot(int i);
    void removeNounFromSlot(int i);
    void clearCustomVerb();
    void clearItem();
    void clearUnMatchItem();
    void clearSlotEntry();
    bool canAddSlot(QString* errorMsg);

    void addSlot();
    InputData& at(int i =-1);
    void clearAll();
    void clearKeys();

    //used in fulltext plugin...
    CatItem getContextType(QString label);
    //used in fulltext plugin...
    CatItem getContextItem(QString label);

    CatItem getParentContextItem();

    bool isOrganizingItem(CatItem item);

    QList<ListItem> getOrganizingFilterItems(int itemsAllowed=4, int charsAvail=100, int minWeight = 0);
    QList<ListItem> getOrganizingSubFilterItems(int itemsAllowed, int charsAvail, int minWeight = 0);
    void setOrganzingItems(QList<ListItem> items);

    //Ignores operations
    QString getCommandLine();
    //Ignores operations
    CatItem getStatementObject();

    QHash<QString, CatItem> getUserInputFields();

    QList<QString> getKeyWords() ;

public:

    QHash<QString, EntryField> asFieldValueHashEx(QHash<int, CatItem> fieldSlotHash,bool fullPath=true);
    QHash<QString, EntryField> asFieldValueHash(bool includeTag=false, bool fullPath=true);
    CatItem asStateItem(bool includeTag = true);
    void loadHistoryEvent(HistoryEvent he, QHash<QString, CatItem> theRealItems);
    QString asSerializedString(bool includeTag);
    CatItem hotkeyActivated(QChar k);
    void popSlot();
    void popLastItem();
    CatItem currentItem();
    CatItem& currentItemRef() ;
    int iconItemCount();
    CatItem iconItem(int i);
    CatItem itemAtSlot(int i);
    bool atCustomField();
    bool moveRight();
    bool moveLeft();
    bool atEnd();
    void setFilterItem(ListItem filterItem);
    void setSubFilterItem(ListItem filterItem);
    int getListItemCount();

    //List items...
    void addSingleListItem(CatItem it, int pos);
    void addListItemList(QList<CatItem> il ) ;

    void appendListItemList(QList<CatItem> il);

    void filterItems();

    bool isExpanded();
    void setExpanded(bool e=true);
    QString getExpandedSearchStr();
    void setExpandedSearchStr(QString  str);
    void pushChildren(CatItem& parItem) ;
    int popParent();
    QList<ListItem> getFormattedListItems(int& selected, int itemsHigh, int charsAvail);
    void formatActionList(QList<CatItem>& listItems);
    QList<CatItem> getListItems();
    QList<CatItem> getStubbedItems();
    ListItem& getListItem(int i );
    int getCurrentItemIndex();
    ListItem getItemByPath(QString path);
    ListItem& getItemByPathRef(QString path);
    ListItem findItemInParent(QString path);
    void popToParentPosition(QString path);
    bool moveCurSelectionUp();
    bool moveCurSelectionDown();
    bool isExploringChildren();

    bool showFormattedItem();
    int userInSlot();
    QString getUserKeys(int pos=-1);
    void setUserKeys(QString t);
    void appendUserkeys(QString s);
    void backspaceKey(int n=1);
    void modifyUserText(QString newKeys, int cursorLoc);

    bool selectingByKeys();
    QString getPathText(int spaceAvailable);
    bool currentItemMatchesAllStrings();
    QString mainItemText(bool skipOp = false);
    QString basicItemText();
    QString simpleItemText();
    QString asFormattedString(CatItem substituteItem= CatItem(),
                              int* beginFieldPos=0, int* endFieldPos=0, bool usePath=true);
    QString addOperationStr(QString& stmt, CatItem substituteItem=CatItem());
    QString asFormattedCustomExpr(CatItem substituteItem= CatItem(),
                                  int* beginFieldPos=0, int* endFieldPos=0, bool usePath=true);
    QString asFormattedStandardExpr(CatItem substituteItem= CatItem());
    QString formatItemByPos(int j);
    QString getExplanationText(int* beginFieldPos=0, int* endFieldPos=0);

    //This is for verb matching the various args in the list
    // It would get pathological if the list had more than 3-4 members
    // But can't have more than three...
    QList< QList < CatItem > > getCustomVerbSignature();

    // turn {{x,y},{w,z},{a,b}} into {{x,w,a},{x,w,b}...}
    // Lots of recursion here, of course...
    QList< QList<CatItem> > getPermuteList(QList< QList<CatItem> > toPermute);

    bool isLabeledFromStart();
    bool isLabeledFromEnd();
    QList < CatItem >  getPossibleArgTypes(int i);
    QList < CatItem >  getPossibleArgTypes(InputData inputDatum);
    CatItem getOperationPart();
    CatItem getOperationTarget();
    QString opArgKeys();
    CatItem getActionPart();
    CatItem& lastItem();
    CatItem userItem();

};

#endif // ITEMARGLIST_H
