

#include "icon_delegate.h"
#include "edit_controls.h"
#include "main.h"
#include "globals.h"
#include "options.h"
#include "plugin_interface.h"
#include "appearance_window.h"
#include "cat_builder.h"
#include "list_item.h"
#include "multiTextDisplay.h"
#include "listwithdisplay.h"
#include "listwithframe.h"
#include "fancycontextmenu.h"

//Qt::FramelessWindowHint | Qt::Tool

FancyContextMenu::FancyContextMenu(CatItem priorityItem, QList<CatItem> actionChildren):
        QWidget(0,
                Qt::FramelessWindowHint), m_layout(QBoxLayout::TopToBottom,this), m_sublayout(QBoxLayout::LeftToRight), m_slider(this)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_LayoutOnEntireRect,true);

    //slider
    m_slider.setTickPosition(QSlider::TicksLeft);
    m_slider.setMinimum(0);
    m_slider.setMaximum((WEIGHT_TICS-1));

    m_priorityParent = priorityItem.getSearchSourceParent();
    int w = m_priorityParent.getSourceWeightTics();
    qDebug() << "FancyContextMenu setting scaledWeight:" << w;
    m_slider.setValue(w);
    connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

    QFont f = ListItem(m_priorityParent).getDisplayFont();
    m_topLabel.setFont(f);
    m_topLabel.setText(
            tagAs(m_priorityParent.getName(),"b")
            + QString(" priority ") );
    m_topLabel.setMargin(0);
    m_topLabel.setIndent(0);
    m_topLabel.adjustSize();
    QRect r = m_topLabel.geometry();
    QFontMetrics fm(f);
    r.setHeight(fm.height());
    m_topLabel.setGeometry(r);

    m_layout.addWidget(&m_topLabel, 0, Qt::AlignTop);
    m_sublayout.addWidget(&m_slider,0, Qt::AlignLeft);

    //menu
    for(int i=0; i<actionChildren.count();i++){
        QString name = actionChildren[i].getName();
        if(m_optionItems.contains(name)){
            qDebug() << "warning: " << name << "from" << actionChildren[i].getPath();
            //Q_ASSERT(false);
            continue;
        }
        m_optionItems[name]= actionChildren[i];
        QAction* extendMenu = m_menu.addAction( name,this,SLOT(optionChosen()));
        connect(extendMenu, SIGNAL(triggered()), gMainWidget, SLOT(extendCatalog()));
    }

    connect(this, SIGNAL(operateOnItem(QString, const CatItem )),
            gMainWidget, SLOT(operateOnItem(QString, const CatItem )));
    QRect g = geometry();
    g.setHeight(MAX(m_menu.height(), m_slider.height()) + m_topLabel.height());
    g.setWidth(MAX(m_slider.geometry().width() + m_menu.geometry().width(),
                   m_topLabel.width()));
    setGeometry(g);
    m_sublayout.addWidget(&m_menu,0, Qt::AlignRight);

    m_priorityItem = priorityItem;
    m_sublayout.setMargin(0);
    m_sublayout.setContentsMargins(0,0,0,0);
    //m_sublayout.update();
    m_layout.addLayout(&m_sublayout,1);
    setLayout(&m_layout);
    m_layout.setMargin(0);
    m_layout.setContentsMargins(0,0,0,0);
    //m_layout.update();
    adjustSize();
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setFocus();
}

void FancyContextMenu::focusOutEvent ( QFocusEvent * evt) {
    QWidget::focusOutEvent(evt);
}

void FancyContextMenu::mousePressEvent(QMouseEvent * event){
    QWidget::mousePressEvent(event);
}

FancyContextMenu::~FancyContextMenu(){
    gMainWidget->m_contextMenu=0;
    destroy();
}


void FancyContextMenu::sliderChanged(int value){
    CatItem setPrioritItem(addPrefix(OPERATION_PREFIX,SET_PRIORIT_OPERATION));
    if(m_priorityItem.getSourceWeightTics() !=value){
        setPrioritItem.setCustomValue(SET_PRIORITY_KEY_STR,value);
        emit operateOnItem(m_priorityItem.getPath(),setPrioritItem);
    }
}

void FancyContextMenu::optionChosen(){
    QAction* a = (QAction*)this->sender();
    QString name = a->text();
    Q_ASSERT(m_optionItems.contains(name));
    CatItem optionItem = m_optionItems[name];
    Q_ASSERT(!optionItem.isEmpty());
    CatItem activateOptionItem(addPrefix(OPERATION_PREFIX,ACTIVATE_OPTION_ITEM));
    activateOptionItem.addChild(optionItem);
    emit operateOnItem(m_priorityItem.getPath(),activateOptionItem);
    //gMainWidget->operateOnItem(m_priorityItem.getPath(),activateOptionItem);
    this->end();
}

