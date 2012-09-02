#include <QFileInfo>
#include "item.h"
#include "inputLanguage.h"
#include "inputlanguagetester.h"
#include "itemarglist.h"

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

bool listContainsName(QList<CatItem> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i].getName() == path){
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

InputLanguageTester::InputLanguageTester(QObject *parent) :
    QObject(parent)
{

}

void InputLanguageTester::testArgList(){

    CatItem custVerb ("test://customVerb", "email");
    custVerb.setDescription("Send Email");
    custVerb.setItemType(CatItem::VERB);
    custVerb.setVerbDescription(
            " email ['$body:longtext+' ][title:'$title:shorttext' ]"
            "to: $recepient:emailaddress+ $cc:longtext ");

    //custVerb.setCustomPluginInfo(VERB_COMMAND_STRING, "cust_Acton $body t $title");

    custVerb.setCustomCommandLine("cust_Acton $body t $title");
    custVerb.setTakesAnykeys(true);

    CatItem noun1("test://generic_document1");
    CatItem noun2("test://generic_document2");

    CatItem stndVerb("test://generic_verb");
    stndVerb.setItemType(CatItem::VERB);


    CatItem tagOp("test://generic_tag");
    tagOp.setItemType(CatItem::OPERATION);

    InputList ial;

    //noun verb
    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.setItem(noun1);
    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.addSlot();

    Q_ASSERT(!ial.acceptItem(&noun1));
    Q_ASSERT(ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));


    ial.setItem(stndVerb);
    ial.addSlot();

    Q_ASSERT(!ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.clearAll();
    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    //verb noun
    ial.setItem(stndVerb);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.setUserKeys("Food");

    QString err;
    Q_ASSERT(ial.canAddSlot(&err));

    ial.setItem(noun1);
    Q_ASSERT(ial.canAddSlot(&err));
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));


    ial.setItem(noun1);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun2));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.clearAll();

    ial.setItem(tagOp);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(ial.acceptItem(&stndVerb));
    Q_ASSERT(!ial.acceptItem(&tagOp));

    //verb noun
    ial.setItem(custVerb);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(!ial.acceptItem(&tagOp));

    ial.setItem(noun1);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(!ial.acceptItem(&tagOp));

    ial.setItem(noun2);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(!ial.acceptItem(&tagOp));

    //Q_ASSERT(ial.getVerb().isCustomVerb());

    QString s = ial.getCommandLine();
    Q_ASSERT(s == "cust_Acton test://generic_document1 t test://generic_document2");

}

//Now, test an item that
void InputLanguageTester::testArgList2(){

    CatItem custVerb ("test://customVerb", "email");
    custVerb.setDescription("Send Email");
    custVerb.setItemType(CatItem::VERB);
    custVerb.setVerbDescription(
            " transmit ['$body:longtext+' ][title:'$title:shorttext+' ]"
            "to: $recepient:emailaddress++ $cc:longtext ");

    //custVerb.setCustomPluginInfo(VERB_COMMAND_STRING, "cust_Acton $body t $title");

    custVerb.setCustomCommandLine("cust_Acton $body t:'$title'");
    custVerb.setTakesAnykeys(true);

    CatItem noun1("test://generic_document1");
    CatItem noun2("test://generic_document2");
    CatItem noun3("test://generic_document3");

    CatItem stndVerb("test://generic_verb");
    stndVerb.setItemType(CatItem::VERB);

    CatItem tagOp("test://generic_tag");
    tagOp.setItemType(CatItem::OPERATION);

    InputList ial;

    //verb noun
    ial.setItem(custVerb);
    ial.addSlot();

    QList<CatItem> argItems = ial.getRemainingCustomArgs();

    Q_ASSERT(listContainsName(argItems, "body"));
    Q_ASSERT(listContainsName(argItems, "title"));
    Q_ASSERT(!listContainsName(argItems, "recepient"));

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.setItem(noun1);
    ial.addSlot();

    argItems = ial.getRemainingCustomArgs();

    Q_ASSERT(!listContainsName(argItems, "body"));
    Q_ASSERT(argItems[0].getName() == "title");
    Q_ASSERT(!listContainsName(argItems, "recepient"));

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    ial.setUserKeys("A Book Of Days");

    ial.setItem(argItems[0]);
    ial.addSlot();

    Q_ASSERT(ial.acceptItem(&noun1));
    Q_ASSERT(!ial.acceptItem(&stndVerb));
    Q_ASSERT(ial.acceptItem(&tagOp));

    argItems = ial.getRemainingCustomArgs();

    Q_ASSERT(!listContainsName(argItems, "body"));
    Q_ASSERT(!listContainsName(argItems, "title"));
    Q_ASSERT(listContainsName(argItems, "recepient"));

    //Q_ASSERT(ial.getVerb().isCustomVerb());

    QString s = ial.getCommandLine();
    Q_ASSERT(s == "cust_Acton test://generic_document1 t:'A Book Of Days'");

}


