#include "listwithdisplay.h"



ListWithFrame::ListWithFrame(QWidget* parent) : QWidget(parent){
    this->m_layout = new ListWithDisplayHolder(this);
}
QListWidgetItem * ListWithFrame::currentItem() const{
    return m_layout->currentItem();

}
void ListWithFrame::addItem(ListItem itm){
    m_layout->addItem(itm);
}

void ListWithFrame::addItemList(QList<ListItem> il){
    m_layout->addItemList(il);
}

int ListWithFrame::sizeHintForRow(int row){
    return m_layout->sizeHintForRow(row);
}

void ListWithFrame::addTopicMessage(QString msg){
    m_layout->addTopicMessage(msg);

}

void ListWithFrame::addNavigationMessage(QString msg){
    m_layout->addNavigationMessage(msg);

}
