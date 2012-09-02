#include <QtCore/QCoreApplication>
#include <QFile>

#include "streamplugin.h"
#include "facebook_reader.h"
#include "twitter_reader.h"
#include "previewpane.h"

#include "fillInItem.h"

#include "ui_pin_dialog.h"

#include "parser.h"
#include "serializer.h"

#include <QVariant>

//These are all out-of-date, twitter's fricken api changes something like weekly!!

const QString LOGIN_URL("http://twitter.com/account/verify_credentials.xml");
const QString GET_FRIENDS_XML_URL("http://twitter.com/statuses/friends.xml");
const QString GET_FOLLOWERS_XML_URL("http://twitter.com/statuses/followers.xml");
const QString GET_PUBLIC_STATUS_URL("http://twitter.com/statuses/public_timeline.xml");
const QString GET_FRIENDS_STATUS_URL("http://twitter.com/statuses/friends_timeline.xml");
const QString GET_MY_STATUS__XML_URL("http://twitter.com/statuses/user_timeline.xml");
const QString GET_USER_STATUS_XML_URL("http://twitter.com/statuses/user_timeline/%1.xml");
const QString UPDATE_STATUS_XML_URL("http://twitter.com/statuses/update.xml");

const QString LOGIN_JSON_URL("http://twitter.com/account/verify_credentials.json");
const QString GET_FRIENDS_JSON_URL("http://twitter.com/statuses/friends.json");
const QString GET_FOLLOWERS_JSON_URL("http://twitter.com/statuses/followers.json");
const QString GET_PUBLIC_STATUS_JSON_URL("http://twitter.com/statuses/public_timeline.json");
const QString GET_FRIENDS_STATUS_JSON_URL("http://twitter.com/statuses/friends_timeline.json");
const QString GET_MY_STATUS_JSON_URL("http://twitter.com/statuses/user_timeline.json");
const QString GET_USER_STATUS_JSON_URL("http://twitter.com/statuses/user_timeline/%1.json");
const QString UPDATE_STATUS_JSON_URL("http://twitter.com/statuses/update.json");

//This is specific to my test account
const QString PermissionToken = "7742232";

const QString staticUserToken = "200386187-gu63rrm6da0xgQyWf0FQEtQb5C9LNJGTp41yIuFm";
const QString staticUserTokenSecret =  "lmspKhT3MIKauxu9EDxwpk6ObBIAs1VhimvQU0DeE";

const QString STREAM_PATH("STREAM_PATH");

/*
Example token:

"http://www.facebook.com/connect/login_success.html?
code=7R0jVsc01AzKRLuYwhwagCGn4EnYcNMULITVD3cUyw4.eyJpdiI6IjBfRFEybEZzdmptUjE5WTdFZHA4dmcifQ."
"OHUs55szl1iFwJVsJh0wH4ygepx9JBAsygY2-wEscd_kMLeIsFQhhoIcDQ9oHfGDnyBFrfdLafKHpArAbGvp75ZcYX-s"
"WmrOakgGZfEXn-oGko7n7WuYwBcq-r2ICgAd0oUbPxkGuwXBo5Mo8-c-NQ" */


CatItem item_by_path(QList<CatItem> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == path){
            return items[i];
        }
    }
    Q_ASSERT(false);
    return CatItem();
}

CatItem item_by_unique_name(QList<CatItem> items, QString name){
    bool found = false;
    int index=-1;
    for(int i=0;i<items.count();i++){
        if(items[i].getName() == name){
            //name needs to be unique usually
            Q_ASSERT(!found);
            found = true;
            index = i;
        }
    }
    if(index >=0){ return items[index]; }
    Q_ASSERT(false);
    return CatItem();
}

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

