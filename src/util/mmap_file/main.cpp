

#include "tree_struct_test.h"

//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#include "kchashdb.h"
//#pragma GCC diagnostic pop

//int mmexample();


#include <QtCore/QCoreApplication>
#include <QDebug>

//#include "btree.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    test3();
//    test4();
    //test_tdb_load_save();
    //mmexample();

    qDebug() << "tests started";
    test_pointer_load_save();
    qDebug() << "test ptr load done";
    test2();
    qDebug() << "test2 done again";
    test2_quickRestore();
    qDebug() << "test2 done";
    //test1();
    //test_mem2();
    //return a.exec();
}

