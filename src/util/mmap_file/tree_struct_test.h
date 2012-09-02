#ifndef TREE_STRUCTURE_TEST_H
#define TREE_STRUCTURE_TEST_H


#ifndef INT_CONSTANT_KLUGDE
#define INT_CONSTANT_KLUGDE
//For some screwy config reason these aren't defined where they ought to be...
/* Minimum of signed integral types.  */
# define INT8_MIN		(-128)
# define INT16_MIN		(-32767-1)
# define INT32_MIN		(-2147483647-1)
# define INT64_MIN		(2147483647)
/* Maximum of signed integral types.  */
# define INT8_MAX		(127)
# define INT16_MAX		(32767)
# define INT32_MAX		(2147483647)
# define INT64_MAX		(2147483647)


# define UINT8_MAX		(255)
# define UINT16_MAX		(65535)
# define UINT32_MAX		(4294967295U)
# define UINT64_MAX		(4294967295U)
#endif

#ifndef DEFAULTE_PAGE_COUNT
#define DEFAULTE_PAGE_COUNT 40
#endif

#include <QString>
#include <QHash>
#include <QTextStream>

class TestStringContainer {

    QString val;

public:

    TestStringContainer(){ val = ""; }
    explicit TestStringContainer(QString v){val = v;}
    TestStringContainer(const TestStringContainer& v){val = v.val;}
    QString getId() const { return (val);}
    qint32 getItemId() const { return qHash(val);}

    void setVal(QString s){val = s;}
    bool operator ==(TestStringContainer v2){ return (val == v2.val);}
    bool operator ==(QString v2){ return (val == v2); }
    bool operator > (QString v2){ return (val > v2); }
    bool operator < (QString v2){ return (val <  v2); }

    TestStringContainer& operator =(const TestStringContainer v2){
        val = v2.val;
        return *this;
    }

//    TestStringContainer& operator =(const QString v2){
//        val = v2;
//        return *this;
//    }

    bool isEmpty(){return (
            val.length() == 0);
    }
    bool merge(TestStringContainer& t){
        val = t.val;
        return true;
    }

    static QString typeName(){return QString("TestStringContainer");}

};

inline QDataStream &operator<<(QDataStream &out, const TestStringContainer &item) {

    QString v = item.getId();
    out << v;
    return out;
}


inline QDataStream &operator>>(QDataStream &in, TestStringContainer &item) {

    QString v;
    in >> v;
    //v.detach();
    item.setVal(v);
    return in;
}

inline QTextStream &operator<<(QTextStream &out, const TestStringContainer &item) {

    out << item.getId();

    return out;
}

inline QTextStream &operator>>(QTextStream &in, TestStringContainer &item) {

    QString v;
    v = in.readLine();
    v.detach();
    item.setVal(v);
    return in;
}

#ifndef DEFAULT_KC_FILE
#define DEFAULT_KC_FILE "test_db.kc"
#endif

#include <QHash>
#include <QString>
#include "stdint.h"

inline uint stringHash(QString str){
    return qHash(str);

}
void test1();
void test2();
void test2_quickRestore();
void test3();
void test4();
void test_mem1();
void test_mem2();
void test_pointer_load_save();
//void test_tdb_load_save();
#endif // TEST_H
