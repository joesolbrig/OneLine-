/*
Oneline: Multibrower and Application Launcher
Copyright (C) 2012 Hans Solbrig

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

#include <QtGui>
#include <QUrl>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>
#include <QMessageBox>

#include "testPlugin.h"
#include "../../src/is_test.h"

#include "../../src/test_defines.h" //ONLY include this only per project!!


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


testpluginPlugin* gmypluginInstance = NULL;

void testpluginPlugin::getID(uint* id)
{
    *id = TEST_HASH ;
}

void testpluginPlugin::getName(QString* str)
{
    *str = TEST_PLUGIN_NAME;
}

void testpluginPlugin::init()
{
    itemPath = TEST_PLUGIN_NAME;
}



void testpluginPlugin::getLabels(InputList* )
{
}

void testpluginPlugin::extendCatalog(SearchInfo* , QList<CatItem>* r)
{
    r=r;
#ifdef TEST_GUI
    qDebug() << "testpluginPlugin::extendCatalog";
    QList <CatItem> in_list = *(info->itemListPtr);
    QString extend_suffix = "/extended/";
    CatItem me(itemPath);

    for(int i=0;i< in_list.count();i++){
        if(in_list[i].getPath().right(extend_suffix.length()) !=extend_suffix){
            CatItem e = in_list[i];
            CatItem f(e.getPath() + extend_suffix, e.getName() + extend_suffix);
            f.setExternalWeight(0,me);
            r->append(f);
        }
    }
#endif

}

void testpluginPlugin::getResults(InputList* , QList<CatItem>* results)
{
    qDebug() << "testpluginPlugin::getResults";
    results = results;
#ifdef TEST
    CatItem vi(testVerbStr, "normalVerb",TEST_HASH);
    vi.setItemType(CatItem::VERB);
    vi.setMatchType(CatItem::EXTERNAL_INFO);


    CatItem ci("customTestItem", "customTestItem",TEST_HASH);
    ci.setMatchType(CatItem::EXTERNAL_INFO);
    CatItem childI("testChildItem", "child",TEST_HASH);
    CatItem c("testChildItem", "child",TEST_HASH);
    ci.addChild(c);
	Q_ASSERT(results);
	results->append(ci);
    results->append(childI);

#endif

#ifdef TEST_GUI
    Q_ASSERT(results);

    CatItem ci("test://TestItem/Op", "Test Operation",TEST_HASH);
    ci.setDescription("Do A Test Operation...");
    ci.setItemType(CatItem::OPERATION);
    ci.setTakesAnykeys(true);
    results->append(ci);
#endif

}



QString testpluginPlugin::getIcon()
{
    return libPath + "/icons/test.png";
}

void testpluginPlugin::getCatalog(QList<CatItem>* items)
{
    items = items;
    CatItem me(itemPath);
#ifdef TEST
    Q_ASSERT(items);
    CatItem cv("test://catalogTestVerb", testVerbStr, TEST_HASH);
    cv.setItemType(CatItem::VERB);
    items->append(cv);

    CatItem ci("test://catalogTestNounItem", testNounStr , TEST_HASH);
    CatItem c("testCatalogChildItem", "testChild", TEST_HASH);
    ci.addChild(c);
    items->append(ci);

    for(uint i=0; i < str_len;i++){
        QString out_str = str_list[i][1];
        CatItem it(out_str, out_str, TEST_HASH);
        it.setExternalWeight(MAX_EXTERNAL_WEIGHT, me);
        qDebug() << "test item path: " << it.getPath();
        qDebug() << "test item name: " << it.getName();

        items->append(it);
    }
    QFileInfo fileInfo("~/.oneline/custom_actions/Send_Email.oneline");
    Q_ASSERT(fileInfo.exists());
    QSettings s(fileInfo.absoluteFilePath(), QSettings::IniFormat);

    CatItem custI(&s);

//    CatItem custI("vTestCustomVerb", testCustomVerbStr, TEST_HASH);
//    custI.setCustomPluginValue(MIN_ARG_NUM_KEY,2);
//    custI.setCustomPluginInfo(VERB_ARG "1","built-in://long-text | body | %s");
//    custI.setCustomPluginInfo(VERB_ARG "2","tag://email-address | recipient(s) | to %s ");
//    custI.setCustomPluginInfo(VERB_ARG "3","tag://email-address | recipient(s) | to %s ");
//    custI.setCustomPluginInfo(CUSTOM_ACTION_KEY,"test_action.sh");

#endif

#ifdef TEST_GUI
    Q_ASSERT(items);
    CatItem ci("catalogTestItem", "testItem", TEST_HASH);
    ci.setExternalWeight(HIGH_EXTERNAL_WEIGHT,me);
    ci.addChild(CatItem("testCatalogChildItem", "testChild", TEST_HASH));
    items->append(ci);
    for(uint i=0; i < str_len;i++){
        QString out_str = str_list[i][1];
        CatItem it(out_str, out_str, TEST_HASH);
        items->append(it);
    }

    CatItem ci0("AHighttestResultItem", "AHighcustom",TEST_HASH);
    ci0.setExternalWeight(HIGH_EXTERNAL_WEIGHT,me);
    items->append(ci0);

    CatItem ci1("AtestResultSource", "Acustom",TEST_HASH);
    ci1.setItemType(CatItem::TAG);
    CatItem childI1("AtestResultChild", "nps_child",TEST_HASH);
    ci1.addChild(childI1);
    items->append(childI1);
    items->append(ci1);

    CatItem ci2("XtestResultSource", "Xcustom",TEST_HASH);
    ci2.setItemType(CatItem::TAG);
    CatItem childI2("XtestResultChild", "wmv_child",TEST_HASH);

    ci2.addChild(childI2);
    items->append(childI2);
    items->append(ci2);

    CatItem ci3("UtestResultSource", "Ucustom",TEST_HASH);
    ci3.setItemType(CatItem::TAG);
    CatItem childI3("UtestResultChild", "pst_child",TEST_HASH);

    ci3.addChild(childI3);
    items->append(ci3);
    items->append(childI3);

    for(uint i=0; i < fill_len;i++){
        QString out_str = fill_list[i][1];
        CatItem it(out_str, out_str, TEST_HASH);
        items->append(it);
    }

    CatItem it("tag item");
    it.setItemType(CatItem::OPERATION);
    it.setTakesAnykeys(true);

    for(int i=0; i< 20; i++){
        CatItem it(QString::number(i));
        ListItem li(it);
        for(int j=0; j< 30; j++){
            CatItem it2(numAsStr(i));
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




#endif
}

int testpluginPlugin::launchItem(InputList* , QList<CatItem>* ) {
#ifdef TEST
    printf("ItemLaunched");
    return 0;
#endif

#ifdef TEST_GUI
    QMessageBox msgBox;
    msgBox.setText("test item executed");
    msgBox.exec();
    return 0;
#else
    return 0;
#endif
}

void testpluginPlugin::doDialog(QWidget* , QWidget** )
{

}

void testpluginPlugin::endDialog(bool )
{
}

void testpluginPlugin::setPath(QString * path) {
    libPath = *path;
}

int testpluginPlugin::msg(int msgId, void* wParam, void* lParam)
{
    bool handled = false;
    switch (msgId)
    {
    case MSG_INIT:
        init();
        handled = true;
        break;
    case MSG_GET_LABELS:
        getLabels((InputList*) wParam);
        handled = true;
        break;
    case MSG_GET_ID:
        getID((uint*) wParam);
        handled = true;
        break;
    case MSG_GET_NAME:
        getName((QString*) wParam);
        handled = true;
        break;
    case MSG_GET_RESULTS:
        getResults((InputList*) wParam, (QList<CatItem>*) lParam);
        handled = true;
        break;
    case MSG_GET_CATALOG:
        getCatalog((QList<CatItem>*) wParam);
        handled = true;
        break;
    case MSG_EXTEND_CATALOG:
        extendCatalog((SearchInfo*) wParam, (QList<CatItem>*)lParam);
        handled = true;
        break;
    case MSG_LAUNCH_ITEM:
        handled = launchItem((InputList*) wParam, (QList<CatItem>*) lParam);
        break;
    case MSG_HAS_DIALOG:
        handled = true;
        break;
    case MSG_DO_DIALOG:
        doDialog((QWidget*) wParam, (QWidget**) lParam);
        break;
    case MSG_END_DIALOG:
        endDialog((bool) wParam);
        break;
    case MSG_PATH:
        setPath((QString *) wParam);
    default:
        break;
    }

    return handled;
}

Q_EXPORT_PLUGIN2(myplugin, testpluginPlugin) 