void InputLanguageTester::testInputLanguage(){

//    QFileInfo fileInfo("Send_Email2.oneline");
//    Q_ASSERT(fileInfo.exists());
//    QSettings set(fileInfo.absoluteFilePath(), QSettings::IniFormat);
//
//    CatItem custVerb(&set);
//    Q_ASSERT(custVerb.getName()=="email");
//    Q_ASSERT(custVerb.getDescription()=="Send Email");
//    Q_ASSERT(custVerb.getActionType() == CatItem::VERB_ACTION);
//
//    QString fieldGen = custVerb.getVerbDescription();
//    Q_ASSERT(!fieldGen.isEmpty());

    CatItem custVerb ("test://customVerb", "email");
    custVerb.setDescription("Send Email");
    custVerb.setItemType(CatItem::VERB);
    custVerb.setVerbDescription(" email ['$body:longtext+' ][title:'$title:shorttext' ]to: $recepient:emailaddress+ $cc:longtext ");
    custVerb.setCustomCommandLine("the body is:'$body'");

    VerbItem eb(custVerb);
    QList<CatItem> args = eb.getArguments();
    QString s = eb.getFormattedDescription();
    qDebug() << "formatted description: " << s;
    Q_ASSERT(!eb.error());
    Q_ASSERT(args.count() ==4);


    Q_ASSERT(s == "email to:");

    eb.addInput("title", "my post" );

    s = eb.getFormattedDescription();
    Q_ASSERT(s == "email title:'my post' to:");

    eb.addInput("body", "my message to you" , "/temp.txt");
    s = eb.getFormattedDescription();

    Q_ASSERT(s == "email 'my message to you' title:'my post' to:");
    s = eb.fillInFields("the body is:'$body'");
    Q_ASSERT(s == "the body is:'my message to you'");

    s = eb.getCommandLine();
    Q_ASSERT(s == "the body is:'/temp.txt'");

    CatItem arg1 = args[0];
    Q_ASSERT(arg1.getArgNumber()==0);
    Q_ASSERT(arg1.getName()=="body");
    QString argStr = arg1.getArgType();
    Q_ASSERT(argStr == LONG_TEXT_PATH);

    CatItem arg2 = args[1];
    Q_ASSERT(arg2.getArgNumber()==1);
    Q_ASSERT(arg2.getName()=="title");
    s = arg2.getArgType();
    Q_ASSERT(s== SHORT_TEXT_PATH);
    //QString t = arg2.argCommandString("test@test.com");
    //Q_ASSERT(t == "to: test@test.com");

    CatItem arg3 = args[2];
    Q_ASSERT(arg3.getArgNumber()==2);
    Q_ASSERT(arg3.getArgType()== EMAIL_ADDRESS_PATH);
    //Q_ASSERT(arg3.argCommandString("test text") == "title: test text");


}

void InputLanguageTester::testInputLanguage2(){


    CatItem custVerb ("test://customVerb", "facebook");
    custVerb.setDescription("Send Email");
    custVerb.setItemType(CatItem::VERB);
    custVerb.setVerbDescription(" facebook_message ['$body:longtext+' | 'no body' ]"
                                "[title:'$title:shorttext' | $body#20]to: $recepient:emailaddress+ $cc:longtext ");
    custVerb.setCustomCommandLine("the body is:'$body'");

    VerbItem eb(custVerb);
    QList<CatItem> args = eb.getArguments();
    QString s = eb.getFormattedDescription();
    qDebug() << "formatted description: " << s;
    Q_ASSERT(!eb.error());
    Q_ASSERT(args.count() ==4);


    Q_ASSERT(s == "facebook_message 'no body' to:");

    eb.addInput("title", "my post" );

    s = eb.getFormattedDescription();
    Q_ASSERT(s == "facebook_message 'no body' title:'my post' to:");

    eb.addInput("body", "my message to you" , "/temp.txt");
    s = eb.getFormattedDescription();

    Q_ASSERT(s == "facebook_message 'my message to you' title:'my post' to:");
    s = eb.fillInFields("the body is:'$body'");
    Q_ASSERT(s == "the body is:'my message to you'");

    s = eb.getCommandLine();
    Q_ASSERT(s == "the body is:'/temp.txt'");

    CatItem arg1 = args[0];
    Q_ASSERT(arg1.getArgNumber()==0);
    Q_ASSERT(arg1.getName()=="body");
    QString argStr = arg1.getArgType();
    Q_ASSERT(argStr == LONG_TEXT_PATH);

    CatItem arg2 = args[1];
    Q_ASSERT(arg2.getArgNumber()==1);
    Q_ASSERT(arg2.getName()=="title");
    s = arg2.getArgType();
    Q_ASSERT(s== SHORT_TEXT_PATH);
    //QString t = arg2.argCommandString("test@test.com");
    //Q_ASSERT(t == "to: test@test.com");

    CatItem arg3 = args[2];
    Q_ASSERT(arg3.getArgNumber()==2);
    Q_ASSERT(arg3.getArgType()== EMAIL_ADDRESS_PATH);
    //Q_ASSERT(arg3.argCommandString("test text") == "title: test text");


}

