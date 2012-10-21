#include "formdisplayer.h"

FormDisplayer::FormDisplayer(InputList* inputList,int width)
{
    m_inputList = inputList;
    m_width = width;
    ListItem il(m_inputList->getVerb());
    m_font = il.getDisplayFont();
}

void FormDisplayer::formatFieldRectangle(FieldRectangle& fr,
                     QPainter* painter,
                     int heightAvailable, int topPos){

    fr.labelFont = m_font;
    fr.fieldFont = m_font;
    fr.fieldFont.setBold(true);
    fr.fieldFont.setUnderline(true);

    fr.topPos=topPos;
    QFontMetrics fm(fr.labelFont);
    fr.baseHeight = fm.height()*3;
    fr.totalRemainSpace=heightAvailable;

    fr.baseLabelIndent=0;
    fr.baseFieldIndent=CUSTOM_LABEL_LEFTMARGIN;

    fr.painter = painter;
}


QSize FormDisplayer::sizeOrPaint(QPainter *painter, int heightAvailable, int topPos){
    qDebug() << "painter = " << (int)painter;
    FieldRectangle fr;
    formatFieldRectangle(fr, painter, heightAvailable, topPos);
    if(m_inputList->operationSlot()!=-1) {
        fr.labelText = m_inputList->getOperationPart().getName();
        fr.fieldValue = m_inputList->asFormattedString();
        sizeOrPaintRect(fr);

        fr.labelText = m_inputList->getOperationPart().getOperationHelperPhrase();
        fr.fieldValue = "";
        sizeOrPaintRect(fr);
        sizeOrPaintList(fr, m_choiceList->count()>0);
    } else {
        bool showList = m_choiceList->count()>0
                        && !CatItem::atUserDataField(m_inputList->getArgTypeAt(
                            m_inputList->slotPosition()-1));

        Q_ASSERT(!m_inputList->getVerb().isEmpty());
        fr.labelText = "";
        fr.fieldValue = m_inputList->getVerb().getName();
        fr.baseFieldIndent=CUSTOM_VERB_RIGHTMARGIN;
        sizeOrPaintRect(fr);
        fr.baseFieldIndent=CUSTOM_LABEL_LEFTMARGIN;

        for(int index=m_inputList->nounBeginSlot();
                (index>=0) && index<= m_inputList->fullfilledFieldsEnd();index++){
            if(!m_inputList->argVisibleAt(index)){ continue; }
            fr.labelText = m_inputList->getArgNameAt(index);
            fr.fieldValue = m_inputList->getArgValue(index);
            sizeOrPaintRect(fr);
            if(index==m_inputList->slotPosition())
                { sizeOrPaintList(fr,showList);}
        }
    }
    return QSize(m_width, fr.topPos + CUSTOM_FORM_BOTTOMMARGIN);
}

void FormDisplayer::sizeOrPaintRect(FieldRectangle& fr){
    if(fr.painter){
        QRect labelRect(fr.baseLabelIndent,fr.topPos,
                        fr.baseFieldIndent -fr.baseLabelIndent,fr.baseHeight);
        drawHtmlLine(fr.painter, fr.labelFont, labelRect, fr.labelText);
        QRect fieldRect(fr.baseFieldIndent,fr.topPos,
                        m_width - fr.baseFieldIndent,fr.baseHeight);
        drawHtmlLine(fr.painter, fr.fieldFont, fieldRect, fr.labelText);
    }
    fr.topPos +=fr.baseHeight;
    fr.totalRemainSpace -= fr.baseHeight;
}

void FormDisplayer::sizeOrPaintList(FieldRectangle& fr, bool show){
    if(!fr.painter){
        if(show){
            QRect listRect(fr.baseLabelIndent,fr.topPos,
                            m_width - fr.baseFieldIndent,fr.totalRemainSpace);
            m_choiceList->setVisible(true);
            m_choiceList->setGeometry(listRect);
        } else {
            m_choiceList->setVisible(false);
        }
    }
}

