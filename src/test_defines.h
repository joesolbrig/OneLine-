#ifndef TEST_DEFINES_H
#define TEST_DEFINES_H

//const int ITEM_LIMIT=1000; //000;
//Make this BIG for a FULL test...
const int ITEM_LIMIT=200000; //000;


static const char* const unfoundStr = "XYZunfound";
static const char* const longMessageStr = "fasdf asdf ls kgsdf asdf lsdf kgsdf asdf lsdfkgsdf asdf lsdf";
static const char* const testEmailAddressStr = "oneline@gmail.com";
static const char* const testVerbStr = "IamAVerb";
static const char* const testNounStr = "testNounItem";
static const char* const testCustomVerbStr = "ACustomVerbIam";
#define str_len 3
//Mis-spelling on purpose for uniqueness!
static const char* const str_list[str_len][3] = {
    {"xyz","xyzItem", "<font color=\"yellow\">xyz</font>Item"},
    {"yob","YobTube", "<font color=\"yellow\">Yob</font>Tube"},
    {"aboat","aboatUs", "<font color=\"yellow\">aboat</font>Us"}
};

#define fill_len 9

#ifdef TEST_GUI
//Third field doesn't matter here
static const char* fill_list[fill_len][3] = {
    {"xyz","xyzItem", "<font color=\"yellow\">xyz</font>Item"},
    {"yob","YobTube", "<font color=\"yellow\">Yob</font>Tube"},
    {"aboat","aboatUs", "<font color=\"yellow\">aboat</font>Us"},
    {"pdq","pdqItem", "<font color=\"yellow\">xyz</font>Item"},
    {"bln","bobBube", "<font color=\"yellow\">Yob</font>Tube"},
    {"cboat","coatCUs", "<font color=\"yellow\">aboat</font>Us"},
    {"dyz","dxyzDItem", "<font color=\"yellow\">xyz</font>Item"},
    {"eob","EobETube", "<font color=\"yellow\">Yob</font>Tube"},
    {"fboat","FboatFucked", "<font color=\"yellow\">aboat</font>Us"}
};
#endif


#endif // TEST_DEFINES_H
