/*
Oneline: Application Launcher
Copyright (C) 2010  Hans J. Solbrig

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/



//Test routines
#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QPalette>
#include <QLineEdit>
#include <QPixmap>
#include <QBitmap>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QSettings>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTranslator>
#include <QtNetwork>
#include <parser.h>

#include "icon_delegate.h"
#include "edit_controls.h"
#include "main.h"
#include "globals.h"
#include "options.h"
#include "plugin_interface.h"
#include "cat_builder.h"

#include "util/mmap_file/tree_struct_test.h"
#include "plugins/streamplugin/streamplugin.h"
#include "information_coalator.h"

//#ifdef TEST
#include "test.h"

#include "test_defines.h" //ONLY include this only per project!!
#include "util/mmap_file/btree_impl.h"

#include "cat_store.h"


//This goes between 'a' and 'a'+10,
//That shouldn't change any results since includes
//Vowels and consonants...
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



const QString ALL_INDEX("All_Index");
const QString ODD_INDEX("Odd_Index");
const QString EVEN_INDEX ("Even_Index");
const QString MOD10_INDEX("Mod10_Index");


QString mainHtmlize(QString str){
    return QString("<html><body><p>") + str + "</p></body></html>";
}


bool gTest_tree_struct;


QString numAsStr(int num);

bool listContainsPath(QList<CatItem> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == path){
            return true;
        }
    }
    return false;
}

bool listContainsPath(QList<CatItem> items, CatItem it){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == it.getPath()){
            return true;
        }
    }
    return false;
}

bool listContainsPath(QList<QString> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i] == path){
            return true;
        }
    }
    return false;
}

CatItem itemFromPath(QList<CatItem> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == path){
            return items[i];
        }
    }
    return CatItem();
}

bool listContainsItem(QList<CatItem> items, CatItem it){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == it.getPath()){
            return true;
        }
    }
    return false;
}

bool listContainsName(QList<VerbItem> items, QString name){
    for(int i=0;i<items.count();i++){
        if(items[i].getName() == name){
            return true;
        }
    }
    return false;
}

bool listContainsName(QList<CatItem> items, QString name){
    for(int i=0;i<items.count();i++){
        if(items[i].getName() == name){
            return true;
        }
    }
    return false;
}

bool listHasType(QList<CatItem> items, CatItem::ItemType t){
    for(int i=0;i<items.count();i++){
        if(items[i].getItemType() == t){
            return true;
        }
    }
    return false;
}

bool listHasActionType(QList<CatItem> items, CatItem::ActionType t){
    for(int i=0;i<items.count();i++){
        if(items[i].getActionType() == t){
            return true;
        }
    }
    return false;
}

QString consonants(QString s){
    QString str;
    for(int i=0;i<s.length();i++){
        if(isConsonant(s[i])){
            str.append(s[i]);
        }
    }
    return str;
}




TheApplicationTester::TheApplicationTester(){
    gTest_tree_struct=false;
    the_test_builder=0;
    lWind = gMainWidget;
    qDebug() << "Important little detail ... this is SIZEOF CatItem: " << sizeof(CatItem);
    qDebug() << "Another little detail ... this is SIZEOF TrieNode: " << sizeof(TrieNode);

    ((*gDirs)[QString(LOCAL_DIR)]).value(0)=".";

    Catalog* c = CatBuilder::getOrCreateCatalog(*gDirs);
    c = c;
    Q_ASSERT(c  != NULL );
}



TheApplicationTester::~TheApplicationTester(){
    if(the_test_builder){
        delete the_test_builder;
        the_test_builder=0;
    }
    CatBuilder::destroyCatalog();

}

void TheApplicationTester::testMultiPartJson(){
    //Refresh since you'll edit the download file
    if(the_test_builder==0){
        CatBuilder::destroyCatalog();
        the_test_builder = new CatBuilder(CatBuilder::REFRESH, *gDirs, true);
    }
    the_test_builder->catalogTask(CatBuilder::REFRESH);

    // http://www.rawstory.com/rs/2012/01/02/cantor-refuses-to-admit-reagan-raised-taxes/
    CatItem it("http://www.popsci.com/science/article/2012-01/new-material-can-pull-carbon-dioxide-right-out-air-unprecedented-rates");

    QList<CatItem> res = CatBuilder::loadExtensionSourceForPlugins();
    Q_ASSERT(res.count()>0);
    CatItem redditSource = res[0];

    QHash<QString, EntryField> vars;
    vars[EXTERN_SOURCE_ARG_PATH].value= it.getPath();
    vars[EXTERN_SOURCE_ARG_NAME].value= "";
    CatItem resItem = FillinItem::createChildren(redditSource,vars);
    Q_ASSERT(resItem.getChildRelationCount()>4);


    QFile sampleRedditResponse("test/testData/Reddit_posts.cached_feed");
    Q_ASSERT(sampleRedditResponse.exists());
    sampleRedditResponse.open(QIODevice::ReadOnly);

    QByteArray rawRedditJson= sampleRedditResponse.readAll();

    QJson::Parser parser;
    bool ok=true;
    QVariant parsedResult = parser.parse(rawRedditJson, &ok);
    if (!ok) {
        qWarning() << "Error while parsing test data: " << rawRedditJson;
        return;
    }
    VariantTree vt(parsedResult);

    QString fillinFile = redditSource.getCustomString(FILLIN_GEN_FILE_KEY);
    redditSource.setCustomPluginInfo(FILLIN_GEN_SUBKEY_KEY, "redditcomments");
    QString fillinKey = redditSource.getCustomString(FILLIN_GEN_SUBKEY_KEY);

    Q_ASSERT(!fillinFile.isEmpty() &&
       !fillinKey.isEmpty());

    QSettings set(fillinFile, QSettings::IniFormat);
    SettingsTree st(&set,fillinKey);
    FillinItem fi;
    fi.setGeneratingItem(redditSource);
    fi.addData(&st,0,0);
    fi.setGenerating();
    fi.addData(&vt,0,0);
    VerbItem vi(it);
    CatItem bi = fi.createTheItem(vi, QHash<QString, EntryField>());
    Q_ASSERT(bi.getChildRelationCount()>10);

    it.setLabel(HTML_HEADER_PROCESSED_KEY);
    QList<CatItem> toUpdate;
    toUpdate.append(it);

    CatBuilder* builder = new CatBuilder(CatBuilder::EXTEND, *gDirs);
    builder->m_userItems = toUpdate;
    builder->m_userKeys = "";
    builder->m_keywords = QStringList();
    builder->m_extensionType = UserEvent::SELECTED;
    builder->m_extendDefaultSources = false;

    builder->catalogTask(CatBuilder::EXTEND);

    QList<CatItem> resL = builder->m_extension_results;
    Q_ASSERT(resL.count()>1);
}

void TheApplicationTester::testInputListAsHash(){

    InputList inputList;

    //Create custom verb

    //add custom verb

    //Test we get all with unfilled hash and none w/ fill
    //

    CatItem custVerb("Copy");
    custVerb.setCustomPluginInfo(CUSTOM_VERB_DISCRIPTION_KEY_STR, " copy $filename:file+ to $destination:filefolder+");
    custVerb.setCustomPluginInfo(VERB_COMMAND_STRING_KEY, "cp $filename $destination");
    custVerb.setItemType(CatItem::VERB);
    custVerb.setUseDescription(true);

    QString desStr = custVerb.getVerbDescription();
    qDebug() << desStr;

    inputList.setItem(custVerb);

    inputList.addSlot();

    QHash<QString, EntryField> testHash;
    QList<CatItem> argList1;
    argList1= inputList.getRemainingCustomArgs();
    Q_ASSERT(argList1.count()==2);
    testHash=inputList.asFieldValueHash();
    Q_ASSERT(testHash.count()==0);
    Q_ASSERT(inputList.atFieldChoicePoint());

    inputList.setItem(argList1[0]);
    Q_ASSERT(inputList.atFieldChoicePoint());
    inputList.appendUserkeys("d");
    QString argName = inputList.getArgNameAt(1);
    Q_ASSERT(argName == "filename");

    CatItem dummyFile("/dummypath", "dummy");
    inputList.setItem(dummyFile);
    argName = inputList.getArgNameAt(1);
    Q_ASSERT(argName == "filename");
    Q_ASSERT(!inputList.atFieldChoicePoint());

    QList<CatItem> argList2=inputList.getRemainingCustomArgs();
    Q_ASSERT(argList2.count()==1);
    testHash=inputList.asFieldValueHash();
    Q_ASSERT(testHash.count()==1);
    Q_ASSERT(!inputList.atFieldChoicePoint());
    inputList.addSlot();

    inputList.appendUserkeys("f");
    inputList.setItem(argList2[0]);
    argList2=inputList.getRemainingCustomArgs();
    Q_ASSERT(argList2.count()==0);
    int nB = inputList.nounBeginSlot();
    nB = nB;
    int nE = inputList.fullfilledFieldsEnd();
    nE = nE;
    Q_ASSERT(nE-nB ==1);
    testHash=inputList.asFieldValueHash();
    Q_ASSERT(testHash.count()==2);
    CatItem fakeFile("/fakepath", "fake");
    inputList.setItem(fakeFile);
    argName = inputList.getArgNameAt(2);
    Q_ASSERT(argName== "destination");
    Q_ASSERT(!inputList.atFieldChoicePoint());

    argList2=inputList.getRemainingCustomArgs();
    Q_ASSERT(argList2.count()==0);
    testHash=inputList.asFieldValueHash();
    Q_ASSERT(testHash.count()==2);
    QString dmsg;
    Q_ASSERT(inputList.canRun(&dmsg));

}


void TheApplicationTester::extensionCycler(QList<CatItem> inList, QList<CatItem>* outList){
    if(!the_test_builder)
        { the_test_builder = new CatBuilder(CatBuilder::EXTEND, *gDirs); }

    int pluginCount = CatBuilder::getPluginHandle()->getPlugins().count();
    for(int i=0; i< pluginCount; i++){
        gTime_offset_for_testing += 1000*1000;
        the_test_builder->m_userItems = inList;
        the_test_builder->start(QThread::NormalPriority);
        the_test_builder->wait(100000000);
        outList->append(the_test_builder->m_extension_results);
    }

}






void TheApplicationTester::testOHash(){
    //Test the 'order hash' combined container
    OHash ohi;

    ListItem b1("b1");
    ohi.append(b1);
    Q_ASSERT(ohi.at(0) ==b1);

    ListItem b2("b2");
    ohi.append(b2);
    Q_ASSERT(ohi.at(1) ==b2);
    ListItem b3("b3");
    ohi.append(b3);
    Q_ASSERT(ohi.at(2) ==b3);
    ListItem b4("b4");
    ohi.append(b4);
    Q_ASSERT(ohi.at(3) ==b4);
    ohi.append(b4);
    ListItem b5("b5");
    ohi.append(b5);
    ListItem b6("b6");
    ohi.append(b6);
    ohi.append(b1);
    Q_ASSERT(ohi.at(0) ==b1);
    ohi.append(b2);
    Q_ASSERT(ohi.at(1) ==b2);
    //Test the order-list function too

}


void TheApplicationTester::testInformationFormat1(){

    CatItem it("/path/to", "Diary");

    ListItem li(it);
    li.setNextKeyIndex(0);
    QString formattedName = li.formattedName();
    QString desired = htmlize("<p><b><font color=\"" NEXT_KEY_FORMAT  "\"><u>D</u></font>iary</b></p>");
    Q_ASSERT(formattedName == desired);

    li.setNextKeyIndex(2);
    formattedName = li.formattedName();
    desired = htmlize("<p><b>Di<font color=\"" NEXT_KEY_FORMAT  "\"><u>a</u></font>ry</b></p>");
    Q_ASSERT(formattedName == desired);

    const int path_size = 17;
    const char* paths[path_size]= {

        "apps/kdeutils-4.3.2/kwallet/konfigurator/kwalletconfig.desktop",
                "/apps/aMule-2.2.6/po/gl.po",
                "/apps/aMule-2.2.6/po/et_EE.po",
                "/apps/firefox3.5/firefox/dictionaries/en-US.dic",
                "/apps/Simple_sheet/SimpleGroupware_0.520/src/lib/browser/browser.php",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/smb.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/ns71oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/nn301oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op962oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/ie60oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/nn475oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/firefox20oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op85oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/firefox30oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op90oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/kdepasswd/kcm/kcm_useraccount.desktop",
                "/apps/kdebase-4.3.2/apps/kdepasswd/kdepasswd.desktop"

    };

    QList<CatItem> l;
    QList<ListItem> ll;
    for(int i=0;i < path_size;i++){
        QString p(paths[i]);
        CatItem itm(p);
        l.append(itm);
        ListItem litm(p);
        ll.append(litm);
    }

    QList<ListItem> lres = sortListItems(ll, USER_DESCRIPTION_KEY_STR);

    Q_ASSERT(ll.count() == lres.count());
    Q_ASSERT(ll.toSet() == lres.toSet());



    InformationCoalator iC;


    QString fontStr("yellow");
    //int rowsAvail = 13; //less than array length..
    int colsAvail = 30;
    iC.addList(l);
    iC.relativeInformationFormatting();
    iC.setNextChoiceKeys(colsAvail);
    //iC.relativeInformationFormatting(rowsAvail, colsAvail, fontStr);

    QChar prevC = ' ';
    for(int i=0;i<path_size;i++){
        ListItem& listI = iC.at(i);
        QChar kC = listI.hotkeyChar();
        Q_ASSERT(kC !=prevC);
        Q_ASSERT(iC.at(i).hotkeyChar() == kC);
        Q_ASSERT(iC.at(i).getUsePath());
        prevC = kC;
    }

    QString prevS = "";
    int prevMax=0;
    int prevMin=0;
    for(int i=0;i<14;i++){
        ListItem& listI = iC.at(i);
        QString s = iC.at(i).getFormattedPath(colsAvail);
        QString g = iC.at(i).getRawDisplayPath();
        qDebug() << "formatted: " << s;
        qDebug() << "unformatted: " << g;
        Q_ASSERT(s !=prevS);
        //allow slop for the next one,
        Q_ASSERT(abs(g.length() - MIN(listI.getPath().length(), colsAvail)) < 7);
        prevS = iC.at(i).getFormattedPath(colsAvail);

        if(i>0){
            Q_ASSERT(listI.getMaxSplit() == prevMax
                     ||(prevMin == listI.getMinSplit()) ||
                     (prevMax == listI.getMinSplit()) ||
                      (prevMin == listI.getMaxSplit()));
        }
        prevMax = listI.getMaxSplit();
        prevMin = listI.getMinSplit();
    }

}

void TheApplicationTester::testCoalateItems2(){

    //int loop_var = 5;
    CatItem inCat1("InCat1");
    CatItem inCat2("InCat2");
    CatItem inCat3("InCat3");
    CatItem inCat4("InCat4");
    CatItem outCat1("OutCat1");
    CatItem outCat2("OutCat2");
    CatItem outCat3("OutCat3");

    CatItem always("always");

    ListItem b1("b1");
    ListItem b2("b2");
    ListItem b3("b3");
    ListItem b4("b4");
    ListItem b5("b5");
    ListItem b6("b6");
    ListItem b7("b7");
    ListItem b8("b8");
    ListItem b9("b9");
    ListItem b10("b10");

    ListItem b11("b11");
    ListItem b12("b12");
    ListItem b13("b13");
    ListItem b14("b14");
    ListItem b15("b15");
    ListItem b16("b16");

    {
        QString tk("Test_key");
        QList<ListItem> sorterItems;
        b1.setCustomPluginValue(tk,10);
        sorterItems.append(b1);
        b2.setCustomPluginValue(tk,9);
        sorterItems.append(b2);
        b3.setCustomPluginValue(tk,8);
        sorterItems.append(b3);
        b4.setCustomPluginValue(tk,6);
        sorterItems.append(b4);
        b5.setCustomPluginValue(tk,7);
        sorterItems.append(b5);
        //no value case
        sorterItems.append(b6);
        b7.setCustomPluginValue(tk,103);
        sorterItems.append(b7);
        b8.setCustomPluginValue(tk,10101);
        sorterItems.append(b8);
        b9.setCustomPluginValue(tk,145);
        sorterItems.append(b9);
        QList<ListItem> res = sortListItems(sorterItems, tk);
        ListItem f = res[0];
        for(int i=0; i< res.count(); i++){
            if(i >0){
                int v1 = f.getCustomValue(tk);
                v1=v1;
                int v2 = res[i].getCustomValue(tk);
                v2 = v2;
                Q_ASSERT( v1 < v2 );
            }
            f = res[i];
        }
        Q_ASSERT(res.count() == sorterItems.count());
        Q_ASSERT(res.toSet() == sorterItems.toSet());
    }

    //Test whether you can compress sub-windows...
    QList<CatItem> inItems;

    {
        QString tk(TOTAL_WEIGHT_KEY_STR);
        inItems.clear();
        //Cat1
        b1.setCustomPluginValue(tk,500);
        inCat1.addChild(b1);

        Q_ASSERT(b1.getParents().count() >0);
        Q_ASSERT(b1.getParents().contains(inCat1));
        inItems.append(b1);
        b2.setCustomPluginValue(tk,50);
        inCat1.addChild(b2);
        inCat4.addChild(b2);
        Q_ASSERT(b2.getParents().count() >0);
        Q_ASSERT(b2.getParents().contains(inCat1));
        inItems.append(b2);
        b3.setCustomPluginValue(tk,5);
        inCat1.addChild(b3);
        Q_ASSERT(inCat1.getChildren().count() == 3);
        inItems.append(b3);

        //Cat2
        b4.setCustomPluginValue(tk,600);
        b4.addParent(inCat2);
        Q_ASSERT(inCat2.hasChild(b4));

        inItems.append(b4);
        b5.setCustomPluginValue(tk,70);
        b5.addParent(inCat2);
        Q_ASSERT(inCat2.hasChild(b4));
        Q_ASSERT(inCat2.hasChild(b5));
        inItems.append(b5);

        inItems.append(b6);
        b7.setCustomPluginValue(tk,103);
        b7.addParent(inCat2);
        Q_ASSERT(b7.getParents().count() >0);
        Q_ASSERT(b7.getParents().contains(inCat2) >0);
        inItems.append(b7);

        Q_ASSERT(inCat2.hasChild(b4));
        Q_ASSERT(inCat2.hasChild(b5));
        Q_ASSERT(inCat2.hasChild(b7));
        b8.setCustomPluginValue(tk,10101);
        b8.addParent(inCat2);
        inItems.append(b8);
        Q_ASSERT(inCat2.hasChild(b4));
        Q_ASSERT(inCat2.hasChild(b5));
        Q_ASSERT(inCat2.hasChild(b7));
        Q_ASSERT(inCat2.hasChild(b8));
        int catC = inCat2.getChildren().count();
        catC= catC;
        Q_ASSERT( catC==4);

        b9.setCustomPluginValue(tk,145);
        b9.addParent(inCat3);
        inItems.append(b9);

        InformationCoalator iCoal(inItems);
        iCoal.addCategories();
        //QList<ListItem> oItems = iCoal.toList();
        //QHash <CatItem, QList<CatItem> > categoryItems = iCoal.getCategories();
        //Q_ASSERT(oItems[4]==ListItem(inCat1));
        QList<CatItem> ocItems = iCoal.toCatList();

        //The extra categories are added...
        Q_ASSERT(ocItems.count() ==inItems.count()+1);

        InputList inputList;

        inputList.addListItemList(inItems);

        ListItem lCat(inCat1);

        inputList.setFilterItem(lCat);

        //We're no longer filtering except when there are children of parents..
        //QList<CatItem> li = inputList.getListItems();
        //Q_ASSERT(li.count()==3);

        //Q_ASSERT(categoryItems.keys().contains(inCat1));
    }

}



void TheApplicationTester::testTreeCompress(){


    QList<ListItem> tl;
    QString key("Test_key");
    for(int i=0; i< 20; i++){
        CatItem it(QString::number(i));
        ListItem li(it);
        li.setCustomPluginInfo(key, numAsStr(500 + i*(9.5) - (i*i)));
        tl.append(li);
    }
    QList<ListItem> res = sortListItems(tl,key);


    QString prevStr = res[0].getCustomString(key);
    for(int i=1; i< 20; i++){
        QString curStr = res[i].getCustomString(key);
        Q_ASSERT( curStr > prevStr);
    }

    //Test compress to tree
    tl.clear();
    for(int i=0; i< 20; i++){
        CatItem it(QString::number(i));
        ListItem li(it);
        for(int j=0; j< 30; j++){
            if(j==i){continue;}
            CatItem it2(QString::number(j));
            ListItem li2(it2);
            li.setTotalWeight(abs(5000 - j*j*10 + i*i*i));
            li.addChild(li2);
            if(j % 3 ==0){
                tl.append(li2);
            }

        }
        if(i%5 ==0){
            tl.append(li);
        }
    }

    InformationCoalator iCoal(tl);
    iCoal.informativelyReorder(CatItem(),10);

    res = iCoal.toList();
    Q_ASSERT(res.count() >= 10);
}


void TheApplicationTester::testXslPlugin2(){


    if(!the_test_builder){
        CatBuilder::destroyCatalog();
        the_test_builder = new CatBuilder(CatBuilder::LOAD, *gDirs);
    }
    the_test_builder->catalogTask(CatBuilder::LOAD);


    CatItem extendIt1 = the_test_builder->getItem(XMEL_BOOKMARKS_PATH1);
    Q_ASSERT(!extendIt1.isEmpty());
    Q_ASSERT(extendIt1.isCategorizingSource());
    Q_ASSERT(extendIt1.getRequestUrl() =="/home/hansj/.recently-used.xbel");
    Q_ASSERT(extendIt1.isCategorizingSource());
    CatItem extendIt2 = the_test_builder->getItem(XMEL_BOOKMARKS_PATH2);
    Q_ASSERT(!extendIt2.isEmpty());
    Q_ASSERT(extendIt2.isCategorizingSource());
    Q_ASSERT(extendIt2.getRequestUrl() =="/home/hansj/.recently-used");
    the_test_builder->getCatalog()->reweightItem(extendIt1);
    the_test_builder->getCatalog()->reweightItem(extendIt2);


    extendIt1 = the_test_builder->getItem(XMEL_BOOKMARKS_PATH1);
    extendIt1.getFullWeight();
    extendIt2.getFullWeight();
    Q_ASSERT(!extendIt1.isEmpty());
    extendIt2 = the_test_builder->getItem(XMEL_BOOKMARKS_PATH2);
    Q_ASSERT(!extendIt2.isEmpty());

    QList<CatItem> extenSrc = the_test_builder->getCatalog()->getSourcesForExtension();
    Q_ASSERT(listContainsPath(extenSrc,extendIt1) || listContainsPath(extenSrc,extendIt2));

    SearchInfo search_info;
    QList<CatItem> inItems;
    QList<CatItem> outItems;
    inItems.append(extendIt1);
    inItems.append(extendIt2);
    search_info.itemListPtr = &inItems;
    CatBuilder::getPluginHandle()->extendCatalogs(&search_info, &outItems);

    CatItem d("/home/hansj/Documents/Political/Laptop Diary3.odt");
    Q_ASSERT(listContainsPath(outItems,d));


    //Give time for extension to cycle to proper plugin
//    for(int j=0; j< 15; j++){
//        gTime_offset_for_testing += 1000*100;
//        the_test_builder->catalogTask(CatBuilder::EXTEND);
//    }

//    CatItem final;
//    InputList il;
//    QString userKs;
//    QList<CatItem> ls;
//
//    //-- System specific file names!
//    //-- fill-in in values from YOUR ".recently-used" and ".recently-used.xbel" file
//
//    final = the_test_builder->getItem("/home/hansj/Documents/Political/Laptop Diary3.odt");
//    Q_ASSERT(!final.isEmpty());
//    Q_ASSERT(final.getPath() == "file:///home/hansj/Documents/Political/Laptop%20Diary3.odt");
//    Q_ASSERT(final.getName() == "Laptop%20Diary3.odt");
//    userKs = "Laptop";
//    il.setUserKeys(userKs);
//    ls = the_test_builder->getCatalog()->cat_store.getItemsByKey(
//            userKs,&il, 30);
//    Q_ASSERT(listContainsPath(ls,final));
//    Q_ASSERT(listContainsPath(ls,"file:///home/hansj/Documents/Political/Laptop%20Diary3.odt"));


//    final = the_test_builder->getItem("file:///home/hansj/downloads/img-vqbqn792dcynaypi33.jpg");
//    Q_ASSERT(!final.isEmpty());
//    Q_ASSERT(final.getPath() == "file:///home/hansj/downloads/img-vqbqn792dcynaypi33.jpg");
//    Q_ASSERT(final.getName() == "img-vqbqn792dcynaypi33.jpg");
    //Extra optional for keystroke match testing...
//    userKs = "dcnpp";
//    il.setUserKeys(userKs);
//    ls = the_test_builder->getCatalog()->cat_store.getItemsByKey(
//            userKs,&il, 30);
//    Q_ASSERT(listContainsPath(ls,final));
//    Q_ASSERT(listContainsPath(ls,"file:///home/hansj/downloads/img-vqbqn792dcynaypi33.jpg"));


}

void TheApplicationTester::BatchPseudoTes_ALL_email(){
    CatItem extendIt1("../../mail_test/mail");
    SearchInfo search_info;
    QList<CatItem> inItems;
    QList<CatItem> outItems;
    QString subItemPath;
    extendIt1.setMaxUpdatePasses(45);
    gThrottleUpdates = 0;
    for(int i=0; i<100; i++){
        inItems.clear();
        outItems.clear();
        inItems.append(extendIt1);
        search_info.itemListPtr = &inItems;
        CatBuilder::getPluginHandle()->extendCatalogs(&search_info, &outItems);
        extendIt1 = outItems[outItems.length()-1];// itemFromPath(outItems,extendIt1.getPath());
        Q_ASSERT(!extendIt1.isEmpty());
        QString newSubPath = extendIt1.getLastUpdatePath();
//        if(extendIt1.hasLabel(IS_FULL_TEXT_INDEXED)){
//            break;
//        }
        Q_ASSERT(i >20 || subItemPath != newSubPath || i<4);
        subItemPath = newSubPath;
        CatBuilder::getCatalog()->indexItemList(&outItems);
    }
}

void TheApplicationTester::testXslPlugin(){

    QFile xlsCodefile("./plugins/streamplugin/xml_code/rss/rss.xsl");
    Q_ASSERT(xlsCodefile.exists());
    xlsCodefile.open(QIODevice::ReadOnly |QIODevice::Text);
    QString xlsCode = xlsCodefile.readAll();

    CatItem testSource("http://libcom.org/forums/thought-0/feed");
    testSource.setFilterStr(xlsCode);

    testSource.setLabel(FIREFOX_PLUGIN_NAME);
    testSource.setLabel(HTML_HEADER_PROCESSED_KEY);
    testSource.setLabel(STREAM_SOURCE_PATH);

    testSource.setRequestUrl("http://libcom.org/forums/thought-0/feed");
    Q_ASSERT(testSource.getRequestUrl() == "http://libcom.org/forums/thought-0/feed");
    QList<CatItem> tsl;
    tsl.append(testSource);

    QList<CatItem> results;
    extensionCycler(tsl,&results);

    Q_ASSERT(results.count() > 10);
    Q_ASSERT(!results[0].getName().isEmpty());
    Q_ASSERT(!results[1].getName().isEmpty());
    Q_ASSERT(listContainsName( results, "Direct action and electoralism"));


}

void TheApplicationTester::testUpdateUsage(){
    CatBuilder::getCatalog()->clearAll();

    int old_time_offset = gTime_offset_for_testing;

    CatItem src("externalSource1");
    CatItem src2("externalParent");
    src.setSourceWeight(HIGH_EXTERNAL_WEIGHT,src2);
    src.setTagLevel(CatItem::INTERNAL_SOURCE);
    CatBuilder::getCatalog()->addItem(src);
    CatItem srcSaved = CatBuilder::getCatalog()->getItem("externalSource1");
    Q_ASSERT(srcSaved.isCategorizingSource());

    CatItem e1("externalItem1");
    e1.setExternalWeight(HIGH_EXTERNAL_WEIGHT,src);
    CatItem e2("externalItem2");
    e2.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,src);
    CatBuilder::getCatalog()->addItem(e1);
    CatBuilder::getCatalog()->addItem(e2);

    CatItem eri1 = CatBuilder::getCatalog()->getItem("externalItem1");
    CatItem eri2 = CatBuilder::getCatalog()->getItem("externalItem2");
    CatBuilder::getCatalog()->reweightItems();
    long ew1 = eri1.getFullWeight();
    long ew2 = eri2.getFullWeight();
    Q_ASSERT(ew1 < ew2); //Full weight goes down versus higher weight going up.
    //Reweigh items
    for(int i=0;i < 24;i++){
        CatBuilder::getCatalog()->reweightItems();
    }
    CatBuilder::getCatalog()->reweightItems();

    eri1 = CatBuilder::getCatalog()->getItem("externalItem1");
    eri2 = CatBuilder::getCatalog()->getItem("externalItem2");
    ew1 = eri1.getFullWeight();
    ew2 = eri2.getFullWeight();

    Q_ASSERT(ew1 < ew2); //Full weight goes down versus higher weight going up.

    //Take three items
    CatItem item1("two_w_old");
    CatItem item2("one_w_old");
    CatItem item3("now_old");
    item1.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,src);
    item2.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,src);
    item3.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,src);

    CatBuilder::getCatalog()->addItem(item1);
    CatBuilder::getCatalog()->addItem(item2);
    CatBuilder::getCatalog()->addItem(item3);
    lWind->m_inputList.setItem(item1);
    //One gets 3 calls "now"
    gTime_offset_for_testing = 0;
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);
    gTime_offset_for_testing += 10;
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);
    gTime_offset_for_testing += 10;
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);

    CatBuilder::getCatalog()->reweightItem(item1);
    CatBuilder::getCatalog()->reweightItem(item2);
    CatBuilder::getCatalog()->reweightItem(item3);

    CatItem ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
    CatItem ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
    CatItem ri3 = CatBuilder::getCatalog()->getItem("now_old");
    long rw1 = ri1.getRelevanceWeight();
    long rw2 = ri2.getRelevanceWeight();
    long rw3 = ri3.getRelevanceWeight();
    Q_ASSERT(rw1 > rw2);
    Q_ASSERT(rw1 > rw3);

    //One gets 2 calls later
    gTime_offset_for_testing += 2*5* 7*24*60*60;
    lWind->m_inputList.setItem(item2);
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);
    gTime_offset_for_testing += 10;
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);

    CatBuilder::getCatalog()->reweightItem(item1);
    CatBuilder::getCatalog()->reweightItem(item2);
    CatBuilder::getCatalog()->reweightItem(item3);

    ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
    ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
    ri3 = CatBuilder::getCatalog()->getItem("now_old");
    rw1 = ri1.getRelevanceWeight();
    rw2 = ri2.getRelevanceWeight();
    rw3 = ri3.getRelevanceWeight();
    Q_ASSERT(rw1 < rw2);
    Q_ASSERT(rw1 > rw3);
    srcSaved = CatBuilder::getCatalog()->getItem("externalSource1");
    Q_ASSERT(srcSaved.isCategorizingSource());
    //one gets 1 calls four weeks later
    //one was just called
    gTime_offset_for_testing += 24*60*60;

    lWind->m_inputList.setItem(item3);
    CatBuilder::getCatalog()->setExecuted(&lWind->m_inputList);


    srcSaved = CatBuilder::getCatalog()->getItem("externalSource1");
    Q_ASSERT(srcSaved.isCategorizingSource());
    //Reweigh items
    CatBuilder::getCatalog()->reweightItem(item1);
    CatBuilder::getCatalog()->reweightItem(item2);
    CatBuilder::getCatalog()->reweightItem(item3);

    //get entries from catalog
    ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
    ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
    ri3 = CatBuilder::getCatalog()->getItem("now_old");

    rw1 = ri1.getRelevanceWeight();
    rw2 = ri2.getRelevanceWeight();
    rw3 = ri3.getRelevanceWeight();
    Q_ASSERT(rw1 <= rw2);
    Q_ASSERT(rw2 < rw3);

    long dw1 = ri1.getFullWeight();
    dw1 = dw1;
    long dw2 = ri2.getFullWeight();
    dw2 = dw2;
    long dw3 = ri3.getFullWeight();
    dw3 = dw3;

    Q_ASSERT(dw1 > dw2);
    Q_ASSERT(dw2 > dw3);

    //Check they begin in time order

    //advance the clock a month
    gTime_offset_for_testing = 10*7*24*60*60; //...weeks later in seconds


    //Reweigh items
    for(int i=0;i < 24;i++){
        CatBuilder::getCatalog()->reweightItems();
    }
    //get entries from catalog
    ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
    ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
    ri3 = CatBuilder::getCatalog()->getItem("now_old");

    long fw1 = ri1.getFullWeight();
    fw1 = fw1;
    long fw2 = ri2.getFullWeight();
    fw2 = fw2;
    long fw3 = ri3.getFullWeight();
    fw3 = fw3;

    //Check they are now in frequency order
    Q_ASSERT(fw1 > fw2);
    Q_ASSERT(fw2 < fw3);

    gTime_offset_for_testing = old_time_offset;

}

void TheApplicationTester::testUpdateUsage2(){

    //Cat_Store cs(true,".");
    CatBuilder::getCatalog()->clearAll();

    CatItem src("externalSource1");
    CatItem src2("externalParent");
    src.setSourceWeight(MEDIUM_EXTERNAL_WEIGHT*10,src2);
    src.setTagLevel(CatItem::INTERNAL_SOURCE);
    CatBuilder::getCatalog()->addItem(src);

    int levels =5;
    for(int i=levels-1;i>=0;i--){
        float expI = pow(2,i);
        for(int j=0;j<expI;j++){
            //should give swaths of similar names but with gaps
            QString path = QString("externalItem1") +
                              numAsStr(i*2375400 + j*7+1000); //QString("longPrefix") +
            CatItem e1( path);
            e1.setExternalWeight(
                    MAX(MAX_MAX_EXTERNAL_WEIGHT/(expI+0.2), MEDIUM_EXTERNAL_WEIGHT),src);
            CatBuilder::getCatalog()->addItem(e1);
            CatItem eri1 = CatBuilder::getCatalog()->getItem(path);
            Q_ASSERT(eri1.getPath() == path);
        }
    }
    for(int i=0;i<levels;i++){
        CatBuilder::getCatalog()->reweightItems();
    }

    for(int i=0;i<levels;i++){
        for(int j=0;j<i;j++){
            //should give swaths of similar names but with gaps
            QString path = QString("externalItem1") +
                              numAsStr(i*2375400 + j*7+1000); //QString("longPrefix") +
            CatItem eri1 = CatBuilder::getCatalog()->getItem(path);
            Q_ASSERT(eri1.getPath() == path);
            qDebug() <<"ij:" << i << "," << j << " rel weight: "
                    << eri1.getRelevanceWeight();
            qDebug() <<"ij:" << i << "," << j << " weight : " << eri1.getFullWeight();
            int wSection = eri1.getWeightSection();
            qDebug() <<"ij:" << i << "," << j << " weight section: " << wSection;
            Q_ASSERT( wSection < i+2);
        }
    }
}

void TheApplicationTester::testItemFormatRoutine(){

    CatItem it("/path/to", "Diary");

    ListItem li(it);
    li.setNextKeyIndex(0);
    QString formattedName = li.formattedName();
    QString expected = htmlize("<p><b><font color=\"" NEXT_KEY_FORMAT  "\"><u>D</u></font>iary</b></p>");
    Q_ASSERT(formattedName.simplified() == expected.simplified());

    li.setNextKeyIndex(2);
    formattedName = li.formattedName();
    expected = htmlize("<p><b>Di<font color=\"" NEXT_KEY_FORMAT  "\"><u>a</u></font>ry</b></p>");
    Q_ASSERT(formattedName == expected);



    const int path_size = 17;
    const char* paths[path_size]= {

        "apps/kdeutils-4.3.2/kwallet/konfigurator/kwalletconfig.desktop",
                "/apps/aMule-2.2.6/po/gl.po",
                "/apps/aMule-2.2.6/po/et_EE.po",
                "/apps/firefox3.5/firefox/dictionaries/en-US.dic",
                "/apps/Simple_sheet/SimpleGroupware_0.520/src/lib/browser/browser.php",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/smb.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/ns71oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/nn301oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op962oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/ie60oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/nn475oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/firefox20oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op85oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/firefox30oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/konqueror/settings/kio/uasproviders/op90oncurrent.desktop",
                "/apps/kdebase-4.3.2/apps/kdepasswd/kcm/kcm_useraccount.desktop",
                "/apps/kdebase-4.3.2/apps/kdepasswd/kdepasswd.desktop"

    };

    QList<CatItem> l;
    for(int i=0;i < path_size;i++){
        QString p(paths[i]);
        CatItem itm(p);
        l.append(itm);
    }
    InputList il;
    QString fontStr("yellow");
    //int rowsAvail = 13; //less than array length..
    int colsAvail = 30;
    il.addListItemList(l);

    int selected=0;
    QList<ListItem> formattedList = il.getFormattedListItems(selected,path_size, colsAvail);

    QChar prevC = ' ';

    QSet<QChar> keySet;
    for(int i=0;i<path_size;i++){
        ListItem& listI = formattedList[i];
        QChar kC = listI.hotkeyChar();
        Q_ASSERT(!keySet.contains(kC));
        Q_ASSERT(kC !=prevC);
        Q_ASSERT(il.getListItem(i).hotkeyChar() == kC);
        Q_ASSERT(il.getListItem(i).getUsePath());
        prevC = kC;
        keySet.insert(kC);
    }

    QString prevS = "";
    int prevMax=0;
    int prevMin=0;
    for(int i=0;i<14;i++){
        ListItem& listI = il.getListItem(i);
        QString s = il.getListItem(i).getFormattedPath(colsAvail);
        QString g = il.getListItem(i).getRawDisplayPath();
        qDebug() << "formatted: " << s;
        qDebug() << "unformatted: " << g;
        Q_ASSERT(s !=prevS);
        //allow slop for the next one,
        Q_ASSERT(abs(g.length() - MIN(listI.getPath().length(), colsAvail)) < 7);

        prevS = il.getListItem(i).getFormattedPath(colsAvail);

        if(i>0){
            Q_ASSERT(listI.getMaxSplit() == prevMax
                     ||(prevMin == listI.getMinSplit()) ||
                     (prevMax == listI.getMinSplit()) ||
                      (prevMin == listI.getMaxSplit()));
        }
        prevMax = listI.getMaxSplit();
        prevMin = listI.getMinSplit();
    }

}

void TheApplicationTester::test_ItemTrie1(){
    sub_test_ItemTrie(10);

}

void TheApplicationTester::test_FindNamesWitSpaces(){
    Cat_Store cs(false);
    TrieNode items_by_keys(&cs);

    CatItem ci("path_to_an_item");
    ci.setDescription("name that is long with many spaces");
    ci.setUseDescription(true);

    QList<int> keyList;
    cs.addItem(ci);
    items_by_keys.insert(ci,keyList);

    ItemFilter ifilter;
    QList<CatItem> res;
    res = items_by_keys.find("with",10,&ifilter);
    Q_ASSERT(listContainsItem(res,ci));
    res = items_by_keys.find("many",10,&ifilter);
    Q_ASSERT(listContainsItem(res,ci));
    res = items_by_keys.find("space",10,&ifilter);
    Q_ASSERT(listContainsItem(res,ci));


}

void TheApplicationTester::test_ItemTrie2(){
    sub_test_ItemTrie(100);

}





void TheApplicationTester::sub_test_ItemTrie(int loop_var){

    QList<CatItem> search_list;
    QList<long> itK;
    Cat_Store cs;


    TrieNode items_by_keys(&cs);

    for(int i=0;i<loop_var;i++){
        for(int j=0;j<loop_var;j++){
            //swaths of similar names but with gaps
            QString ns = numAsStr(i*13729 + j+10000);
            //We need to also test partial matches here...
            QString cons = consonants(ns);
            Q_ASSERT(!ns.isEmpty());

            CatItem ts(ns);
            ts.setTotalWeight(((i*j*100) % 100)+1);
            if(j % 11 == 0){
                ts.setItemType(CatItem::VERB);
            } else if(j % 11 == 3){
                ts.setItemType(CatItem::OPERATION);
            }

//            if(cons.length() > 3){
//                ts.setTotalWeight(1000);
//            } else {
//                ts.setTotalWeight(10);
//            }
            search_list.push_back(ts);
            Q_ASSERT(search_list.last().getName() == ns);
            QList<int> keyList;
            cs.addItem(ts);
            items_by_keys.insert(ts,keyList);

            QString testKeys = search_list[i].getName();
            ItemFilter ifilter;
            QList<CatItem> res = items_by_keys.findProtected(testKeys,20,0,&ifilter);
            Q_ASSERT(listContainsPath(res,testKeys));
//            ItemRep r(testKeys);
//            Q_ASSERT(res.contains(r));

            //Test key-search filtering by types...
            if(cons.length() > 3 &&(search_list[i].getFullWeight() >75)){
                int bottom = (int)CatItem::MIN_TYPE;
                if(j % 11 == 0){
                    bottom = (int)CatItem::VERB;
                } else if(j % 11 == 3){
                    bottom = (int)(CatItem::OPERATION);
                }
                ItemFilter ifilter;
                res = items_by_keys.findProtected(cons,10,0,&ifilter);
                //ItemRep r2(testKeys);
                Q_ASSERT(listContainsPath(res, testKeys));
                //Q_ASSERT(res.contains(r2));
            }
        }
    }

    int cons_count =0;
    for(int i=0;i<search_list.size();i++){
        QString testKeys = search_list[i].getName();
        ItemFilter ifilter;
        QList<CatItem> res = items_by_keys.findProtected(testKeys,20,0,&ifilter);
        Q_ASSERT(listContainsPath(res, testKeys));

        //We need to also test partial matches here...
        QString cons = consonants(testKeys);
        if(cons.length() > 3 && (search_list[i].getFullWeight() >75) ){
            res = items_by_keys.findProtected(cons,10,0,&ifilter);
            //ItemRep r2(testKeys);
            Q_ASSERT(listContainsPath(res,testKeys));
            cons_count++;
        }
    }

    //Remove a portion
    QHash<QString, ItemRep> rmvd;
    for(int i=0;i<search_list.size();i++){
        if((i % 7) == 1){
            QString testKeys = search_list[i].getName();
            qDebug() << "keys removed: " << testKeys;
            CatItem it(testKeys);
            ItemRep r(it);
            items_by_keys.remove(r);
            rmvd[testKeys] = r;
        }
    }

    for(int i=0;i<search_list.size();i++){
        QString testKeys = search_list[i].getName();
        if(!rmvd.contains(testKeys)){
            ItemFilter ifilter;
            QList<CatItem> res = items_by_keys.findProtected(testKeys,20,0,&ifilter);
            //ItemRep r(testKeys);
            Q_ASSERT(listContainsPath(res, testKeys));

            //We need to also test partial matches here...
            QString cons = consonants(testKeys);
            if(cons.length() > 3 && (search_list[i].getFullWeight() >75) ){
                res = items_by_keys.findProtected(cons,20,0,&ifilter);
                //ItemRep r2(testKeys);
                Q_ASSERT(listContainsPath(res, testKeys));
                //Q_ASSERT(res.contains(r2));
                cons_count++;
            }
        }
    }
}

void TheApplicationTester::testParentChildDBAdd(){


    QString  appName1= "Feral Fox";
    CatItem a(appName1);

    CatItem b("item1");
    CatItem c("cat1");
    c.addChild(a,"dummy" , BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD );

    Q_ASSERT(a.hasParent(c));
    Q_ASSERT(!a.hasParent(b));

    //b.addParent(c);
    b.addParent(c, "dummy", BaseChildRelation::DATA_CATEGORY_PARENT);

    Cat_Store item_store;
    item_store.addItem(b);
    item_store.addItem(a);

    CatItem b_restored = item_store.getItemByPath(b.getPath());
    item_store.restoreRelations(b_restored,2);


    Q_ASSERT(b_restored.getParents()[0] == c);
    CatItem c_dummy(b_restored.getParents()[0].getPath());
    CatItem c_restored = item_store.getItemByPath(c.getPath());
    item_store.restoreRelations(c_restored,2);

    c_dummy.merge(c_restored);

    Q_ASSERT(listContainsItem(c_restored.getChildren(), a));

// We're never restoring children of parents 'cause that could bring in thousands from type-parents
//    Q_ASSERT(!listContainsItem(b_restored.getParents()[0].getChildren(), a));
//    item_store.restoreRelations(b_restored,3);
//    Q_ASSERT(listContainsItem(b_restored.getParents()[0].getChildren(), a));

}

void TheApplicationTester::testPeopleFilterCombine(){
    Cat_Store item_store(false);

    QString fullEmailAddress = "foo@bar.com";
    QString senderName="foo bar";
    QString namePath = senderName+ "/" + stringHash(fullEmailAddress);
    CatItem contactP(PERSON_PREFIX + namePath, senderName);
    contactP.setItemType(CatItem::PERSON);

    CatItem emailP(SYNONYM_PREFIX + namePath+ "/" + fullEmailAddress,fullEmailAddress);
    emailP.setItemType(CatItem::PERSON);
    emailP.setIsIntermediateSource();
    emailP.addParent(contactP,FULL_TEXT_PLUGIN_NAME,BaseChildRelation::SYNONYM);
    CatItem res("test");

    res.setAuthor(senderName);
    res.setEmailAddr(fullEmailAddress);
    res.addParent(contactP);

    item_store.addItem(res);




}

void TheApplicationTester::test_cat_store(){

    Cat_Store item_store(false);

    QList<CatItem> search_list;
    QList<CatItem> c1_list;
    QList<CatItem> c2_list;
    //search_vect.resize (ITEM_LIMIT);
    //itK.resize(ITEM_LIMIT);
    int count1 = 0;

    CatItem prev_item("base_parent");
    const int loop_var = 40; //change from 40 for full testing...
    //loop_var*loop_var items...
    for(int i=0;i<loop_var;i++){
        for(int j=0;j<loop_var;j++){
            //should give swaths of similar names but with gaps
            QString ns = numAsStr(i*2375400 + j*7+1000);
            CatItem ts(ns);
            QString c1_ns = QString("Child1_") + ns;
            CatItem ch1(c1_ns);
            QString c2_ns = QString("Child2_") + ns;
            CatItem ch2(c2_ns);
            ts.addChild(ch1);
            ts.addChild(ch2);
            ts.addParent(prev_item);
            search_list.push_back(ts);
            c1_list.push_back(ch1);
            c2_list.push_back(ch2);
            qDebug() << "Item Id: " << ts.getItemId();
            item_store.addItem(ts);
            prev_item = CatItem(ns);

            CatItem retrieved = item_store.getItemByPath(ns);
            item_store.restoreRelations(retrieved,2);
            QList<CatItem> restoredChildren = retrieved.getChildren();
            Q_ASSERT(listContainsPath(restoredChildren, c1_ns));
            Q_ASSERT(listContainsPath(restoredChildren, c2_ns));
            count1++;
            //item_store.testStruct();
        }
    }
    item_store.testStruct();

    int reweigh_step = 49;

    for(int i=0;i<loop_var*loop_var;i+=reweigh_step){//
        InputList il;
        il.setItem(search_list[i]);
        qDebug() << "seting: " << search_list[i].getPath() << "excuted";
        item_store.setItemExecuted(&il);

        for(int j=0;j<3 && i+j < loop_var*loop_var;j++){
            item_store.addVisibilityEvent(search_list[i+j],
                                          UserEvent(UserEvent::SHOWN_EDITLINE));
        }
    }
    item_store.reweightItems();
    item_store.testStruct();

    InputList dummyList;

    for(int i=0;i<loop_var*loop_var - 1 ;i+=reweigh_step){
        item_store.reweightItem(search_list[i]);
        item_store.reweightItem(search_list[i+1]);
        CatItem res1 = item_store.getItemByPath(search_list[i].getPath());
        CatItem res2 = item_store.getItemByPath(search_list[i+1].getPath());
        Q_ASSERT(res1.getFullWeight() < res2.getFullWeight());
        QList<CatItem> childs = item_store.getItemChildren(&dummyList, res1);
        Q_ASSERT(childs.count() == 3 || i ==0);

    }

    for(int i=0;i<loop_var*loop_var - 1;i++){
        CatItem oi = search_list[i];
        CatItem ci = item_store.getItemByPath(oi.getPath());
        Q_ASSERT(oi.getPath() == ci.getPath());
        Q_ASSERT(oi.getUseDescription() == ci.getUseDescription());
        QString d1 = oi.getDescription();
        QString d2 = ci.getDescription();
        Q_ASSERT(d1 == d2);
        Q_ASSERT(ci.matches_exactly(oi));
    }


    for(int i=0;i<loop_var*loop_var - 1 ;i+=reweigh_step){
        CatItem res1 = item_store.getItemByPath(search_list[i].getPath());
        CatItem res2 = item_store.getItemByPath(search_list[i+1].getPath());
        Q_ASSERT(res1.getFullWeight() < res2.getFullWeight());
        QList<CatItem> childs = item_store.getItemChildren(&dummyList, res1);
        Q_ASSERT(childs.count() == 3 || i ==0);

    }

}


void TheApplicationTester::test_cat_store2(){

    Cat_Store item_store(false);

    QList<CatItem> search_list;
    QList<CatItem> c1_list;
    QList<CatItem> c2_list;
    //search_vect.resize (ITEM_LIMIT);
    //itK.resize(ITEM_LIMIT);
    int count1 = 0;

    CatItem prev_item("base_parent");
    const int loop_var = 40; //change from 40 for less or more testing...
    InputList il;
    //loop_var*loop_var items...
    for(int i=0;i<loop_var;i++){
        for(int j=0;j<loop_var;j++){
            //should give swaths of similar names but with gaps
            QString postFix = numAsStr(i*2375400 + j*7+1000); //QString("longPrefix") +
            QString ns = QString("longPrefix") + postFix;
            CatItem ts(ns);
            QString c1_ns = QString("Child1_") + ns;
            CatItem ch1(c1_ns);
            QString c2_ns = QString("Child2_") + ns;
            CatItem ch2(c2_ns);

            ts.addChild(ch1);
            ts.addChild(ch2);
            ts.addParent(prev_item);
            search_list.push_back(ts);
            c1_list.push_back(ch1);
            c2_list.push_back(ch2);
            //qDebug() << "Item Id: " << ts.getItemId();

            item_store.addItem(ts);
            prev_item = CatItem(ns);

            il.setUserKeys(postFix);
            CatItem retrieved;
            QList<CatItem> fnd= item_store.getItemsByKey(ns, &il, 20);
            int k;
            bool found = false;
            for(k=0; k < fnd.length();k++){
                if(fnd[k].getPath() == ts.getPath()){
                    retrieved = fnd[k];
                    retrieved.matches_exactly(fnd[k]);
                    found = true;
                    break;
                }
            }
            if((i!=26) && (j!=28)){
                Q_ASSERT(found);
            }
            //Q_ASSERT(k <20);
            item_store.restoreRelations(retrieved,2);
            QList<CatItem> restoredChildren = retrieved.getChildren();
            Q_ASSERT(listContainsPath(restoredChildren, c1_ns));
            Q_ASSERT(listContainsPath(restoredChildren, c2_ns));
            count1++;

        }
    }
    item_store.testStruct();
    int reweigh_step = 49;
    InputList dummyList;


    for(int i=0;i<loop_var*loop_var;i+=reweigh_step){//
        InputList il;
        il.setItem(search_list[i]);
        qDebug() << "seting: " << search_list[i].getPath() << "excuted";
        item_store.setItemExecuted(&il);

        for(int j=0;j<3 && i+j < loop_var*loop_var;j++){
            item_store.addVisibilityEvent(search_list[i+j],
                                          UserEvent(UserEvent::SHOWN_EDITLINE));
        }
        item_store.reweightItem(search_list[i]);
        item_store.reweightItem(search_list[i+1]);
        CatItem res1 = item_store.getItemByPath(search_list[i].getPath());
        CatItem res2 = item_store.getItemByPath(search_list[i+1].getPath());
        qint32 wres1 = res1.getFullWeight();
        wres1=wres1;
        qint32 wres2 = res2.getFullWeight();
        wres2 =wres2;
        Q_ASSERT( wres1 < wres2 );
        QList<CatItem> childs = item_store.getItemChildren(&dummyList, res1);
        Q_ASSERT(childs.count() == 3 || i ==0);
    }
    item_store.reweightItems();
//    item_store.testStruct();

//    for(int i=0;i<loop_var*loop_var - 1 ;i+=reweigh_step){
//        CatItem res1 = item_store.getItemByPath(search_list[i].getPath());
//        CatItem res2 = item_store.getItemByPath(search_list[i+1].getPath());
//        qint32 wres1 = res1.getFullWeight();
//        qint32 wres2 = res2.getFullWeight();
//        Q_ASSERT( wres1 < wres2 );
//        QList<CatItem> childs = item_store.getItemChildren(res1);
//        Q_ASSERT(childs.count() == 3 || i ==0);
//
//    }

    for(int i=0;i<loop_var*loop_var - 1;i++){
        CatItem oi = search_list[i];
        CatItem ci = item_store.getItemByPath(oi.getPath());
        Q_ASSERT(oi.getPath() == ci.getPath());
        Q_ASSERT(oi.getUseDescription() == ci.getUseDescription());
        Q_ASSERT(ci.matches_exactly(oi));
    }
    item_store.testStruct();


//    for(int i=0;i<loop_var*loop_var - 1 ;i+=reweigh_step){
//        CatItem res1 = item_store.getItemByPath(search_list[i].getPath());
//        CatItem res2 = item_store.getItemByPath(search_list[i+1].getPath());
//        Q_ASSERT(res1.getFullWeight() < res2.getFullWeight());
//        QList<CatItem> childs = item_store.getItemChildren(res1);
//        Q_ASSERT(childs.count() == 3 || i ==0);
//
//    }

}




void TheApplicationTester::test_MultiTree2(){
    MultiTree<TestStringContainer> index;
    index.clearAll();
    index.addIndex(ALL_INDEX);
    index.addIndex(ODD_INDEX);
    index.addIndex(EVEN_INDEX);
    index.addIndex(MOD10_INDEX);

    QString forSprintf;
    QString str;

    vector<TestStringContainer> search_vect;
    search_vect.resize (ITEM_LIMIT);
    vector<long> itK;
    itK.resize(ITEM_LIMIT);

    long max_size = index.size();
    cout << "max index size: " << max_size;

    for (int i=0; i<ITEM_LIMIT; i++) {
        QString sstr;
        itK[i] = i;
        sstr = sstr.sprintf("%s%d"," key value ", i);
        TestStringContainer ts(sstr);
        search_vect[i] = ts;
        TestStringContainer t = search_vect[i];
        index.addEntry(itK[i],t,ALL_INDEX);
//        index.add(itK[i/10],t,MOD10_INDEX);
        if( i % 2 == 0){ index.addEntry(itK[i],t,EVEN_INDEX); }
        if( i % 2 == 1){ index.addEntry(itK[i],t,ODD_INDEX); }
    }

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer r = index.find(itK[i],ALL_INDEX);
        Q_ASSERT( search_vect[i] == r);
        if( i % 2 == 0){
            r = index.find(itK[i],EVEN_INDEX);
            Q_ASSERT( search_vect[i] == r);
        }
        if( i % 2 == 1){
            r = index.find(itK[i],ODD_INDEX);
            Q_ASSERT( search_vect[i] ==r);
        }
    }

//    for (long i=0; i<ITEM_LIMIT; i++) {
//        itK[i] = i;
//    }

    //Revalue every seventh item
    for (long i=0; i<ITEM_LIMIT; i+=7) {
        long new_key = i*1000;
        //Old
        index.addEntry(Tuple(new_key), search_vect[i], ALL_INDEX);
        itK[i] = new_key;
    }

    //Remove every third item
    for (int i=0; i<ITEM_LIMIT; i+=3) {
        bool b = index.removeObject(search_vect[i]);
        b = b;
        Q_ASSERT(b);
    }

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer t = index.find(itK[i],ALL_INDEX);
//        qDebug() << "t: " << t.getName()
//                << "itK[i]: " << itK[i]
//                << "search_vect[i]" << search_vect[i].getName();
        if(i %3 == 0){
            Q_ASSERT(t.isEmpty());
        } else {
            if(i % 7 !=0)
                Q_ASSERT( t == search_vect[i] );
        }

        if( i % 2 == 0){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,EVEN_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,EVEN_INDEX)); }
        }
        if( i % 2 == 1){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,ODD_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,ODD_INDEX)); }
        }
    }

}

void TheApplicationTester::test_MultiTree1()
{
    //See seperate lbtree test project!
}

void TheApplicationTester::testCreateFacebookSources(){
    CustomFieldInfo info;
    InputList iList;
    QSettings s("script/custom_actions/facebook_linkItem.oneline",QSettings::IniFormat);
    CatItem it(&s, CatItem("foo"));
    iList.setItem(it);
    iList.addSlot();

    iList.setArgValue(OAUTHKEY_KEY,
        "152825198104910|f6dfd6202528a791fd083447.1-100002054609991|opKFzJDmYj33UVmExAmZKAwXWK8");
    iList.setArgValue(FACEBOOK_USERNAME_FIELD, "Hans Solbrig");

    QList<CatItem> outputList;
    iList.rescanSlot();
    Q_ASSERT(iList.shouldExecuteNow());
    QString errString;
    Q_ASSERT((iList.canRun(&errString)));
    int res= CatBuilder::launchObject(iList, outputList, &errString);
    res = res;
    Q_ASSERT(res);
    //I could also check for

    CatItem resultItem = outputList[0];

    QList<CatItem> childs = resultItem.getChildren();
    Q_ASSERT(listContainsName(childs,"Profile feed (Wall) of Hans Solbrig"));

}

void TheApplicationTester::testExtendFacebookSources(){
    QSettings s("/generated_items/News feed of Hans Solbrig262008500.oneline",QSettings::IniFormat);
    CatItem fbSourceItem(&s);
    QSettings t("generated_items/Facebook Account of Hans Solbrig265940023.oneline",QSettings::IniFormat);
    CatItem fbParentItem(&t);
    fbSourceItem.addParent(fbParentItem);
    CatBuilder::updateItem(fbSourceItem,2);

    QList<CatItem> results;
    QList<CatItem> lst;
    lst.append(fbSourceItem);

    SearchInfo inf;
    inf.m_userKeys = "";
    inf.m_extensionType = UserEvent::STANDARD_UPDATE;
    inf.itemListPtr = &lst;
    inf.lastUpdate = gSettings->value("GenOps/lastUpdateTime", 0).toLongLong();
    CatBuilder::getPluginHandle()->doTestCatalogueExtension("libstreamplugin.so", &inf, &results);
    Q_ASSERT(results.count()>0);
    QList<CatItem> childs = results[0].getChildren();
    Q_ASSERT(childs.count() > 4);

}



void TheApplicationTester::testCreateCustomArg(){
    QFileInfo fileInfo("custom_actions/Send_Email.oneline");
    Q_ASSERT(fileInfo.exists());
    QSettings s(fileInfo.absoluteFilePath(), QSettings::IniFormat);

    CatItem custVerb(&s, CatItem("foo"));
    QString desStr = custVerb.getVerbDescription();
    qDebug() << desStr;
    Q_ASSERT( desStr.simplified()
              == QString(" send email [Title:$title:shorttext-] $body:longtext#20+ "
                         "to: $recepient:emailaddress+ [$cc:longtext]").simplified());

    Q_ASSERT(custVerb.getName()=="email");
    Q_ASSERT(custVerb.getDescription()=="Send Email");
    Q_ASSERT(custVerb.getActionType() == CatItem::VERB_ACTION);

    QList<CatItem> args = custVerb.getCustomArguments();
    Q_ASSERT(args.length()>3);

    CatItem arg1 = args[0];
    Q_ASSERT(arg1.getArgNumber()==0);
    Q_ASSERT(arg1.getName()=="title");
    QString argStr = arg1.getArgType();
    Q_ASSERT(argStr == SHORT_TEXT_NAME);
    //Q_ASSERT(arg1.argCommandString("test text") == "test text");

    CatItem arg2 = args[1];
    int argNum = arg2.getArgNumber();
    Q_ASSERT(argNum ==1);
    argStr = arg2.getName();
    Q_ASSERT(argStr=="body");
    argStr = arg2.getArgType();
    Q_ASSERT(argStr == LONG_TEXT_NAME);
    //Q_ASSERT(arg1.argCommandString("test text") == "test text");

    arg2 = args[2];
    argNum = arg2.getArgNumber();
    Q_ASSERT(argNum==2);
    Q_ASSERT(arg2.getName()=="recepient");
    argStr = arg2.getArgType();
    Q_ASSERT(arg2.getArgType()== EMAIL_ADDRESS_NAME);
    //QString t = arg2.argCommandString("test@test.com");
    //Q_ASSERT(t == "to: test@test.com");

    CatItem arg3 = args[3];
    Q_ASSERT(arg3.getArgNumber()==3);
    argStr = arg3.getArgType();
    Q_ASSERT(arg3.getArgType()== LONG_TEXT_NAME);
    //Q_ASSERT(arg3.argCommandString("test text") == "title: test text");


    InputList il;
    il.setUserKeys("This is a message");
    il.addSlot();
    Q_ASSERT(!il.isLabeledFromEnd());
    Q_ASSERT(!il.isLabeledFromStart());
    Q_ASSERT(il.slotCount()>1);
    //ItemRep verbRep(custVerb);
    Q_ASSERT(il.acceptItem(&custVerb));

    InputList il2;
    il2.setUserKeys("This is a long message body with plenty of text");
    il2.addSlot();
    il2.setUserKeys("donald@batcave.com");


    CatBuilder::getCatalog()->addItem(custVerb);
    QList< QList < CatItem > > types = il2.getCustomVerbSignature();
    QList<CatItem> bTypeStrings;
    for(int i=0; i < types.count();i++){
        bTypeStrings.append(types[i][0]);
    }
    Q_ASSERT(listContainsPath(bTypeStrings,LONG_TEXT_PATH));
    QString rt = LONG_TEXT_PATH;
    QList<CatItem> rawVerbs = CatBuilder::getCatalog()->cat_store.getCustomVerbByActionType(CatItem(rt));
    Q_ASSERT(listContainsPath(rawVerbs,custVerb.getPath()));
//    bool matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.matchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(matched);
//
//    matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.completeMatchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(matched);

    QList<CatItem> sig;
    sig.append(CatItem(rt));
    sig.append(CatItem::createActionItem(EMAIL_ADDRESS_NAME));
//    Q_ASSERT(custVerb.completeMatchArgSig(sig));


    //Q_ASSERT(il2.acceptItem(&verbRep));

    il.setItem(custVerb);
    types = il.getCustomVerbSignature();
    //Q_ASSERT(types.length()=2);
    il.setUserKeys("hans@google.com");


}

void TheApplicationTester::testCreateCustomArg3(){

    CatItem custVerb("Foo");
    QString d = QString(" Add Facebook Account [$oathkey:FacebookId:StreamPlugin+] [$user_name++]");
    custVerb.setCustomPluginInfo(CUSTOM_VERB_DISCRIPTION_KEY_STR,d);
    custVerb.setCustomPluginValue(EXECUTE_IF_FULLFILLED_KEY,(int)true);
    custVerb.setItemType(CatItem::VERB);

    QList<CatItem> args = custVerb.getCustomArguments();
    Q_ASSERT(args.length()>0);

    InputList il;

    il.setItem(custVerb);
    Q_ASSERT(!il.customVerbChosen());
    il.setCustomVerbChosen();
    Q_ASSERT(!il.shouldExecuteNow());

    //il.addField();
    Q_ASSERT(il.slotCount()==2);
    il.setArgValue(OAUTHKEY_KEY, "123XYZ");
    Q_ASSERT(il.slotCount()==2);
    //duplicate shouldn't change anything
    il.setArgValue(OAUTHKEY_KEY, "123XYZ");
    Q_ASSERT(il.slotCount()==2);
    QString userName("joe solbrig");
    il.setArgValue("user_name", userName);
    Q_ASSERT(il.slotCount()==3);
    Q_ASSERT(il.slotPosition()==1);

    QHash<QString, EntryField> iH = il.asFieldValueHash();
    Q_ASSERT(iH["user_name"].value== userName);
    Q_ASSERT(iH[OAUTHKEY_KEY].value== "123XYZ");

    Q_ASSERT(il.shouldExecuteNow());

}


void TheApplicationTester::testCreateCustomArg2(){
    QFileInfo fileInfo("custom_actions/Send_Email.oneline");
    Q_ASSERT(fileInfo.exists());
    QSettings s(fileInfo.absoluteFilePath(), QSettings::IniFormat);

    CatItem custVerb(&s, CatItem("foo"));
    QString desStr = custVerb.getVerbDescription();
    qDebug() << desStr;

    Q_ASSERT(custVerb.getName()=="email");
    Q_ASSERT(custVerb.getDescription()=="Send Email");
    Q_ASSERT(custVerb.getActionType() == CatItem::VERB_ACTION);
    Q_ASSERT(custVerb.getUseDescription());

    VerbItem vi(custVerb);
    vi.setField("title", "my post" );
    vi.setField("recepient", "donald@batcave.com" );
    QString str = vi.getFormattedDescription("",false,0,0,false,false);
    Q_ASSERT(str=="send email Title:my post to: donald@batcave.com");
    QString l_str(
        "In a bowl or on a clean work surface combine the flour, "
        "baking powder and salt.  "
        "Make a well in the center of the flour and pour the warm water "
        "into the center.  With your hands or a wooden spoon, start working "
        "the mixture together.  "
        "Sprinkle flour on a clean work surface and turn the dough out and knead lightly." );
    //QString shortened = l_str.left(20);
    vi.setField("body", l_str );


    QList<CatItem> args = custVerb.getCustomArguments();
    Q_ASSERT(args.length()>3);

    CatItem arg1 = args[0];
    Q_ASSERT(arg1.getArgNumber()==0);
    Q_ASSERT(arg1.getName()=="title");
    QString argStr = arg1.getArgType();
    Q_ASSERT(arg1.getUseDescription());
    Q_ASSERT(arg1.getDescription() =="Enter title");
    Q_ASSERT(argStr == SHORT_TEXT_NAME);
    //Q_ASSERT(arg1.argCommandString("test text") == "test text");

    CatItem arg2 = args[1];
    int argNum = arg2.getArgNumber();
    Q_ASSERT(argNum ==1);
    argStr = arg2.getName();
    Q_ASSERT(argStr=="body");
    argStr = arg2.getArgType();
    Q_ASSERT(argStr == LONG_TEXT_NAME);
    //Q_ASSERT(arg1.argCommandString("test text") == "test text");

    arg2 = args[2];
    argNum = arg2.getArgNumber();
    Q_ASSERT(argNum==2);
    Q_ASSERT(arg2.getName()=="recepient");
    argStr = arg2.getArgType();
    Q_ASSERT(arg2.getArgType()== EMAIL_ADDRESS_NAME);
    //QString t = arg2.argCommandString("test@test.com");
    //Q_ASSERT(t == "to: test@test.com");

    CatItem arg3 = args[3];
    Q_ASSERT(arg3.getArgNumber()==3);
    argStr = arg3.getArgType();
    Q_ASSERT(arg3.getArgType()== LONG_TEXT_NAME);
    //Q_ASSERT(arg3.argCommandString("test text") == "title: test text");

    InputList il;
    il.setUserKeys("This is a message");
    il.addSlot();
    Q_ASSERT(!il.isLabeledFromEnd());
    Q_ASSERT(!il.isLabeledFromStart());
    Q_ASSERT(il.slotCount()>1);
    //ItemRep verbRep(custVerb);
    Q_ASSERT(il.acceptItem(&custVerb));

    InputList il2;
    il2.setUserKeys("This is a long message body with plenty of text");
    il2.addSlot();
    il2.setUserKeys("donald@batcave.com");


    CatBuilder::getCatalog()->addItem(custVerb);
    QList< QList < CatItem > > types = il2.getCustomVerbSignature();
    QList<CatItem> bTypeStrings;
    for(int i=0; i < types.count();i++){
        bTypeStrings.append(types[i][0]);

    }
    Q_ASSERT(listContainsPath(bTypeStrings,LONG_TEXT_PATH));
    QString rt = LONG_TEXT_PATH;
    QList<CatItem> rawVerbs = CatBuilder::getCatalog()->cat_store.getCustomVerbByActionType(CatItem(rt));
    Q_ASSERT(listContainsPath(rawVerbs,custVerb.getPath()));
//    bool matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.matchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(matched);

//    matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.completeMatchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(!matched);

    QList<CatItem> sig;
    sig.append(CatItem::createActionItem(LONG_TEXT_PATH));
    //Q_ASSERT(custVerb.matchArgSig(sig));


    //Q_ASSERT(il2.acceptItem(&verbRep));

    il.setItem(custVerb);
    types = il.getCustomVerbSignature();
    //Q_ASSERT(types.length()=2);
    il.setUserKeys("hans@google.com");

}





QList<CatItem> TheApplicationTester::backgroundFind(QString userKeys, QStringList keywords ){
    QList<CatItem> insertList;
    QList<CatItem> dummyList;
    SearchInfo inf;
    inf.m_userKeys = userKeys;
    inf.m_keyWords = keywords;
    inf.itemListPtr = &(dummyList);
    CatBuilder::getPluginHandle()->backgroundSearch(&inf, &insertList);
    return insertList;
}

void TheApplicationTester::testFileFind(){

    QString filePath1 = "./test1.txt";
    QString filePath2 = "./test2.txt";
    QString filePath3 = "./test3.txt";
    QString filePath4 = "./test4.txt";

    QDir baseDir(QDir::current() );
    filePath1 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath1));
    filePath2 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath2));
    filePath3 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath3));
    filePath4 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath4));

    QFile file1(filePath1);
    if(!file1.open(QIODevice::WriteOnly | QIODevice::Text)) { Q_ASSERT(FALSE); }
    qint32 c = file1.write(QByteArray("A bbaattcch ffouou bbaarr"));
    file1.close();
    Q_ASSERT(c >0);


    QFile file2(filePath2);
    if(!file2.open(QIODevice::WriteOnly| QIODevice::Text)) { Q_ASSERT(FALSE); }
    c = file2.write(QByteArray("A bbaattcch ffouou "));
    Q_ASSERT(c >0);
    file2.close();

    QFile file3(filePath3);
    if (!file3.open(QIODevice::WriteOnly| QIODevice::Text)) { Q_ASSERT(FALSE); }
    c = file3.write(QByteArray("A bbaattcch bbaarr"));
    Q_ASSERT(c >0);
    file3.close();

    QFile file4(filePath4);
    if(!file4.open(QIODevice::WriteOnly| QIODevice::Text)) { Q_ASSERT(FALSE); }
    c = file4.write(QByteArray("tastfr asdf ihoo"));
    Q_ASSERT(c >0);
    file4.close();

    CatItem i1(filePath1);
    CatItem i2(filePath2);
    CatItem i3(filePath3);
    CatItem i4(filePath4);
    QList<CatItem> li;
    li.append(i1);
    li.append(i2);
    li.append(i3);
    li.append(i4);
    if(!the_test_builder)
        { the_test_builder = new CatBuilder(CatBuilder::REFRESH, *gDirs); }
    the_test_builder->m_userItems = li;
    QList<CatItem> ol;
    the_test_builder->doExtension(&ol);

    QString q("bbaattcch ffouou");
//    InputList il;
//    il.setUserKeys(q);
    ol.clear();
    ol = backgroundFind(q, q.split(" "));
    //CatBuilder::getCatalog()->getItemsFromQuery(il,ol,30);

    Q_ASSERT(listContainsPath(ol,filePath1));
    Q_ASSERT(listContainsPath(ol,filePath2));
    Q_ASSERT(listContainsPath(ol,filePath3));
    Q_ASSERT(!listContainsPath(ol,filePath4));
    Q_ASSERT(ol.count()>2);

    q = ("bbaarr "); //the space character matters...
//    ol.clear();
//    CatBuilder::getCatalog()->getItemsFromQuery(il,ol,20);
    ol = backgroundFind(q, q.split(" "));

    Q_ASSERT(listContainsPath(ol,filePath1));
    Q_ASSERT(!listContainsPath(ol,filePath2));
    Q_ASSERT(listContainsPath(ol,filePath3));
    Q_ASSERT(!listContainsPath(ol,filePath4));
    Q_ASSERT(ol.count()>1);

//    QIcon icon = gPlatform->icon(QDir::toNativeSeparators(i1.getPath()));
//    Q_ASSERT(!icon.isNull());

    gPlatform->alterItem(&i1);

    QList<CatItem> parents = i1.getParents();
    qDebug() << "mime and other parents";
    for(int i=0;i< parents.count();i++){
        qDebug()<< parents[i].getPath();
        qDebug()<< parents[i].getDescription();
    }
    Q_ASSERT(listContainsPath(parents,"action_type://text/plain"));



}


void TheApplicationTester::testTest() {

     QString str = "Hello";
     Q_ASSERT(str == "Hello");

}

void TheApplicationTester::testFilterRoutines(){
//    CatBuilder::destroyCatalog();
//    CatBuilder::getOrCreateCatalog(*gDirs);

    QString base ="BCB";

    //Simple match
    QString itemStr = "BCBaLe";

    {CatItem it(itemStr,itemStr);
        Q_ASSERT(filterItemByKeys(it,base));

        CatBuilder::getCatalog()->addItem(it);
        InputList il;
        il.setItem(base);
        il.setUserKeys(base);
        QList<CatItem> ol;
        CatBuilder::getCatalog()->getItemsFromQuery(il,ol,10);
        CatItem ir = ol[0];
        qDebug() << "ir.getPath() " << ir.getPath();

        il.setItem(ir);
        //QString formatted = il.asFormattedString();
        QString formatted = il.mainItemText();

        qDebug() << "formatted: " << formatted;
        QString expected = (
                "<font color=\"yellow\">BCB</font>aLe");
        Q_ASSERT(formatted.simplified() == expected.simplified());
    }

    //Multi-sylabul
    itemStr = "BCaBaLe";
    {
        CatItem it(itemStr, itemStr);
        Q_ASSERT(filterItemByKeys(it,base));
        InputList il;
        CatBuilder::getCatalog()->addItem(it);
        CatItem ir(it);
        il.setItem(ir);
        QString formated = il.asFormattedString();
        QString expected = (
                "<font color=\"yellow\">BC</font>a<font color=\"yellow\">B</font>aLe"

                );
        Q_ASSERT(formated.simplified() == expected.simplified());
    }

    //Far Non-match
    itemStr = "XXXX";
    {CatItem it(itemStr, itemStr);
        Q_ASSERT(!filterItemByKeys(it,base));}

    //Near Non-match
    itemStr = "BXaBaLe";
    {CatItem it(itemStr, itemStr);
        Q_ASSERT(!filterItemByKeys(it,base));}

    //More Multi-sylabul
    itemStr = "BaCaBaLe";
    {CatItem it(itemStr, itemStr);
        Q_ASSERT(filterItemByKeys(it,base));
        InputList il;
        CatBuilder::getCatalog()->addItem(it);
        CatItem ir(it);
        il.setItem(ir);
        QString formated = il.asFormattedString();
        Q_ASSERT(formated ==
            (
            "<font color=\"yellow\">B</font>a<font color=\"yellow\">"
            "C</font>a<font color=\"yellow\">B</font>aLe"));
    }

    //Multi-sylabul skipping vowels
    base ="BeCeB";
    itemStr = "BaCaBaLe";
    {CatItem it(itemStr);
        Q_ASSERT(filterItemByKeys(it,base));}

    //Verify matches does NOT work for close items...
    base =unfoundStr;
    itemStr = str_list[0][1];
    {CatItem it(itemStr);
        Q_ASSERT(!matches( &it, base));}

    for(int i =0;i<str_len;i++){
        QString in_str(str_list[i][0]);
        QString out_str(str_list[i][1]);
        {CatItem it(out_str);
            Q_ASSERT(filterItemByKeys(it, in_str));}
    }

}

//Low total weight == higher
void TheApplicationTester::testComparisonRoutines(){
    Q_ASSERT(CatBuilder::getCatalog());
    QString base;

    //subQString versus non match
    Catalog::curFilterStr = "BC";
    QString itemStr = "XXXX";
    base ="BCB";
    {CatItem it(itemStr);
    CatItem it2(base);
    it.setTotalWeight(10);
    it2.setTotalWeight(5); //Lower lexical weight - lower order
    it.setMatchType(CatItem::USER_KEYS);
    it2.setMatchType(CatItem::USER_KEYS);
        Q_ASSERT(CatLessNoPtr(it2,it));}

    //Multi-sylabul versus non
    Catalog::curFilterStr = "BCB";
    itemStr = "BCaBaLe";
    base ="XYZ";
    {CatItem it2(itemStr);
    QList<int> il;
    Q_ASSERT(consonant_match(it2, Catalog::curFilterStr, &il));
    CatItem it(base);
    it2.setTotalWeight(20);
    it.setTotalWeight(10);
    it.setMatchType(CatItem::USER_KEYS);
    it2.setMatchType(CatItem::USER_KEYS);
        Q_ASSERT(CatLessNoPtr(it2,it));}

    //Exact versus multi-partial
    itemStr = "BCBaLe";
    Catalog::curFilterStr = "BCB";
    base = "BCB";
    {CatItem it(itemStr);
    QList<int> il;
    Q_ASSERT(consonant_match(it, Catalog::curFilterStr, &il));
    CatItem it2(base);
    it.setMatchType(CatItem::USER_KEYS);
    it2.setMatchType(CatItem::USER_KEYS);
        Q_ASSERT(CatLessNoPtr(it2,it));}

    //Exact versus multi-partial
    Catalog::curFilterStr = "BCB";
    itemStr = "BCBaLe";
    base = "xxxBCB";
    {CatItem it(itemStr);
    QList<int> il;
    Q_ASSERT(consonant_match(it, Catalog::curFilterStr, &il));
    CatItem it2(base);
    it.setMatchType(CatItem::USER_KEYS);
    it2.setMatchType(CatItem::USER_KEYS);
        Q_ASSERT(CatLessNoPtr(it, it2));}

    //Weighting...
    itemStr = "BCBaLe";
    Catalog::curFilterStr = "XYZ";
    base = "BCB";
    {CatItem it(itemStr);
    CatItem it2(base);
    it.setTotalWeight(20);
    it2.setTotalWeight(10);
    it.setMatchType(CatItem::USER_KEYS);
    it2.setMatchType(CatItem::USER_KEYS);
        Q_ASSERT(CatLessNoPtr(it2,it));}


    for(int i =0;i<str_len;i++){
        QString in_str(str_list[i][0]);
        QString out_str(str_list[i][1]);
        Catalog::curFilterStr = in_str;
        QString other = "heavy" + in_str; //note: y
        {CatItem it(out_str);
        CatItem it2(other);
        CatItem src("testSrc");
        it2.addParent(src);
        it2.setExternalWeight(1000, src);
        it.setMatchType(CatItem::USER_KEYS);
        it2.setMatchType(CatItem::USER_KEYS);
            Q_ASSERT(CatLessNoPtr(it,it2));}
    }

    const char* sim[] = { "Antarctica Experiment Discovers Puzzling Space Ray Pattern",
        "xslt_source://~/recently-used",
                   "x-nautilus-desktop",
                   "xslt_source://test.xml::",
                   "xyzItem"};

    QList<CatItem> simL;
    for(int i =0;i<5;i++){
        QString in_str(sim[i]);
        CatItem s(in_str);
        s.setMatchType(CatItem::USER_KEYS);
        simL.append(s);
    }
    Catalog::curFilterStr = "xyz";
    qSort(simL.begin(), simL.end(), CatLessNoPtr);
    Q_ASSERT(QString("xyzItem") == simL[0].getName());

}


void TheApplicationTester::testSortRoutines(){
    //Don't delete factory created routine...
    Q_ASSERT( CatBuilder::getCatalog() );
    QList<CatItem> insertList;
    CatBuilder::getCatalog()->plugins.getCatalogs(&insertList);
    for(int i =0;i<insertList.length();i++){
        insertList[i].setMatchType(CatItem::USER_KEYS);
    }

    for(int i =0;i<str_len;i++){
        Catalog::curFilterStr = str_list[i][0];
        qSort(insertList.begin(), insertList.end(), CatLessNoPtr);
        QString outVal = insertList.first().getName();
        Q_ASSERT(outVal ==  str_list[i][1]);
    }
}

void TheApplicationTester::verifyGotResultFromKeys(QString keys, QString result){
    QList<CatItem> testResult;
    InputList ip;
    ip.setUserKeys(keys);
    CatBuilder::getCatalog()->getItemsFromQuery(ip, testResult, 20);
    Q_ASSERT(testResult.size() > 0);
    CatItem bestItemRep = testResult[0];
    CatItem bestItem = bestItemRep;
    Q_ASSERT(matches( &bestItem, ip.getUserKeys()));
    QString returnedItem = bestItemRep.getName();
    Q_ASSERT(returnedItem == result);
    result = result;
    //Don't delete factory created routine...
}

void TheApplicationTester::verifyChildItem(QString keys, QString result){
    InputList ip;
    ip.setUserKeys(keys);
    QList<CatItem> testResults;
    CatBuilder::getCatalog()->getItemsFromQuery( ip, testResults, 20);
    Q_ASSERT(testResults.size() > 0);
    bool foundChild = false;
    for(int i=0; i < testResults.size();i++){
        CatItem bestItemRep = testResults[i];
        QList<CatItem> children = bestItemRep.getChildren();
        foundChild = foundChild  || listContainsPath(children,result);
    }
    Q_ASSERT(foundChild);
}

void TheApplicationTester::testFilePlugin(){
    if(the_test_builder==0){
        CatBuilder::destroyCatalog();
        the_test_builder = new CatBuilder(CatBuilder::REFRESH, *gDirs, true);
    }
    the_test_builder->catalogTask(CatBuilder::REFRESH);
    // Duplicates the mode argument above 'cause we're calling a otherwise-private function
    // to avoid starting a new thread
    bool s = verifyCatHasItemName(QString("testFilePluginFile.txt"));
    Q_ASSERT(s);
    s = verifyCatHasItemName(QString("eyixm_gibberish"));
    Q_ASSERT(!s);


}

bool TheApplicationTester::verifyCatHasItemName(QString name){
    bool found = false;
    InputList il;
    il.setUserKeys(name);

    QList<CatItem> results;
    CatBuilder::getCatalog()->getItemsFromQuery(il,results, 50);
    for(int i=0;i<results.count();i++){
        qDebug() <<  results[i].getName();
        if(results[i].getName() == name){ found = true; }
    }
    return found;
}

void TheApplicationTester::testFireFoxPlugin(){
    QList<CatItem> insertList;
    CatBuilder::getCatalog()->plugins.getCatalogs(&insertList);
    int insertCount = insertList.count();
    insertCount = insertCount;
    Q_ASSERT(10 < insertCount );
    CatBuilder::getCatalog()->indexItemList(&insertList);
    //QList<CatItem> forSave = CatBuilder::getCatalog()->getItemsForSave();
    //Q_ASSERT( forSave .count()>= insertCount );

    for(uint i=0; i < str_len;i++){
        QString in_str(str_list[i][0]);
        QString out_str(str_list[i][1]);
        verifyGotResultFromKeys(in_str,out_str);
    }
    //This test uses existing catalog...
    //testFilePlugin();
    //Mozilla plugin - obvious not portable
    //Just choose a common site in your list
    bool r = verifyCatHasItemName(QString("craigslist.org"));
    r = r;
    Q_ASSERT(r);
    verifyChildItem("customTestItem", "testChildItem");
}

void TheApplicationTester::clearEntry(){

    for(int i=0; i<5;i++){
        QTest::keyClick(lWind, Qt::Key_Escape);
    }
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(QString()));
    Q_ASSERT(lWind->m_inputList.slotCount()==1);
    lWind->showApp();

}


void TheApplicationTester::subTestTextEntry(){
    clearEntry();

    sendKeysAndWait(unfoundStr);
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    //QString requiredFormattedStr = "<html><body>XYZunfound</body></html>";
    Q_ASSERT(titlize(unfoundStr)==resStr);
    QTest::keyClick(lWind, Qt::Key_Backspace);
    QString str = unfoundStr;
    str.chop(1);
    resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(titlize(str)==resStr);

    QTest::keyClick(lWind, Qt::Key_Escape);
    resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(QString("")));

    Q_ASSERT(CatBuilder::getCatalog(
            )->cat_store.getItemsByKey(
                    str_list[0][0], &dummyInputList,10).count() >0);

    for(uint i=0; i < str_len;i++){
        QString in_str(str_list[i][0]);
        QString out_str(str_list[i][1]);
        QString out_format_str(str_list[i][2]);

        sendKeysAndWait(in_str);
        resStr = lWind->m_inputDisplay->getMainHtmlText();
        qDebug() << "restStr at test:" << resStr;
        //Sometimes the edit adds full html formatting... but either is correct
        Q_ASSERT(resStr==titlize( out_format_str ) || resStr==( out_format_str ));
        QTest::keyClick(lWind, Qt::Key_Escape);
        resStr = lWind->m_inputDisplay->getMainHtmlText();
        Q_ASSERT(resStr==titlize(QString()));

        Q_ASSERT(CatBuilder::getCatalog(
                )->cat_store.getItemsByKey(
                        str_list[0][0], &dummyInputList,10).count() >0);
    }

}


bool TheApplicationTester::verifyBoxContainsName(QString name){
    for(int i=0; i<lWind->m_itemChoiceList->count();i++){
        QListWidgetItem* lItem = lWind->m_itemChoiceList->getItem(i);
        if(lItem){
            if(name == (lItem->data(ROLE_SHORT).toString())){
                return true;
            }
        }
    }
    return false;
}

//Need to test also

//- Select text file
//- You get at least one verb as an option
//- You get tag as an option

void TheApplicationTester::subTestTabFromItem(){

    QTest::keyClick(lWind, Qt::Key_Escape);
    QTest::keyClick(lWind, Qt::Key_Escape);
    QTest::keyClick(lWind, Qt::Key_Escape);

    sendKeysAndWait(unfoundStr);
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(unfoundStr));
    QTest::keyClick(lWind, Qt::Key_Tab);
    QString tabStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(tabStr == titlize("") );
//    QString tabOut = lWind->m_inputDisplay->getExplanationText();
//    qDebug() << "tabout: " << tabOut;
//    Q_ASSERT(tabOut == unfoundStr+UI_SEPARATOR_STRING);
    QTest::keyClick(lWind, Qt::Key_Escape);
    tabStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(tabStr == titlize(unfoundStr));
    QTest::keyClick(lWind, Qt::Key_Escape);
    Q_ASSERT(lWind->m_inputDisplay->getMainHtmlText() == titlize(""));

}

void TheApplicationTester::subTestDoAction(){
    clearEntry();
    sendKeysAndWait(str_list[0][0]);
    CatItem it = lWind->m_inputList.currentItem();
    int oldWeight = it.getFullWeight();
    oldWeight = oldWeight;
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(str_list[0][2]) || resStr==(str_list[0][2]));
    //Q_ASSERT(resStr==it.getName());
    QTest::keyClick(lWind, Qt::Key_Return);

    CatItem it2 = CatBuilder::getCatalog()->getItem(it.getPath());
    int newWeight = it2.getFullWeight();
    newWeight = newWeight;
    Q_ASSERT(newWeight  < oldWeight );
}

void TheApplicationTester::subTestUpdateUsage(){

//DUPLICATE - see stand-alone!!


//    int old_time_offset = gTime_offset_for_testing;
//
//
//    CatItem src("externalSource1");
//    src.setSourceWeight(MEDIUM_EXTERNAL_WEIGHT*10);
//    src.setTagLevel(CatItem::INTERNAL_SOURCE);
//
//
//    CatItem e1("externalItem1");
//    e1.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,src);
//    CatItem e2("externalItem2");
//    e2.setExternalWeight(MINIMUM_EXTERNAL_WEIGHT,src);
//    CatBuilder::getCatalog()->addItem(src);
//    CatBuilder::getCatalog()->addItem(e2);
//    CatBuilder::getCatalog()->addItem(e1);
//
//
//    //Reweigh items
//    for(int i=0;i < 24;i++){
//        CatBuilder::getCatalog()->reweightItems();
//    }
//    CatBuilder::getCatalog()->reweightItems();
//
//    CatItem eri1 = CatBuilder::getCatalog()->getItem("externalItem1");
//    CatItem eri2 = CatBuilder::getCatalog()->getItem("externalItem2");
//    long ew1 = eri1.getFullWeight();
//    long ew2 = eri2.getFullWeight();
//
//    Q_ASSERT(ew1 < ew2); //Full weight corresponds to
//
//
//    //Take three items
//    CatItem item1("two_w_old");
//    CatItem item2("one_w_old");
//    CatItem item3("now_old");
//    CatBuilder::getCatalog()->addItem(item1);
//    CatBuilder::getCatalog()->addItem(item2);
//    CatBuilder::getCatalog()->addItem(item3);
//    lWind->inputList.setItem(item1);
//    //One gets 3 calls "now"
//    gTime_offset_for_testing = 0;
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//
//    //One gets 2 calls one weeks ago
//    gTime_offset_for_testing = 2*7*24*60*60;
//    lWind->inputList.setItem(item2);
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//    //Reweigh items
//    for(int i=0;i < 24;i++){
//        CatBuilder::getCatalog()->reweightItems();
//    }
//
//    //one gets 2 calls one weeks ago
//    //one was just called
//    gTime_offset_for_testing = 4*7*24*60*60;
//    lWind->inputList.setItem(item3);
//    CatBuilder::getCatalog()->setExecuted(lWind->inputList);
//
//    //Reweigh items
//    for(int i=0;i < 24;i++){
//        CatBuilder::getCatalog()->reweightItems();
//    }
//    //get entries from catalog
//    CatItem ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
//    CatItem ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
//    CatItem ri3 = CatBuilder::getCatalog()->getItem("now_old");
//    long w1 = ri1.getFullWeight();
//    long w2 = ri2.getFullWeight();
//    long w3 = ri3.getFullWeight();
//
//    Q_ASSERT(w1 > w2);
//    Q_ASSERT(w2 > w3);
//
//    //Check they begin in time order
//
//    //advance the clock a month
//    gTime_offset_for_testing = 6*7*24*60*60; //six weeks later in seconds
//
//    //Reweigh items
//    for(int i=0;i < 24;i++){
//        CatBuilder::getCatalog()->reweightItems();
//    }
//    //get entries from catalog
//    ri1 = CatBuilder::getCatalog()->getItem("two_w_old");
//    ri2 = CatBuilder::getCatalog()->getItem("one_w_old");
//    ri3 = CatBuilder::getCatalog()->getItem("now_old");
//    //Check they are now in frequency order
//    Q_ASSERT(ri1.getFullWeight() < ri2.getFullWeight());
//    Q_ASSERT(ri2.getFullWeight() < ri3.getFullWeight());
//
//    gTime_offset_for_testing = old_time_offset;

}

void TheApplicationTester::subTestTextFormatting(){
    //This is pretty much done elsewhere...

}



void TheApplicationTester::subTestTagOperation(){
    clearEntry();
    //Tag then verb then noun
    sendKeysAndWait(unfoundStr);
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(unfoundStr) || resStr==(unfoundStr));
    QTest::keyClick(lWind, Qt::Key_Tab);
    QString tabStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(tabStr == titlize("") || tabStr == (""));
    sendKeysAndWait(testVerbStr);
    Q_ASSERT(lWind->m_inputList.slotCount()==2);

    //
    Q_ASSERT(!lWind->m_inputList.getVerb().isEmpty());
    Q_ASSERT(!(lWind->m_inputList.getVerb().getPath() ==CUSTOM_VERB_PREFIX +"Send_Email.oneline"));

    QString msg;
    Q_ASSERT(lWind->m_inputList.canAddSlot(&msg));

    QTest::keyClick(lWind, Qt::Key_Tab);
    //test that a scan gets only nouns
    QList<CatItem> ol;
    CatBuilder::getCatalog()->getItemsFromQuery(lWind->m_inputList,ol,50);
    Q_ASSERT(ol.length() > 0);
    Q_ASSERT(!listHasActionType(ol, CatItem::VERB_ACTION));

//    //test that a scan gets only verbs
//    ol.clear();
//    CatBuilder::getCatalog()->getItemsFromQuery(lWind->m_inputList,ol,50);
//    Q_ASSERT(ol.length() > 0);
//    Q_ASSERT(listHasActionType(ol, CatItem::NOUN_ACTION));
//    Q_ASSERT(listHasActionType(ol, CatItem::OPERATION_ACTION));

    clearEntry();

    //Tag then noun then verb
    sendKeysAndWait(unfoundStr);
    resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize(unfoundStr) || resStr==(unfoundStr));
    QTest::keyClick(lWind, Qt::Key_Tab);
    tabStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(tabStr == titlize("") || tabStr == (""));
    sendKeysAndWait(testVerbStr);
    Q_ASSERT(lWind->m_inputList.slotCount()==2);

    //
    Q_ASSERT(!lWind->m_inputList.getVerb().isEmpty());
    Q_ASSERT(!(lWind->m_inputList.getVerb().getPath() ==CUSTOM_VERB_PREFIX +"Send_Email.oneline"));

    Q_ASSERT(lWind->m_inputList.canAddSlot(&msg));

    QTest::keyClick(lWind, Qt::Key_Tab);
    //test that a scan gets only nouns
    ol.clear();
    CatBuilder::getCatalog()->getItemsFromQuery(lWind->m_inputList,ol,50);
    Q_ASSERT(ol.length() > 0);
    Q_ASSERT(~listHasActionType(ol, CatItem::VERB_ACTION));

    //test that a scan gets only verbs
    ol.clear();
    CatBuilder::getCatalog()->getItemsFromQuery(lWind->m_inputList,ol,50);
    Q_ASSERT(ol.length() > 0);
    Q_ASSERT(~listHasActionType(ol, CatItem::NOUN_ACTION));
    Q_ASSERT(~listHasActionType(ol, CatItem::OPERATION_ACTION));

    //noun then verb then Tag

    //Custom verb, then specific argument

    //long text, Custom verb, then specific arguments

}

void TheApplicationTester::testLongEdit(){

}

void TheApplicationTester::subTestCustomVerb(){
    //Create Custom Text
    clearEntry();
    sendKeysAndWait(longMessageStr);
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    qDebug() << "infamous resStr: " << resStr;
    Q_ASSERT(resStr==titlize(longMessageStr) || resStr==(longMessageStr));
    QString err;
    Q_ASSERT(lWind->m_inputList.canAddSlot(&err));
    QTest::keyClick(lWind, Qt::Key_Tab);
    Q_ASSERT(lWind->m_inputList.slotCount() == 2);
    QString tabStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(tabStr == titlize("") );
    Q_ASSERT(lWind->m_inputList.itemAtSlot(0).isEmpty());



    QFileInfo fileInfo("custom_actions/Send_Email.oneline");
    Q_ASSERT(fileInfo.exists());
    QSettings s(fileInfo.absoluteFilePath(), QSettings::IniFormat);

    //Add a custom verb
    CatItem custVerb(&s, CatItem("foo"));
    CatItem rt = CatItem::createActionItem(LONG_TEXT_NAME);
    QList<CatItem> sig;
    sig.append((rt));
    sig.append(CatItem::createActionItem(EMAIL_ADDRESS_NAME));
    //Q_ASSERT(custVerb.completeMatchArgSig(sig));
    CatBuilder::getCatalog()->cat_store.addItem(custVerb);

    //Add keys which should specify at least it...
    sendKeysAndWait(testEmailAddressStr);
    resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr == titlize(testEmailAddressStr) );

    Q_ASSERT(lWind->m_inputList.itemAtSlot(1).isEmpty());

    QList<CatItem> rawVerbs = CatBuilder::getCatalog()->cat_store.getCustomVerbByActionType(rt);
    Q_ASSERT(listContainsPath(rawVerbs,custVerb.getPath()));

    QList< QList < CatItem > > types = lWind->m_inputList.getCustomVerbSignature();
    Q_ASSERT(!types.isEmpty());

//    bool matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.matchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(matched);

//    matched = false;
//    for(int i=0; i < types.count(); i++){
//        if(custVerb.completeMatchArgSig(types[i])){
//            matched = true;
//        }
//    }
//    Q_ASSERT(matched);

    Q_ASSERT(lWind->m_inputList.slotCount()==2);
    Q_ASSERT(lWind->m_inputList.isCustomVerbChoosableByArgSig());

    //We're might have automatically chosen a custom verb but now we never do...


//    CatItem ov = lWind->m_inputList.getVerb();
//
//    Q_ASSERT(!ov.isEmpty());
//    Q_ASSERT((lWind->m_inputList.getVerb().getPath() ==CUSTOM_VERB_PREFIX
//              + "custom_actions/Send_Email.oneline"));
//
//    QString msg;
//    Q_ASSERT(lWind->m_inputList.canAddSlot(&msg));

}


//The GUI driver in here...
void TheApplicationTester::subtestGui(){
    //loopForProfiling();
    subTestTextEntry();
    subTestTextFormatting();
    subTestListBoxNav();
    subTestTabFromItem();
    subTestDoAction();
    subTestTagOperation();
    subTestCustomVerb();
}

void TheApplicationTester::loopForProfiling(){
    for(int i=0; i< 15; i++){
        subTestListBoxNav(i%29);
    }
}


//loop if we want to profile
void TheApplicationTester::testGui(){

    if(!the_test_builder){
        CatBuilder::destroyCatalog();
        the_test_builder = new CatBuilder(CatBuilder::REFRESH, *gDirs);
    }
    the_test_builder->catalogTask(CatBuilder::REFRESH);
    Q_ASSERT(CatBuilder::getCatalog());
    //Mostly so as to profile without loading test plugin
    loopForProfiling();
//    for(int i=0; i< 10; i++){
//        subtestGui();
//        //subTestListBoxNav(i%29);
//    }

}


void TheApplicationTester::subTestListBoxNav(int offset){
    clearEntry();
    QTest::keyClicks(lWind, QString(QChar('a'+offset)));
    lWind->st_showItemList=true;
    lWind->st_ListFilled = false;
    lWind->st_SearchResultsChanged = true;
    lWind->st_UserChoiceChanged = true;
    lWind->showitemList();

    QListWidgetItem* lItem = 0;
    QListWidgetItem* oldIt = 0;
    Q_ASSERT(lWind->m_itemChoiceList->count() > 0);
    do {
        oldIt = lItem;
        QTest::keyClick(lWind, Qt::Key_Up);
        lItem = lWind->m_itemChoiceList->currentItem();
        if(lItem){
            QString itemPath = lItem->data(ROLE_ITEM_PATH).toString();
            QString selectText = lWind->m_inputDisplay->getMainHtmlText();

            CatItem it = lWind->m_inputList.getItemByPath(itemPath);
            Q_ASSERT(it.getPath() == itemPath);


        }
        else{ break;}
    } while(oldIt != lItem);
    lItem = 0;
    do {
        oldIt = lItem;
        QTest::keyClick(lWind, Qt::Key_Down);
        lItem = lWind->m_itemChoiceList->currentItem();
        if(lItem){
            QString itemPath = lItem->data(ROLE_ITEM_PATH).toString();
            QString selectText = lWind->m_inputDisplay->getMainHtmlText();
            CatItem it = lWind->m_inputList.getItemByPath(itemPath);
            Q_ASSERT(it.getPath() == itemPath);
        }
        else{ break;}
    } while(oldIt != lItem);
    //Two escapes - to remove listbox and remove edit
    clearEntry();

//    Q_ASSERT(CatBuilder::getCatalog(
//            )->cat_store.getItemsByKey(
//                    str_list[0][0], &dummyInputList,10).count() >0);
}


void TheApplicationTester::testGuiIntegration(){
    //Catalog isn't loaded by extra threads during testing
    Q_ASSERT(lWind);

    lWind->loadCatalog(true);

    clearEntry();
    sendKeysAndWait("Laptop");
    QString resStr = lWind->m_inputDisplay->getMainHtmlText();
    Q_ASSERT(resStr==titlize("<font color=\"yellow\">Laptop</font>%20Diary3.odt"));
}

void TheApplicationTester::sendKeysAndWait(QString str){
    QTest::keyClicks(lWind, str);
    gTime_offset_for_testing += FORGROUND_SEARCH_DELAY+10;
    lWind->choiceUnderstoodTimeout();

}


//#endif
