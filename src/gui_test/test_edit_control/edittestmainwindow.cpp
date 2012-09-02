#define WNCK_I_KNOW_THIS_IS_UNSTABLE

#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnome/gnome-desktop-item.h>
#include <gio/gio.h>

#include "edittestmainwindow.h"
#include "ui_edittestmainwindow.h"

#include "multiTextDisplay.h"
#include "previewpane.h"
#include "globals.h"

QString numAsStr(int num){
    QString output;
    int counter = num;
    while(counter > 0){
        int new_num = counter/10;
        short int j = counter - new_num*10;
        output.append('a' + j);
        counter = new_num;
    }
    return output;
}


EditTestMainWindow::EditTestMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::EditTestMainWindow)
{
    m_ui->setupUi(this);
    g_gsr->setTargetWindow(m_ui->IconWidget);

    //createTransparentWindows();

//    MultiTextDisplay* multiTextDisplay = new MultiTextDisplay();
//    multiTextDisplay->show();
//    multiTextDisplay->raise();

    //CatItem it("http://qt.nokia.com/");
    PreviewPane* m_previewPane = new PreviewPane();
    m_previewPane->setGeometry(100,100,400,400);
    m_previewPane->show();
    m_previewPane->update();
    //m_previewPane->load(QUrl("http://againstsleepandnightmare.com"));
    CatItem it("http://news.ycombinator.com");
    m_previewPane->setItem(it);


    connect(m_previewPane, SIGNAL(loadFinished(bool)),this, SLOT(loadHasFinished()));

    //CatItem it("test://test");
    ListItem li(it);
    QIcon qi = QIcon::fromTheme(PLUS_ICON_NAME);
    li.setActualIcon(qi);
    g_gsr->initializeList();//probably not necessary
    m_ui->IconWidget->addItem(li,0);
    m_ui->IconWidget->resize(400,20);


    QPushButton* pBut = m_ui->parentButton;
    QPushButton* childBut = m_ui->childButton;
    connect(pBut, SIGNAL(clicked()), this, SLOT(addParentItems()));
    connect(childBut, SIGNAL(clicked()), this, SLOT(addChildItems()));

    QPushButton* expdBut = m_ui->expandListButton;
    QPushButton* shrnkBut = m_ui->shrinkListButton;
    connect(expdBut, SIGNAL(clicked()), this, SLOT(expandListWindow()));
    connect(shrnkBut , SIGNAL(clicked()), this, SLOT(shrinkListWindow()));

    m_ui->listDisplayWidget->addTopicMessage("Top Msg");
    m_ui->listDisplayWidget->addNavigationMessage("Nav Msg");

    m_initialListGeo = m_ui->listDisplayWidget->geometry();



    QList<ListItem> ll;
    for(int i=0; i < 10;i++){
        CatItem it(numAsStr(i*9+77));
        it.setUseDescription(true);
        it.setDescription(numAsStr(i*4+8));
        ListItem li(it);
        QIcon qi = QIcon::fromTheme(PLUS_ICON_NAME);
        li.setActualIcon(qi);
        ll.append(li);
    }
    m_ui->listDisplayWidget->addRawList(ll);



}

void EditTestMainWindow::createTransparentWindows(){
    //Transparent window stuff
    m_fancyLabel = new FancyLabel();//no parent - seperate window
    QImage transImage("transparent_image.png");
    m_fancyLabel->setVisible(true);
    m_fancyLabel->raise();
    m_fancyLabel->activateWindow();
    m_fancyLabel->setupImage(transImage, 25, 6);
}

void EditTestMainWindow::shrinkListWindow(){
    //m_fancyLabel->setCurrentHeight(50);

    //Weird bug not worth chasing down HERE ... I'll chase if it reappear elsewhere...
//    Q_ASSERT(m_previewPane);
//    CatItem it("http://againstsleepandnightmare.com");
//    m_previewPane->setItem(it);


    QRect r = m_initialListGeo;
    QPoint p = r.bottomLeft();
    r.setHeight(1);
    r.moveTopLeft(p);
    m_ui->listDisplayWidget->animateToLocation(r);

}

void EditTestMainWindow::expandListWindow(){

    m_ui->listDisplayWidget->animateToLocation(m_initialListGeo);

}


void EditTestMainWindow::addParentItems(){

    QList<ListItem> ll;
    for(int i=0; i < 10;i++){
        CatItem it(numAsStr(i*5+10));
        it.setUseDescription(true);
        it.setDescription( (numAsStr(i*4+8)));
        ListItem li(it);
        QIcon qi = QIcon::fromTheme(PLUS_ICON_NAME);
        li.setActualIcon(qi);
        ll.append(li);
    }
    m_ui->listDisplayWidget->addParentList(ll);


}



void EditTestMainWindow::addChildItems(){
    QList<ListItem> ll;
    for(int i=0; i < 10;i++){
        CatItem it(numAsStr(i*3+20));
        it.setUseDescription(true);
        it.setDescription((numAsStr(i*6+11)));
        ListItem li(it);
        QIcon qi = QIcon::fromTheme(PLUS_ICON_NAME);
        li.setActualIcon(qi);
        ll.append(li);
    }
    m_ui->listDisplayWidget->addChildList(ll);

}



EditTestMainWindow::~EditTestMainWindow()
{
    delete m_ui;
    //m_fancyLabel->deleteLater();
}

void EditTestMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
