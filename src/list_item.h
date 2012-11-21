#ifndef LIST_ITEM_H
#define LIST_ITEM_H

#include <QFont>
#include <QListWidget>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QTextDocument>
#include <QPainter>
#include <QTextLayout>
#include <QAbstractTextDocumentLayout>
#include <QApplication>

#include <QTextStream>
#include "item.h"

QString summarizeMessage(CatItem it);

int drawFieldAndValue(QPainter *painter, QFont font,
                       QString htmlField, QString htmlFieldValue, QRect rect);

int drawJustField(QPainter *painter, QFont font,
                      QString htmlField, QRect rect);

int drawFieldLabel(QPainter *painter, QFont font,
                       QString htmlFieldLabel, QRect rect, bool bottomAligned=false);

int fontSize4Width(QString t, QSize size, QFont inFont);

void drawHtmlLine(QPainter *painter, const QFont font, QRect rect, QString text,
                  bool multiline=false,
                  bool leftAligned=true);
void drawHtmlRightAligned(QPainter *painter, const QFont font, QRect rect, QString text);
qreal pixelOffsetForHtmlChar( QFont font, QString text, int index=-1);

int htmlLineCount( QFont font, QString text, int width);

//An actual visible Item
//some of its effects are manipulated by main window,
//So its members are public for now...
class ListItem :public CatItem {


public:

//    QString rawDisplayPath;
//    QString formattedDisplayPath;

    ListItem(): CatItem(){
        m_minSplitPointIndex = -9;
        m_maxSplitPointIndex = -9;
        m_nextKeyIndex = -1;
        m_charsAvail =0;
    }

    ListItem(QString s): CatItem(s){
        m_minSplitPointIndex = -9;
        m_maxSplitPointIndex = -9;
        m_nextKeyIndex = -1;
        m_charsAvail =0;
    }

    void createComponentList();

    void setCharsAvailable(int i){
        m_charsAvail = i;

    }

    int getCharsAvailable(){
        return m_charsAvail;
    }

    explicit ListItem(CatItem it);

    ListItem(const ListItem& c): CatItem(c){
        m_componentList = c.m_componentList;
        m_useDisplayPathForNextKey = c.m_useDisplayPathForNextKey;
        m_minSplitPointIndex = c.m_minSplitPointIndex;
        m_maxSplitPointIndex = c.m_maxSplitPointIndex;
        m_nextKeyIndex = c.m_nextKeyIndex;
        m_charsAvail = c.m_charsAvail;

        m_sectFontStr = c.m_sectFontStr;
        m_keyFontStr = c.m_keyFontStr;

        m_displayLengths = c.m_displayLengths;
        containingItem = c.containingItem;
        actualIcon = c.actualIcon;

        m_tempCoveringWeight = c.m_tempCoveringWeight;
    }

    ListItem(ListItem& c): CatItem(c){
        m_componentList = c.m_componentList;
        m_useDisplayPathForNextKey = c.m_useDisplayPathForNextKey;
        m_minSplitPointIndex = c.m_minSplitPointIndex;
        m_maxSplitPointIndex = c.m_maxSplitPointIndex;
        m_nextKeyIndex = c.m_nextKeyIndex;
        m_charsAvail = c.m_charsAvail;

        m_sectFontStr = c.m_sectFontStr;
        m_keyFontStr = c.m_keyFontStr;

        m_displayLengths = c.m_displayLengths;
        containingItem = c.containingItem;
        actualIcon = c.actualIcon;

        m_tempCoveringWeight = c.m_tempCoveringWeight;
    }

    ListItem& operator=(const ListItem& c){
        assign_from(c);
        m_componentList = c.m_componentList;
        m_useDisplayPathForNextKey = c.m_useDisplayPathForNextKey;
        m_minSplitPointIndex = c.m_minSplitPointIndex;
        m_maxSplitPointIndex = c.m_maxSplitPointIndex;
        m_nextKeyIndex = c.m_nextKeyIndex;
        m_charsAvail = c.m_charsAvail;

        m_sectFontStr = c.m_sectFontStr;
        m_keyFontStr = c.m_keyFontStr;

        m_displayLengths = c.m_displayLengths;
        containingItem = c.containingItem;
        actualIcon = c.actualIcon;

        m_tempCoveringWeight = c.m_tempCoveringWeight;
        return *this;
    }

    static QList<ListItem> convertList(QList<CatItem> cList);

    QString modifiedDescription();

    QIcon getActualIcon(){
        if(!actualIcon.isNull()){
            return actualIcon;
        }
        if(hasCustomVariant(REAL_ICON_KEY)){
            QVariant v = this->getCustomPluginVariant(REAL_ICON_KEY);
            if (v.canConvert<QIcon>()){
                QIcon ic = v.value<QIcon>();
                actualIcon = ic;
                return ic;
            }
        }

        return QIcon();
    }

    void setActualIcon(QIcon pm){
        actualIcon = pm;
    }

    bool hasIconSet(){
        return !actualIcon.isNull();
    }


