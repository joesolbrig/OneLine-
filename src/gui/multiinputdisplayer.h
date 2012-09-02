#ifndef MULTIINPUTDISPLAYER_H
#define MULTIINPUTDISPLAYER_H

#include <QPainter>

#include "itemarglist.h"
#include "listwithdisplay.h"

class ListWithFrame;

class MultiInputDisplayer {

public:
    MultiInputDisplayer(InputList* inputList, int width);
    void setChoiceList(ListWithFrame* list){ m_choiceList= list;}
    QSize sizeOrPaint(QPainter *painter=0, int heightAvailable=-1, int topPos=0);
    int sizeOrPaintLabel(QPainter *painter, int index, int topPos);
    int sizeOrPaintField(QPainter *painter, int index, int topPos);
    int sizeOrPaintField(QPainter *painter,QFont font, QString string, int topPos);
    int sizeOrPaintJustField(QPainter *painter,int index, int topPos, bool spaceForList=true);
    int sizeOrPaintExtraFields(QPainter *painter, CatItem item, int topPos);
    int sizeOrPaintCurrentField(QPainter *, int topPos, int heightAvailable, bool spaceForList=true);
    int sizeOrPaintVerb(QPainter *painter, int topPos);
    int sizeOrPaintList(QPainter *, int topPos, int heightAvailable=-1, bool showLabel=true);

protected:
    int displayItems();
    void createSizeByLength();
    int adjustFontSize4Str(QString t, QSize size, QFont inFont);
    void drawSelectionRectangle(QPainter *painter, int topPos, int bottomPos);

private:
    InputList* m_inputList;
    ListWithFrame* m_choiceList;
    QFont m_font;
    int m_width;




};

#endif // MULTIINPUTDISPLAYER_H
