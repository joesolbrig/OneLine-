

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

FancyContextMenu::FancyContextMenu(CatItem priorityItem, QList<CatItem> actionChildren):
        QWidget(0, Qt::FramelessWindowHint | Qt::Tool ), m_layout(QBoxLayout::LeftToRight,this), m_slider(this)
{

    //slider
    m_slider.setTickPosition(QSlider::TicksLeft);
    m_slider.setMinimum(0);
    m_slider.setMaximum(log(MAX_MAX_EXTERNAL_WEIGHT));
    int w = priorityItem.getScaledSourceWeight();
    qDebug() << "FancyContextMenu setting scaledWeight:" << w;
    m_slider.setValue(w);
//    QStyle s;
//    m_slider.setStyle(&s);
    connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    m_layout.setMargin(0);
    m_layout.addWidget(&m_slider,Qt::AlignLeft);

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
    connect(this, SIGNAL(operateOnItem(QString, CatItem )),gMainWidget, SLOT(operateOnItem(QString, const CatItem )));
    QRect g = geometry();
    g.setHeight(m_menu.height());
    g.setWidth(m_slider.geometry().width() + m_menu.geometry().width());
    setGeometry(g);
    m_layout.addWidget(&m_menu,0,Qt::AlignRight);

    //layout
    m_priorityItem = priorityItem;
    setLayout(&m_layout);
    m_layout.update();
    m_layout.setMargin(0);
    adjustSize();
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setFocus();
}

FancyContextMenu::~FancyContextMenu(){
    gMainWidget->m_contextMenu =0;
    destroy();
}


void FancyContextMenu::sliderChanged(int value){
    CatItem setPrioritItem(addPrefix(OPERATION_PREFIX,SET_PRIORIT_OPERATION));
    if(m_priorityItem.getScaledSourceWeight() !=value){
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

