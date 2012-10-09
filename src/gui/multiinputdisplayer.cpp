#include "item.h"

#include "multiinputdisplayer.h"


MultiInputDisplayer::MultiInputDisplayer(InputList* inputList,int width){
    m_inputList = inputList;
    m_width = width;
    ListItem il(m_inputList->getVerb());
    m_font = il.getDisplayFont();

}

//"Polymorphic" code - sizes And paints, depending if painter is null or not
QSize MultiInputDisplayer::sizeOrPaint(QPainter *painter, int heightAvailable, int topPos){
    qDebug() << "painter = " << (int)painter;
    topPos += CUSTOM_FORM_TOPMARGIN;
    if(m_inputList->operationSlot()!=-1) {
        //topPos += sizeOrPaintField(painter, m_inputList->operationSlot(), topPos);
        QFont f = m_font;
        f.setPointSize(f.pointSize()+3);
        QString opName = m_inputList->getOperationPart().getName();
//        topPos += sizeOrPaintField(painter, f,  m_inputList->getOperationPart().getName(),
//                   topPos);
        topPos += sizeOrPaintField(painter, f,
                    "TESTING TESTING", topPos);
        topPos += sizeOrPaintField(painter, f,
                                   opName + " " + m_inputList->asFormattedString(), topPos);
        topPos += sizeOrPaintField(painter, f,
                    m_inputList->getOperationPart().getOperationHelperPhrase(),
                   topPos);
        if(!m_choiceList->count()){
            int avail =heightAvailable - (topPos+CUSTOM_FORM_BOTTOMMARGIN);
            topPos += sizeOrPaintList(painter, topPos, avail, false);
            m_choiceList->setVisible(true);
        } else {
            m_choiceList->setVisible(false);
        }
    } else {
        topPos += sizeOrPaintVerb(painter,topPos);
        qDebug() << "topPos after Verb";
        int listTop = topPos;
        for(int i=m_inputList->nounBeginSlot();
                (i>=0) && i<= m_inputList->fullfilledFieldsEnd();i++){
            if(!m_inputList->argVisibleAt(i)){ continue; }

            int avail =heightAvailable - (topPos+CUSTOM_FORM_BOTTOMMARGIN);
            if(i!=m_inputList->slotPosition()){
                topPos += sizeOrPaintField(painter, i, topPos);
                qDebug() << "topPos after field";
            } else {
                if(painter){
                    Q_ASSERT(heightAvailable<0);
                    topPos += sizeOrPaintCurrentField(painter, topPos,avail,
                        !CatItem::atUserDataField(m_inputList->getArgTypeAt(i)));
                } else { listTop = topPos; }
            }
        }
        //out of order call so available space added correctly
        if(!painter){
            topPos += sizeOrPaintCurrentField(
                    painter, listTop, heightAvailable,
                    !CatItem::atUserDataField(m_inputList->getArgTypeAt(
                                                                      m_inputList->slotPosition()-1)) &&
                    m_choiceList->getRowCount()>0);
        }
    }
    topPos+=CUSTOM_FORM_BOTTOMMARGIN;
    QSize sz(m_width, topPos);
    Q_ASSERT(sz.height()>1);
    Q_ASSERT(sz.width()>1);
    return sz;
}

int MultiInputDisplayer::sizeOrPaintVerb(QPainter *painter,int topPos){
    VerbItem vi = m_inputList->getVerb();
    Q_ASSERT(!vi.isEmpty());
    QString name = vi.getName();

    ListItem li(vi);
    QFont font = li.getDisplayFont();
    QRect r;
    r.setTopLeft(QPoint(CUSTOM_VERB_RIGHTMARGIN,topPos));
    r.setRight(m_width);
    r.setHeight(CUSTOM_FIELD_STANDARDHEIGHT);

    if(painter){ drawFieldLabel(painter, font, name, r); }

    return r.height();
}

int MultiInputDisplayer::sizeOrPaintCurrentField(QPainter *painter, int topPos,
                            int heightAvailable, bool spaceForList){

    //careful with the weird recursion...
//    if(painter){
//        int bottomPos=sizeOrPaintCurrentField(0,topPos, heightAvailable);
//        drawSelectionRectangle(painter,topPos, bottomPos);
//    }

    if(m_inputList->atFieldChoicePoint() || ((int)m_inputList->fieldInputType()<(int)InputList::USER_TEXT)){
        m_choiceList->setVisible(true);
        return sizeOrPaintList(painter, topPos, heightAvailable);
    } else {
        int i=m_inputList->slotPosition();
        if(!painter){
            this->m_choiceList->setVisible(false);
        }
        if(spaceForList){
            //avail =heightAvailable - (topPos+CUSTOM_FORM_BOTTOMMARGIN);
            topPos = sizeOrPaintList(painter, topPos, heightAvailable);
            m_choiceList->setVisible(true);
        } else {
            m_choiceList->setVisible(false);
        }
        //return sizeOrPaintField(painter, i, topPos);
        return sizeOrPaintJustField(painter, i, topPos, spaceForList);
    }
}

