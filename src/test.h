#ifndef TEST_H
#define TEST_H

#include <QtTest/QtTest>
#include <QTest>
#include "appearance_window.h"

bool listContainsItem(QList<CatItem> items, CatItem it);

class TheApplicationTester : public QObject {
        Q_OBJECT
     public:
        TheApplicationTester();
        ~TheApplicationTester();
        MainUserWindow* lWind;

        //Catalog* test_cat;
        ThreadManager* the_test_builder;
        InputList dummyInputList;

        void verifyGotResultFromKeys(QString keys, QString result);
        void verifyChildItem(QString keys, QString result);
        void sendKeysAndWait(QString str);

        void subTestTextEntry();

        void subTestListBoxNav(int offset=0);
        void subTestTabFromItem();
        void subTestDoAction();
        void subTestUpdateUsage();
        void subTestTextFormatting();
        void subTestCustomVerb();
        void subTestTagOperation();

        void sub_test_ItemTrie(int loop_var);

        bool verifyCatHasItemName(QString name);
        bool verifyBoxContainsName(QString name);
        void clearEntry();
        QList<CatItem> backgroundFind(QString userKeys, QStringList keywords );

        void loopForProfiling();

        //---------------------

        //stream has its own tests which should be integrated soon...
        //In a bit...

        //hide place.....................................
        void testInputListAsHash(); //Out of date

        void testGuiIntegration(); // must follow something that loads catalog
        void testGui();
        void subtestGui();

        void testXslPlugin();
        void testXslPlugin2();

        void testCreateFacebookSources();
        void testMultiPartJson();
        void test_FindNamesWitSpaces();

        void testFilterRoutines();

        void testTreeCompress();
        void testItemFormatRoutine();

        void testLongEdit();
        void testTest();//does nothing

        void test_ItemTrie1();
        void testOHash();

        void testInformationFormat1();

        void testFilePlugin();
        void testSortRoutines();
        void testComparisonRoutines();
        void testCreateCustomArg();
        void testCreateCustomArg2();
        void testCreateCustomArg3();

        void testFireFoxPlugin();
        void testParentChildDBAdd();
        void test_ItemTrie2();
        void test_cat_store2();
        void testCoalateItems2();
        void BatchPseudoTes_ALL_email();

        void testPeopleFilterCombine();

        void testExtendFacebookSources();

        void test_MultiTree2();
        void test_MultiTree1();
        void test_cat_store();

        void testUpdateUsage2();
        void testUpdateUsage();


    public slots:
        //Support functions, tests are private slots
        void extensionCycler(QList<CatItem> inList, QList<CatItem>* outList);

    //We swap functions in and out of below for custom tests...
    private slots:
        //working tests
        //not-used functions go above "'slots:" when we want to hide them...
        void testFileFind();


    };

#endif // TEST_H
