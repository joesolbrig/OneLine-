

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



MultiItemLabel::MultiItemLabel(FancyContextMenu* parent): QLabel(parent) {
//Done by parent
//    connect(this, SIGNAL(itemChanged()),
//            parent, SLOT(sourceChanged()));
    m_currentItemIndex=-1;
}






void MultiItemLabel::paintEvent(QPaintEvent * evt){
    QLabel::paintEvent(evt);
    if(m_items.count()==1){ return;}

    QIcon down_triangle = QIcon::fromTheme(DOWN_ARROW);
    QIcon up_triangle = QIcon::fromTheme(UP_ARROW);
    Q_ASSERT(m_currentItemIndex<=m_items.count()-1);
    Q_ASSERT(m_currentItemIndex>=0);

    if(this->m_currentItemIndex==0){
        up_triangle = IconDelegate::grayIfy(up_triangle);
    }
    if(m_currentItemIndex==m_items.count()-1){
        down_triangle = IconDelegate::grayIfy(down_triangle);
    }


    QPainter painter(this);
    painter.drawImage(
            downRect(),
            down_triangle.pixmap(downRect().size()).toImage()
    );

    painter.drawImage(
            upRect(),
            up_triangle.pixmap(upRect().size()).toImage()
    );
}



FancyContextMenu::FancyContextMenu(CatItem theItem, QList<CatItem> actionChildren):
        QWidget((QWidget*)gMarginWidget,
                Qt::FramelessWindowHint),
                m_layout(QBoxLayout::TopToBottom,this),
                m_sublayout(QBoxLayout::LeftToRight), m_slider(this)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_LayoutOnEntireRect,true);
    setAutoFillBackground(true);
    setObjectName("listContextMenu");


    //slider
    m_slider.setTickPosition(QSlider::TicksLeft);
    m_slider.setMinimum(0);
    m_slider.setMaximum((WEIGHT_TICS-1));

    m_itemActedOn = theItem;
    QList<CatItem> priorityParents = m_itemActedOn.getSearchSourceParents();
    connect(&m_topLabel, SIGNAL(itemChanged()),
            this, SLOT(sourceChanged()));
    connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    m_topLabel.setItemList(priorityParents);
    sourceChanged();
    QFont f = ListItem(m_topLabel.curentItem()).getDisplayFont();
    QRect r = m_topLabel.geometry();
    QFontMetrics fm(f);
    r.setHeight(fm.height());
    m_topLabel.setGeometry(r);

    m_layout.addWidget(&m_topLabel, 0, Qt::AlignTop);
    m_sublayout.addWidget(&m_slider,0, Qt::AlignLeft);

    //menu
    for(int i=0; i<actionChildren.count();i++){
        CatItem actionItem = actionChildren[i];
        QString name = actionItem.getName();
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

    QString itemPath = m_topLabel.curentItem().getPath();
    Q_ASSERT(!itemPath.isEmpty());
    if(m_topLabel.curentItem().getSourceWeightTics() !=value){
        m_topLabel.curentItem().setWeightTics(value);

        CatItem setPrioritItem(addPrefix(OPERATION_PREFIX,SET_PRIORIT_OPERATION));
        setPrioritItem.setCustomValue(SET_PRIORITY_KEY_STR,value);
        setPrioritItem.setLabel(RESET_FILTER_ICONS_KEY);

        //Set value here and save the value in the main item
        m_topLabel.curentItem().setCustomValue(SET_PRIORITY_KEY_STR,value);
        m_topLabel.curentItem().setLabel(RESET_FILTER_ICONS_KEY);
        QString itemPath = m_topLabel.curentItem().getPath();
        gMainWidget->operateOnItem(itemPath,setPrioritItem);
    }
}

void FancyContextMenu::sourceChanged(){
    CatItem priorityParent = m_topLabel.curentItem();
    int w = priorityParent.getSourceWeightTics();
    qDebug() << "FancyContextMenu setting scaledWeight:" << w;
    m_slider.setValue(w);
}


void FancyContextMenu::optionChosen(){
    QAction* a = (QAction*)this->sender();
    QString name = a->text();
    Q_ASSERT(m_optionItems.contains(name));
    CatItem optionItem = m_optionItems[name];
    Q_ASSERT(!optionItem.isEmpty());
    CatItem activateOptionItem(addPrefix(OPERATION_PREFIX,ACTIVATE_OPTION_ITEM));
    activateOptionItem.addChild(optionItem);
    gMainWidget->operateOnItem(m_itemActedOn.getPath(),activateOptionItem);
    this->end();
}