    static QColor colorFromType(CatItem::ItemType t){
        QColor c("#FFFFFF");
        switch(t){
            case CatItem::TAG:
            case CatItem::LOCAL_DATA_FOLDER:
            case CatItem::ACTION_TYPE:
            case CatItem::ORGANIZING_TYPE:
            case CatItem::PLUGIN_TYPE: c = QColor("#A0FFFF");
                break;
            case CatItem::VERB:
            case CatItem::OPERATION: c = QColor("#FBFBFF");
                break;
            case CatItem::PUBLIC_FEED:
            case CatItem::PUBLIC_DOCUMENT: c = QColor("#FFFAFC");
                break;
            case CatItem::PERSON: c = QColor("#FBFFFA");
                break;
            case CatItem::ALERT: c = QColor("#00FFFF");
                return c;
                break;
            case CatItem::MESSAGE: c = QColor("#FFEFFC");
                break;
            case CatItem::LOCAL:
            case CatItem::LOCAL_DATA_DOCUMENT:
            default:
                return c;
        }
        return c;
    }

//    void setCharsAvailable(int c){
//        Q_ASSERT(c >0);
//        m_charsAvail = c;
//    }
    int getMaxSplit(){ return m_maxSplitPointIndex; }

    int getMinSplit(){ return m_minSplitPointIndex; }

    bool hasKeyChar(){
        return m_nextKeyIndex >=0;

    }

    QChar hotkeyChar(){
        if(m_nextKeyIndex<0){
            return QChar('*');
        }


        if(m_useDisplayPathForNextKey){
            return this->getRawDisplayPath()[m_nextKeyIndex];
        } else {
            return this->getName()[m_nextKeyIndex];
        }
    }

    void setNextKeyIndex(int i ){
        m_nextKeyIndex = i;
    }

    void setUseDisplayPathForNextKey(bool u=true){
        m_useDisplayPathForNextKey =u;
    }

    int dirDiff(ListItem a){
        if(a.m_componentList.isEmpty()){
            a.createComponentList();
        }
        if(m_componentList.isEmpty()){
            createComponentList();
        }


        if(a.isEmpty()){ return -1; }
        int i;
        for(i=0;  (i<a.m_componentList.count() && (i< m_componentList.count())) && a.m_componentList[i]== m_componentList[i];i++){ }
        return i;
    }
    void setSplitIndex(int index){
        if(m_componentList.isEmpty()){
            createComponentList();
        }
        Q_ASSERT(index > -2);
        Q_ASSERT(index <= m_componentList.count());

        m_minSplitPointIndex = MIN(m_minSplitPointIndex, index);
        m_maxSplitPointIndex = MAX(m_maxSplitPointIndex, index);
    }

    void setBaseSplitI(){
        if(m_componentList.isEmpty()){
            createComponentList();
        }

        m_minSplitPointIndex = m_componentList.count()-1;
        m_maxSplitPointIndex = 0;
    }

    QString formattedName(bool useHtml=true, int charsAllowed=-1);
    QString formattedSingleLine();

    void setFormattedPathOveride(QString alterDescription){
        m_componentList.clear();
        QString desc = alterDescription.simplified();
        setDescription(desc);
        setAltDescription(desc);
        setIsTempItem(true);
        setUseDescription(true);
    }

    void setFormattedNameOveride(QString alterName){
        this->setCustomPluginInfo(OVERRIDE_FORMATTED_NAME_KEY,alterName);

    }

    QString getFormattedPath(int charsAllowed,  bool keepName=false, bool userAlt=false);
    QString getRawDisplayPath(int charsAllowed=-1);

    QFont getDisplayFont(){
        QString family = this->getCustomString(FONT_FAMILY_KEY, NAME_FONT_FAMILY_DEFAULT);
        if(family.isEmpty()){
            return qApp->font();
        }
        int size = this->getCustomValue(FONT_SIZE_KEY, FONT_SIZE_DEFAULT);
        int weight = this->getCustomValue(FONT_WEIGHT_KEY, FONT_WEIGHT_DEFAULT);
        bool italics =this->getCustomValue(FONT_ITALIC_KEY, FONT_ITALIC_DEFAULT);
        return QFont(family, size, weight, italics);

    }

private:
    QStringList m_componentList;

    //QStringList m_Seperators;
    bool m_useDisplayPathForNextKey;
    int m_minSplitPointIndex;
    int m_maxSplitPointIndex;
    int m_nextKeyIndex;
    int m_charsAvail;

    int m_nameStartIndex;

    QString m_sectFontStr;
    QString m_keyFontStr;
    //int m_charsAvail;

    CatItem containingItem;//normally empty
    QIcon actualIcon; //empty but to carry

    qint32 m_tempCoveringWeight;

    //For processing
    QList<int> m_displayLengths;
    QStringList m_processedPieces;
private:
    bool endSep(QString workingPath, int i);
    QList<QString> splitPath(QString path);

    //support classes and functions...
    QList<int> decomposeByStrings(int charsAvail, QList<QString> sl);
    QStringList highlightKey(QStringList l, int kNum);
    QString sect(int s, int e);
    QStringList getPathPieces();
    QStringList truncByLength(QStringList sl, QList<int> n);
    QStringList positionOrder(QStringList l);
    void reorderBounds();
    QStringList stripNulls(QStringList o);
    QString stripCombine(QStringList o, bool keepName);

};







#endif // LIST_ITEM_H
