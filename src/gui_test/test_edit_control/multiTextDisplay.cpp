
#include "multiTextDisplay.h"

void MultiTextDisplay::adjustForTextChange(){

    int rectangleHeight =MAX(m_currentEdit.height(), m_backgroundLabel.baseHeight());
    if(!m_editBelow.toPlainText().isEmpty()){
        rectangleHeight = MAX(rectangleHeight,
            getTopMargin() +  m_currentEdit.height() + m_editBelow.height());
    }
    if(rectangleHeight > m_backgroundLabel.height()){
        m_backgroundLabel.setCurrentHeight(rectangleHeight);
    }

    int totalHeight = rectangleHeight + getIconSize()/2;
    int width = m_backgroundLabel.geometry().width();
    resize(width, totalHeight);

    int iconHorizontalSpace = MAX(getIconSize(), getLeftMargin());
    int iconLeft = iconHorizontalSpace/2 - getIconSize()/2;
    m_mainIconLabel.move(iconLeft,0);

    m_backgroundLabel.move(0, getIconSize()/2);

    QRect r(getLeftMargin(),getIconSize()/2 + getTopMargin(),
                                        width - iconHorizontalSpace,iconHorizontalSpace);

    m_currentEdit.setGeometry(r);

    int rectTopLower = rectangleHeight - getTopMargin() +  m_currentEdit.height();
    m_editBelow.move(getLeftMargin(), rectTopLower);
    m_explanationText.move(getLeftMargin(), getTopMargin()/2 - m_explanationText.height());
    update();

}

//void MultiTextDisplay::BeginAnimateToBelow(){
//    SizeForTexts();
//}
//
//void MultiTextDisplay::EndAnimateToBelow(){
//}