int MultiInputDisplayer::sizeOrPaintExtraFields(QPainter *painter, CatItem item, int topPos){

    ListItem li(item);
    QFont font = li.getDisplayFont();
    QRect r;

    int newPos=topPos;

//    QString fieldPreLabel = m_inputList->getArgLabel(item);
//    if(!fieldPreLabel.isEmpty()){
//        r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,newPos));
//        r.setRight(m_width);
//        r.setHeight(CUSTOM_LABEL_HEIGHT);
//        if(painter){
//            drawFieldLabel(painter, font, fieldPreLabel, r);
//        }
//        newPos+= r.height();
//    }
    r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,newPos));
    r.setRight(m_width);
    r.setHeight(CUSTOM_FIELD_STANDARDHEIGHT + CUSTOM_FIELD_BOTTOMPADDING);

    QString fieldName = item.getName();
    QString fieldValue = item.getArgValue();
    if(fieldValue.isEmpty()){
        fieldValue = DOTS_CHARS;
    }
    if(painter){
        drawFieldAndValue(painter, font, fieldName, fieldValue,r);
    }
    newPos+= r.height();
    return newPos - topPos;
}


int MultiInputDisplayer::sizeOrPaintField(QPainter *painter,int index, int topPos){

    ListItem li(m_inputList->getArgItemAt(index));
    QFont font = li.getDisplayFont();
    QRect r;

    int newPos=topPos;
//    newPos += sizeOrPaintLabel(painter,index,topPos);

    r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,newPos));
    r.setRight(m_width);
    r.setHeight(CUSTOM_FIELD_STANDARDHEIGHT);
    QString fieldName = m_inputList->getArgNameAt(index);
    QString fieldValue = m_inputList->getArgValue(index);
    //QString fieldValue = m_inputList->getUserKeys();
    newPos += drawFieldAndValue(painter, font, fieldName, fieldValue,r);
    newPos+=CUSTOM_FIELD_BOTTOMPADDING;
    return newPos - topPos;
}

int MultiInputDisplayer::sizeOrPaintField(QPainter *painter, QFont font, QString string, int topPos){

    QRect r;

    int newPos=topPos;

    r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,newPos));
    r.setRight(m_width);
    r.setHeight(CUSTOM_FIELD_STANDARDHEIGHT);
    newPos += drawJustField(painter, font, string,r);
    newPos+=CUSTOM_FIELD_BOTTOMPADDING;
    return newPos - topPos;
}

int MultiInputDisplayer::sizeOrPaintJustField(QPainter *painter,int index,
            int topPos, bool spaceForList){

    ListItem li(m_inputList->getArgItemAt(index));
    QFont font = li.getDisplayFont();
    font.setPointSize(font.pointSize()+2);
    QFontMetrics fm(font);
    QRect r;

    int newPos=topPos;
    r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,newPos));
    r.setRight(m_width);
    if(spaceForList){
        r.setHeight(CUSTOM_FIELD_STANDARDHEIGHT + CUSTOM_FIELD_BOTTOMPADDING);
    } else {
        r.setHeight(fm.height());
    }
    QString fieldName = m_inputList->getArgNameAt(index);
    newPos += drawJustField(painter, font, fieldName, r);
    return newPos - topPos;
}

int MultiInputDisplayer::sizeOrPaintLabel(QPainter *painter,int index, int topPos){

    ListItem li(m_inputList->getArgItemAt(index));
    QFont font = li.getDisplayFont();
    QFontMetrics fm(font);
    QRect r;

    int newPos=topPos;

    QString fieldPreLabel = m_inputList->getArgLabel(index);
    if(!fieldPreLabel.isEmpty()){
        r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,topPos));
        r.setRight(m_width);
        r.setHeight(fm.height());
        newPos+= drawFieldLabel(painter, font, fieldPreLabel, r);
    }
    return newPos - topPos;
}

int MultiInputDisplayer::sizeOrPaintList(QPainter * painter, int topPos, int heightAvailable, bool showLabel){

    if(showLabel && !m_inputList->atFieldChoicePoint()){
        topPos += sizeOrPaintLabel(painter,m_inputList->slotCount()-1,topPos);
    }

    int itemCount = m_inputList->getListItems().count();
    int heightBySize = itemCount *UI_DEFAULT_ITEM_HEIGHT;
    heightBySize = MIN(heightBySize,CUSTOM_FIELD_CHOICEHIEGHT);
    if(heightAvailable>0){
        heightBySize = MIN(heightBySize,heightAvailable);
    }
    QRect r;
    r.setTopLeft(QPoint(CUSTOM_LABEL_RIGHTMARGIN,topPos));
    r.setRight(m_width);
    r.setHeight(heightBySize);
    if(!painter){
        m_choiceList->setVisible(true);
        m_choiceList->setGeometry(r);
    }
    return m_choiceList->geometry().height();
}



int MultiInputDisplayer::adjustFontSize4Str(QString t, QSize size, QFont inFont){
    return fontSize4Width(t, size, inFont);
}

void MultiInputDisplayer::drawSelectionRectangle(QPainter *painter, int topPos, int bottomPos){

    QRect r;

    r.setTopLeft(QPoint(CUSTOM_LABEL_SELECT_LEFTMARGIN,topPos));
    r.setBottomRight(QPoint(m_width - CUSTOM_LABEL_SELECT_RIGHTMARGIN,bottomPos));

    QBrush br;
    painter->save();
    br.setColor(CUSTOM_FIELD_SELECT_COLOR);
    painter->setBrush(br);
    painter->drawRect(r);
    painter->restore();

}
