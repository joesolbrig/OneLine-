#ifndef FORMDISPLAYER_H
#define FORMDISPLAYER_H

#include <QString>
#include <QPainter>

#include "itemarglist.h"
#include "listwithdisplay.h"


struct FieldRectangle {
    int topPos;
    int baseHeight;
    int totalRemainSpace;

    int baseLabelIndent;
    int baseFieldIndent;

    QFont labelFont;
    QString labelText;
    QFont fieldFont;
    QString fieldValue;

    QPainter *painter;

};

class ListWithFrame;

class FormDisplayer
{
    InputList* m_inputList;
    ListWithFrame* m_choiceList;
    QFont m_font;
    int m_width;
public:
    FormDisplayer(InputList* inputList,int width);

    void formatFieldRectangle(FieldRectangle& fr,
                              QPainter* painter,
                              int heightAvailable=-1, int topPos=0);
    void setChoiceList(ListWithFrame* choiceList){
        m_choiceList = choiceList;
    }

    QSize sizeOrPaint(QPainter *painter=0, int heightAvailable=-1, int topPos=0);

    void sizeOrPaintRect(FieldRectangle& fr);
    void sizeOrPaintList(FieldRectangle& fr, bool show);

};

#endif // FORMDISPLAYER_H