bool listContainsName(QList<CatItem> items, QString name){
    for(int i=0;i<items.count();i++){
        if(items[i].getName() == name){
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

bool listContainsItem(QList<CatItem> items, CatItem it){
    for(int i=0;i<items.count();i++){
        if(items[i].getPath() == it.getPath()){
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

class TwitterTester;


void testFillitem2(){
    QSettings s("facebook_link2.item_tmplate",QSettings::IniFormat);
    CatItem it(&s);
    it.setCustomPluginInfo(STREAM_PASSWORD_PATH,"RedQueen9");

    FillinItem fi(it);
    fi.setGenerating();
    QHash<QString, QString> hsh;
    hsh["user_name"] = "Joe The User";
    fi.addHash(hsh);
    fi.addInternalGenerator();
    VerbItem vi;
    QHash<QString, EntryField> qh;
    CatItem resultItem = fi.createTheItem(vi, qh);
    QList<CatItem> childs = resultItem.getChildren();

    Q_ASSERT(childs.count()==16);

    Q_ASSERT(listContainsName(childs,"Likes"));

}

void testFillitem3(){
    CatItem it("fillin_test");

    it.setCustomPluginInfo(FILLIN_GEN_FILE_KEY,
                           "facebook_link.item_tmplate");
    it.setCustomPluginInfo(FILLIN_GEN_SUBKEY_KEY, FACEBOOK_ITEM_GENERATOR);


    //----------------------
    QFile testTransFormedFeedfile("facebook_feed2.json");
    testTransFormedFeedfile.open(QIODevice::ReadOnly |QIODevice::Text);
    QByteArray response =  testTransFormedFeedfile.readAll();
    testTransFormedFeedfile.close();

    qDebug() << "parseTransformedStream: " << response;

    bool ok;
    QJson::Parser parser;

    QVariant parsedResult = parser.parse(response, &ok);
    if (!ok) {
        qWarning() << "Error while parsing network reply: " << response;
        return;
    }
    VariantTree vt(parsedResult);

//    QString fillinFile = QDir::homePath() + DEFAULT_APP_DIR + "/generated_items/Profile feed (Wall) of Hans Solbrig190065705.oneline";
    QString fillinFile =
            "../../script/custom_actions/add_facebook_account.oneline";
    QString fillinKey ="FACEBOOK_WALL_GENERATOR";

    QSettings set(fillinFile, QSettings::IniFormat);
//    qDebug() << "reading file" << set.fileName();
//    qDebug() << "reading current keys" << set.childKeys();
//    qDebug() << "reading all keys" << set.allKeys();
//    set.beginGroup(fillinKey);
//    qDebug() << "reading keys" << set.childKeys();
//    qDebug() << "beginParseStream: " << fillinFile << " | " << fillinKey;
//    set.endArray();
    SettingsTree st(&set,fillinKey);
    FillinItem resultAccumulator;

    resultAccumulator.setGeneratingItem(it);
    resultAccumulator.addData(&st);
    resultAccumulator.setGenerating();

    resultAccumulator.addData(&vt);
    VerbItem vi(it);
    CatItem baseResult = resultAccumulator.createTheItem(vi, QHash<QString, EntryField>());
    QList<CatItem> childs = baseResult.getChildren();
    Q_ASSERT(listContainsPath(childs, "feed://facebook/news/65491870325_372962682758295"));
    CatItem fbItem = item_by_path(childs, "feed://facebook/news/65491870325_372962682758295");
    childs = fbItem.getChildren();
    QList<CatItem> parents = fbItem.getParents();
    Q_ASSERT(listContainsPath(childs, "http://rww.to/K2SoXC"));
    Q_ASSERT(listContainsName(parents, "ReadWriteWeb"));
    CatItem rww = item_by_unique_name(parents,"ReadWriteWeb");
    childs = rww.getParents();
    Q_ASSERT(listContainsPath(childs, "person://ReadWriteWeb"));

}

void testFillitem(){

    CatItem it("fillin_test");

    it.setCustomPluginInfo(FILLIN_GEN_FILE_KEY,
                           "facebook_link.item_tmplate");
    it.setCustomPluginInfo(FILLIN_GEN_SUBKEY_KEY, FACEBOOK_ITEM_GENERATOR);

    QSettings s("facebook_link.item_tmplate",
                QSettings::IniFormat);
    Q_ASSERT(s.status() == QSettings::NoError);
    s.beginGroup(FACEBOOK_ITEM_GENERATOR);
    QList<QString> ks = s.childKeys();
    Q_ASSERT(ks.count()>0);
    s.endGroup();

    SettingsTree st(&s,FACEBOOK_ITEM_GENERATOR);
    DataTree* dt = &st;
    FillinItem fi(dt);

    QList<VerbItem> rv = fi.getItems();
    Q_ASSERT(listContainsName(rv,"Item_Path"));
    Q_ASSERT(listContainsName(rv,"DESCRIPTION_KEY_STR"));
    Q_ASSERT(rv.count()==5);

    QByteArray ba;
    QFile testTransFormedFeedfile("fb_link_example.json");

    Q_ASSERT(testTransFormedFeedfile.exists());

    testTransFormedFeedfile.open(QIODevice::ReadOnly |QIODevice::Text);
    ba =  testTransFormedFeedfile.readAll();
    testTransFormedFeedfile.close();

    bool ok;
    QJson::Parser parser;

    QVariant parsedResult = parser.parse(ba, &ok);
    Q_ASSERT(ok);

    Q_ASSERT(parsedResult.canConvert(QVariant::Map));
    QMap<QString, QVariant> vh = parsedResult.toMap();
    Q_ASSERT(vh.contains("id"));
    Q_ASSERT(vh.contains("from"));

    VariantTree vt(parsedResult);

    fi.setGenerating();
    fi.addData(&vt);
    VerbItem vi(it);
    QHash<QString, EntryField> qh;
    CatItem testItem = fi.createTheItem(vi,qh);
    Q_ASSERT(testItem.getDescription()==
             "Good luck, Rishin Banker.  Talk to Atish Mehta if you need help growing a mustache.");
    Q_ASSERT(testItem.getIcon()== "http://static.ak.fbcdn.net/rsrc.php/v1/yD/r/aS8ecmYRys0.gif");

    QObject dummy;
    FacebookReader jr(&dummy,it);
    jr.beginParseStream();
    jr.parseTransformedStream(ba);

    QList<CatItem> items = jr.getResults();
    QList<CatItem> childs = items[0].getChildren();

    Q_ASSERT(childs.count() >2);

    Q_ASSERT(item_by_path(childs, "http://www.youtube.com/watch?v=JA068qeB0oM").getName()
             =="The Next CEO Competition 2010");

    CatItem comment = item_by_path(childs,"facebook://comment/114961875194024_337654");

    Q_ASSERT(comment.getDescription()
             ==    "People listen to a man with a commanding moustache.  "
             "I highly recommend it, if you want to lay the framework for America."
             "  Especially for a lad with your boyish good looks.");

    Q_ASSERT(comment.getName() == "People listen to a m");

    Q_ASSERT(item_by_path(childs,"facebook://contact/7901103").getName()
             ==  "Arjun Banker");

}



//This is a test, keep it as simple as possible
void testTwitterFull(){

    StreamPlugin multiFeedReader;

    CatItem statusSourceItem("stream://twitter_test");
    statusSourceItem.setCustomPluginInfo(STREAM_PATH,GET_MY_STATUS__XML_URL);
    statusSourceItem.setCustomPluginInfo(XSLT_SOURCE_PREFIX,"../xml_code/twitter/public.xsl");
    statusSourceItem.setCustomPluginInfo(STREAM_LOGIN_PATH,TWITTER_LOGIN_URL);
    statusSourceItem.setCustomPluginInfo(STREAM_USERNAME_PATH,"JoeSoulbringer");
    //statusSourceItem.setCustomPluginInfo(STREAM_PASSWORD_PATH,"RedQueen9");

//    statusSourceItem.setCustomPluginInfo(TWITTER_CONSUMER_KEY_KEY,"m4Dx9OiELn7KLIui2Aik9Q");
//    statusSourceItem.setCustomPluginInfo(TWITTER_CONSUMER_SECRET_KEY,"Iv5VeRWwg21FoZya7247t4OjKTNpvlmX1KTXFqZ1NY");

    //statusSourceItem.setCustomPluginInfo(STREAM_PLUGIN_LOGIN_PATH,PermissionToken);
    statusSourceItem.setCustomPluginInfo(STREAM_PASSWORD_PATH,PermissionToken);
    statusSourceItem.setLabel((TWITTER_SOURCE_STR));

    QList<CatItem> itms;
    itms.append(statusSourceItem);

    SearchInfo* info;
    info = new SearchInfo();
    info->itemListPtr = &itms;

    QList<CatItem> r;
    multiFeedReader.getFeedReaders(info,&r);
    Q_ASSERT(r.count()>1);
    Q_ASSERT(r[0].getDescription() == "These updates promise to be extremely boring");
    delete info;
}


QList<CatItem> test5(){

    QFile testTransFormedFeedfile("test_html_head.txt");
    testTransFormedFeedfile.open(QIODevice::ReadOnly |QIODevice::Text);
    QByteArray testTransFormedFeed = testTransFormedFeedfile.readAll();

    CatItem it("http://www.libcom.org/forums/thought");
    StreamPlugin streamPlugin;
    HttpHeadReader headReader(&streamPlugin, it);
    headReader.parseTransformedStream(testTransFormedFeed);
    QList<CatItem> res = headReader.getResults();
    Q_ASSERT(res.count()==1);
    CatItem rItem = res[0];
    CatItem ris(rItem);
    Q_ASSERT(ris.getRequestUrl() == "http://www.libcom.org/forums/thought-0/feed");
    return res;

}

void test6(QList<CatItem> itms){

    StreamPlugin multiFeedReader;

    SearchInfo* info;
    info->itemListPtr = &itms;
    QList<CatItem> r;

    multiFeedReader.getFeedReaders(info,&r);
    Q_ASSERT(r.count()==17);
    Q_ASSERT(r[0].getName() == "theory");
    Q_ASSERT(r[1].getName() == "The abolition of money");

}

void test7(){

    StreamPlugin multiFeedReader;

    CatItem it("http://www.libcom.org/forums/thought");
    QList<CatItem> itms;
    itms.append(it);

    SearchInfo* info;
    info->itemListPtr = &itms;
    QList<CatItem> r;

    multiFeedReader.getFeedReaders(info,&r);
    Q_ASSERT(r.count()==17);
    Q_ASSERT(r[0].getName() == "theory");
    Q_ASSERT(r[1].getName() == "Psychoanalysis and the communist movement");

}



//Older...
//void test1(){
//
//    QFile testTransFormedFeedfile("test_trans_feed.rss");
//    testTransFormedFeedfile.open(QIODevice::ReadOnly |QIODevice::Text);
//    QByteArray testTransFormedFeed = testTransFormedFeedfile.readAll();
//
//    QSettings set("test.ini");
//
//    StreamPlugin multiFeedReader;
//    multiFeedReader.init(&set);
//
//
//    RemoteItemSource filt;
//    XSLTFeedReader singleFeedReader1(&multiFeedReader, filt);
//    singleFeedReader1.parseTransformedStream(testTransFormedFeed);
//
//    QList<CatItem> r = singleFeedReader1.getResults();
//
//    Q_ASSERT(r.count()==4);
//    Q_ASSERT(r[0].getName() == "theory");
//    Q_ASSERT(r[1].getName() == "Psychoanalysis and the communist movement");
//
//
//
//
//}

void test1(){

//    QString filterFilePath = QDir::homePath() + USER_APP_DIR + XLS_SUBDIR + BOOKMARK_XSLT;

//    CatItem recentItemSource1(XMEL_BOOKMARKS_PATH1);
//    recentItemSource1.setCustomPluginInfo(STREAM_FILTER_SOURCE,filterFilePath);
//    recentItemSource1.setCustomPluginInfo(STREAM_PLUGIN_SOURCE_PATH,QDir::homePath() + XBEL_HISTORY_NAME1);
//    recentItemSource1.setPluginId(qHash(QString(XLSTPLUGIN_NAME)));
//
//    CatItem recentItemSource2(XMEL_BOOKMARKS_PATH2);
//    recentItemSource2.setCustomPluginInfo(STREAM_FILTER_SOURCE,filterFilePath);
//    recentItemSource2.setCustomPluginInfo(STREAM_PLUGIN_SOURCE_PATH,QDir::homePath() + XBEL_HISTORY_NAME2);
//    recentItemSource2.setPluginId(qHash(QString(XLSTPLUGIN_NAME)));;

    CatItem recentItemSource1(XMEL_BOOKMARKS_PATH1);
    QString filterFilePath = QDir::homePath() + DEFAULT_APP_DIR + XLS_SUBDIR + BOOKMARK_XSLT;

    recentItemSource1.setFilterFilePath(filterFilePath);
    recentItemSource1.setCustomPluginInfo(STREAM_PATH,QDir::homePath() + XBEL_HISTORY_NAME1);
    recentItemSource1.setSourceWeight(MAX_EXTERNAL_WEIGHT, recentItemSource1);
    recentItemSource1.setTagLevel(CatItem::INTERNAL_SOURCE);;

    CatItem recentItemSource2(XMEL_BOOKMARKS_PATH2);
    QString oldFilterFilePath = QDir::homePath() + DEFAULT_APP_DIR + XLS_SUBDIR + OLD_STYLE_BOOKMARK_XSLT;

    recentItemSource2.setFilterFilePath(oldFilterFilePath);
    recentItemSource2.setCustomPluginInfo(STREAM_PATH,QDir::homePath() + XBEL_HISTORY_NAME2);
    recentItemSource2.setSourceWeight(MAX_EXTERNAL_WEIGHT, recentItemSource1);
    recentItemSource2.setTagLevel(CatItem::INTERNAL_SOURCE);;

    QList<CatItem> itemList;
    itemList.append(recentItemSource1);
    itemList.append(recentItemSource2);


    SearchInfo info;
    info.itemListPtr = &itemList;

//    QObject dummyObject;
//    RemoteItemSource filt;
//    XmlFileReader singleFeedReader1(&dummyObject, filt);

    StreamPlugin sp;

    QList<CatItem> results;
    sp.extendCatalog(&info,&results);
    Q_ASSERT(listContainsPath(results,
           "file:///~/.oneline/oneline.ini"));

}

void test2(){
    QFile xlsCodefile("../xml_code/rss/rss.xsl");
    Q_ASSERT(xlsCodefile.exists());
    xlsCodefile.open(QIODevice::ReadOnly |QIODevice::Text);
    QString xlsCode = xlsCodefile.readAll();

    QFile rssFeedfile("test_feed.rss");
    rssFeedfile.open(QIODevice::ReadOnly |QIODevice::Text);
    QByteArray rssFeed = rssFeedfile.readAll();

    QSettings set("test.ini");

    CatItem filt;
    filt.setFilterStr(xlsCode);

    StreamPlugin multiFeedReader;
    multiFeedReader.init(&set);

    XSLTFeedReader singleFeedReader1(&multiFeedReader, filt);

    QByteArray ba = singleFeedReader1.processRawStream(rssFeed);
    QFile testTransFormedFeedfile("test_trans_feed.rss");
    testTransFormedFeedfile.open(QIODevice::WriteOnly |QIODevice::Text);
    testTransFormedFeedfile.write(ba);
    testTransFormedFeedfile.close();
    singleFeedReader1.parseTransformedStream(ba);

    QList<CatItem> r = singleFeedReader1.getResults();
    Q_ASSERT(listContainsName(r,"Psychoanalysis and the communist movement"));

}

//Test the label/custom string functionality...
#define TEST_STR "Test String"
void test3(){
    CatItem testSource("testSource");
    testSource.setCustomPluginInfo(XSLT_SOURCE_STR,TEST_STR);
    const char* k;
    k = (XSLT_SOURCE_STR).toAscii();
    QString resStr = testSource.getCustomString(XSLT_SOURCE_STR);
    Q_ASSERT(resStr==TEST_STR);
    Q_ASSERT(testSource.hasLabel(k));
    Q_ASSERT(testSource.hasLabel((XSLT_SOURCE_STR)));


    CatItem testCopy = testSource;
    Q_ASSERT(testCopy.getCustomString(XSLT_SOURCE_STR)==TEST_STR);
    Q_ASSERT(testCopy.hasLabel((XSLT_SOURCE_STR)));
    QList<CatItem> tsl;
    tsl.append(testCopy);
    Q_ASSERT(tsl[0].getCustomString(XSLT_SOURCE_STR)==TEST_STR);
    Q_ASSERT(tsl[0].hasLabel((XSLT_SOURCE_STR)));

}




void testRSS(){


    QFile xlsCodefile("../xml_code/rss/rss.xsl");
    Q_ASSERT(xlsCodefile.exists());
    xlsCodefile.open(QIODevice::ReadOnly |QIODevice::Text);
    QString xlsCode = xlsCodefile.readAll();

    CatItem testSource("http://libcom.org/forums/thought-0/feed");
    testSource.setFilterStr(xlsCode);

    testSource.setLabel(FIREFOX_PLUGIN_NAME);
    testSource.setLabel(HTML_HEADER_PROCESSED_KEY);
    testSource.setLabel(XSLT_SOURCE_STR);

//    CatItem testChildSource("http://www.libcom.org/forums/thought-0/feed");
//    testChildSource.setLabel(HTML_HEADER_PROCESSED_KEY);
//    testChildSource.setRequestUrl("http://www.libcom.org/forums/thought-0/feed");
//
//    testSource.addChild(testChildSource);

    testSource.setRequestUrl("http://libcom.org/forums/thought-0/feed");
    QList<CatItem> tsl;
    tsl.append(testSource);

    //Q_ASSERT(tsl[0].getChildren().count()==1);
    

    SearchInfo info;
    info.itemListPtr = &tsl;
    QList<CatItem> results;

    QSettings set("test.ini");
    StreamPlugin multiFeedReader;
    multiFeedReader.init(&set);
    multiFeedReader.extendCatalog(&info, &results);

    Q_ASSERT(results.count() > 10);
    Q_ASSERT(!results[0].getName().isEmpty());
    Q_ASSERT(!results[1].getName().isEmpty());
    Q_ASSERT(listContainsName( results, "Luxemburg:The Mass Strike, the Political Party"));

    //Q_ASSERT(listContainsItem());
}

class TwitterTester: public AbstractReceiverWidget
{
    Q_OBJECT
    Ui::Pin_Dialog* m_pDialog;
    QDialog* m_dialog;
    TwitterReader* m_tr;

public:
    TwitterTester(QWidget* parent=0): AbstractReceiverWidget( parent){
        m_pDialog = 0;
    }

    ~TwitterTester(){
        if(m_pDialog){
            delete m_pDialog;
        }
        if(m_dialog){
            delete m_dialog;
        }
    }

    void showIt(){
        QWidget::show();
        testTwitterAuthorize();
        //testTwitterAccessInformation();
    }
    //void testTwitter();
    void testTwitterAccessInformation();
    void testTwitterAuthorize(){
        CatItem it;
        QObject o;
        m_tr = new TwitterReader(&o,it);
        QString url = m_tr->createPermissionURL();
        qDebug() << "got url:" << url;

        PreviewPane* pp = new PreviewPane(0);
        pp->setUrl(QUrl(url));
        //pp->hookURLChange(this);
        urlChanged();

        pp->show();

        //The user access tokens are gotten in pinEntered()...

    }

public slots:
    void execute();
    void pinEntered(){
        QString pin = m_pDialog->textEdit->toPlainText ();
        qDebug() << "got pin" << pin;

        QOAuth::ParamMap pm = m_tr->createAccessToken(pin);

        QSettings s("stream_test.ini",
                    QSettings::IniFormat);
        QByteArray userToken = pm.value(QOAuth::tokenParameterName());
        QByteArray userSecret = pm.value(QOAuth::tokenSecretParameterName());

        qDebug() << "userToken: " << userToken;
        qDebug() << "userSecret: " << userSecret;

        s.setValue("Twitter_Token", userToken);
        s.setValue("Twitter_Secret", userSecret);
        s.sync();

        testTwitterAccessInformation();

    }

    virtual void urlChanged(){
        m_dialog = new QDialog(this);
        m_pDialog = new Ui::Pin_Dialog();
        m_pDialog->setupUi(m_dialog);
        m_dialog->resize(300,300);
        m_dialog->move(0,0);
        m_dialog->show();
        m_dialog->raise();
        m_dialog->activateWindow();
        connect(m_pDialog->buttonBox, SIGNAL(accepted()), this, SLOT(pinEntered()));

    }

};

void TwitterTester::execute(){
    //test6();
    //testTwitter();
    //testTwitterMin();

    test2();
    testFillitem();
    testRSS();

    test1();
    test3();
    test5();
    QCoreApplication::instance()->quit();
}


void TwitterTester::testTwitterAccessInformation(){

    QSettings s("stream_test.ini",
                QSettings::IniFormat);

    QByteArray userToken = s.value("Twitter_Token").toByteArray();
    QByteArray userSecret = s.value("Twitter_Secret").toByteArray();

//    QByteArray userToken = staticUserToken.t;
//    QByteArray userSecret = staticUserTokenSecret ;

    qDebug() << "restored userToken: " << userToken;
    qDebug() << "restored userSecret: " << userSecret;

    CatItem statusSourceItem("stream://twitter_test");
    statusSourceItem.setCustomPluginInfo(STREAM_PATH,GET_MY_STATUS_JSON_URL);
    statusSourceItem.setCustomPluginInfo(XSLT_SOURCE_PREFIX,"../xml_code/twitter/public.xsl");
    statusSourceItem.setCustomPluginInfo(STREAM_LOGIN_PATH,TWITTER_LOGIN_URL);
    statusSourceItem.setCustomPluginInfo(STREAM_USERNAME_PATH,"JoeSoulbringer");

    statusSourceItem.setCustomPluginInfo(TWITTER_CONSUMER_KEY_KEY,userToken);
    statusSourceItem.setCustomPluginInfo(TWITTER_CONSUMER_SECRET_KEY,userSecret);

    //statusSourceItem.setCustomPluginInfo(STREAM_PLUGIN_LOGIN_PATH,PermissionToken);
    statusSourceItem.setCustomPluginInfo(STREAM_LOGIN_PATH,PermissionToken);
    statusSourceItem.setLabel((TWITTER_SOURCE_STR));


    QObject parent;
    TwitterReader tr(&parent, statusSourceItem);

    QNetworkRequest request;
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);

    QList<CatItem> itms;
    itms.append(statusSourceItem);

    SearchInfo* info;
    info = new SearchInfo();
    info->itemListPtr = &itms;

    tr.setAuthorization(&request);
    qDebug() << "request url:" << tr.getFilter().getRequestUrl();

    QNetworkAccessManager nam;

    QEventLoop loop;
    QNetworkReply* reply = nam.get(request);
    if(!reply){
        qDebug() << "Failed to create reply object";
        loop.quit();
    }
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Download Error: " << reply->errorString();
        Q_ASSERT(false);
    } else {
        QByteArray ba = (reply->readAll());
        qDebug() << "got data" << ba;
        QFile testTransFormedFeedfile("twitter_feed_result.json");
        testTransFormedFeedfile.open(QIODevice::WriteOnly |QIODevice::Text);
        testTransFormedFeedfile.write(ba);
        testTransFormedFeedfile.close();
    }
    QWidget::close();
    QApplication::instance()->quit();

}

class PreviewTester: public AbstractReceiverWidget
{
    Q_OBJECT

    PreviewPane* m_pp;


public:
    PreviewTester(QWidget* parent=0): AbstractReceiverWidget( parent){
        m_pp = 0;

    }
    ~PreviewTester(){
        if(m_pp){
            delete m_pp;
        }
    }

    void showIt(){
        QWidget::show();
        testPreview();
    }

    void testPreview(){
        CatItem it("test:testPreviewItem");
        it.setJavascriptFilterFilePath("./javascript/reader.js");


        m_pp = new PreviewPane(0);
        m_pp->setUrl(QUrl(url));
        m_pp->hookURLChange(this);

        m_pp->show();

        //The user access tokens are gotten in pinEntered()...

    }

public slots:
    void execute();

    //Call multiple times
    virtual void urlChanged(const QUrl &url);

};


class FacebookTester: public AbstractReceiverWidget
{
    Q_OBJECT
    Ui::Pin_Dialog* m_pDialog;
    FacebookReader* m_facebookReader;
    PreviewPane* m_pp;


public:
    FacebookTester(QWidget* parent=0): AbstractReceiverWidget( parent){
        m_pp = 0;

    }
    ~FacebookTester(){
        if(m_pp){
            delete m_pp;
        }
    }

    void showIt(){
        QWidget::show();
        testFacebookAuthorize();
    }
    //testOAuthWindow2();
    //void testTwitter();
    void testFacebookAccessInformation();
    void testFacebookAuthorize(){
        CatItem it;

        QObject o;
        m_facebookReader = new FacebookReader(&o,it);
        QString url = m_facebookReader->createPermissionURL();
        qDebug() << "got url:" << url;

        m_pp = new PreviewPane(0);
        m_pp->setUrl(QUrl(url));
        m_pp->hookURLChange(this);

        m_pp->show();

        //The user access tokens are gotten in pinEntered()...

    }

public slots:
    void execute();

    //Call multiple times
    virtual void urlChanged(const QUrl &url);

};

void FacebookTester::urlChanged(const QUrl &url){
    qDebug() << "got url" << url.toString();

    QString oAuthCode = m_facebookReader->scanResultURL(url.toString());
    if(oAuthCode.isEmpty()){
        qDebug() << "got no code, probably first pass: " << url.toString();
        return;
    }
    qDebug() << "got code" << oAuthCode;

    QString oAuthKey = m_facebookReader->tokenGetter(oAuthCode);
    qDebug() << "got key" << oAuthKey;

    Q_ASSERT(!oAuthKey.isEmpty());
    QSettings s("stream_test.ini",
                QSettings::IniFormat);
    s.setValue("oAuthKey", oAuthKey );
    s.sync();

    m_pp->close();
    testFacebookAccessInformation();

}


void FacebookTester::execute(){

    testFillitem3();
    QCoreApplication::instance()->quit();

//    testFacebookAccessInformation();
//    QCoreApplication::instance()->quit();
}

void FacebookTester::testFacebookAccessInformation(){

    QSettings s("stream_test.ini",
                QSettings::IniFormat);

    QByteArray oAuthKey = s.value("oAuthKey").toByteArray();

    qDebug() << "restored oAuthKey : " << oAuthKey;

    CatItem statusSourceItem("stream://facebook_test");
    statusSourceItem.setCustomPluginInfo(JSON_ACCESS_TOKEN,oAuthKey);

    QString sourceUrl = "https://graph.facebook.com/me/likes?access_token=%1";
    sourceUrl = sourceUrl.arg(oAuthKey.constData());
    statusSourceItem.setCustomPluginInfo(STREAM_PATH,oAuthKey);

    QObject parent;
    FacebookReader fBR(&parent, statusSourceItem);

    QNetworkRequest request;
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);

    QList<CatItem> itms;
    itms.append(statusSourceItem);

    SearchInfo* info;
    info = new SearchInfo();
    info->itemListPtr = &itms;


    fBR.setAuthorization(&request);
    qDebug() << "request url:" << fBR.getFilter().getRequestUrl();

    QUrl url = QUrl(fBR.getFilter().getRequestUrl());
    request.setUrl(sourceUrl);
//    request.setUrl(url);
    QNetworkAccessManager nam;

    QEventLoop loop;
    QNetworkReply* reply = nam.get(request);
    if(!reply){
        qDebug() << "Failed to create reply object";
        loop.quit();
    }
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray ba;
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Download Error: " << reply->errorString();
        Q_ASSERT(false);
    } else {
        ba = (reply->readAll());
        qDebug() << "got data" << ba;
    }

    QFile testTransFormedFeedfile("facebook_feed_result.json");
    testTransFormedFeedfile.open(QIODevice::WriteOnly |QIODevice::Text);
    testTransFormedFeedfile.write(ba);
    testTransFormedFeedfile.close();



    //QWidget::close();
    //QApplication::instance()->quit();

}


int main(int argc, char *argv[])
{
//    QApplication   app(argc, argv); //QCoreApplication
//    FacebookTester tester;
//    QTimer::singleShot(0, &tester, SLOT(execute()));
//    app.exec();

    QApplication a(argc, argv);
//    TwitterTester t;
    PreviewTester t;
    t.showIt();
    return a.exec();

}

#include "main.moc"
