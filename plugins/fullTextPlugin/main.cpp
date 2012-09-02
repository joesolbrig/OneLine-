#include <QtCore/QCoreApplication>
#include <QFile>
#include <qglobal.h>
#include "test.h"

#include "recoll_interface.h"

// Globals for atexit cleanup
//static ConfIndexer *confindexer;


bool PluginUpdater::stopindexing=false;
RclConfig *rclconfig=0;

// Global stop request flag. This is checked in a number of place in the
// indexing routines.
int stopindexing=0;

Recoll_Interface ri;

RclConfig* RclConfig::getMainConfig()
{
    return rclconfig;
}

bool listContainsPath(QList<RecollQueryItem> items, QString path){
    for(int i=0;i<items.count();i++){
        if(items[i].filePath == path){
            return true;
        }
    }
    return false;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    test1();
    test2();
    //return a.exec();
}

void test1(){
    QString db;
    QString error;
    ri.createDB(db, error);

    QString filePath1 = "./test1.txt";
    QString filePath2 = "./test2.txt";
    QString filePath3 = "./test3.txt";

    QDir baseDir(QDir::current() );
    filePath1 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath1));
    filePath2 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath2));
    filePath3 = baseDir.cleanPath(baseDir.absoluteFilePath(filePath3));

    QFile file1(filePath1);
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text)) { Q_ASSERT(FALSE); }
    qint64 c = file1.write(QByteArray("A batch foo bar"));
    file1.close();
    Q_ASSERT(c >0);


    QFile file2(filePath2);
    if (!file2.open(QIODevice::WriteOnly| QIODevice::Text)) { Q_ASSERT(FALSE); }
    c = file2.write(QByteArray("A batch foo "));
    Q_ASSERT(c >0);
    file2.close();

    QFile file3(filePath3);
    if (!file3.open(QIODevice::WriteOnly| QIODevice::Text)) { Q_ASSERT(FALSE); }
    c = file3.write(QByteArray("A batch bar"));
    Q_ASSERT(c >0);
    file3.close();

    CatItem i1(filePath1);
    ri.openDB(true);
    bool sucess = ri.addPossiblyCompoundFile(i1);
    //bool sucess = ri.addFile_Old(filePath1);
    Q_ASSERT(sucess);
    CatItem i2(filePath2);
    sucess = ri.addPossiblyCompoundFile(i2);
    //sucess = ri.addFile_Old(filePath2);
    Q_ASSERT(sucess);
    CatItem i3(filePath3);
    sucess = ri.addPossiblyCompoundFile(i3);
    //sucess = ri.addFile_Old(filePath1);
    Q_ASSERT(sucess);
    ri.closeDB();

    QList<RecollQueryItem> results;
    QString q("batch foo");
    int i = ri.getMatches(q, "", results, 10, error);
    QString dummy = error;
    qDebug() << dummy;

    Q_ASSERT(listContainsPath(results,filePath1));
    Q_ASSERT(listContainsPath(results,filePath2));
    Q_ASSERT(listContainsPath(results,filePath3));
    Q_ASSERT(results.count()>2);

    results.clear();
    i = ri.getMatches(q, "", results, 10, error, true);
    dummy = error;
    qDebug() << dummy;

    Q_ASSERT(listContainsPath(results,filePath1));
    Q_ASSERT(listContainsPath(results,filePath2));
    Q_ASSERT(listContainsPath(results,filePath3));
    Q_ASSERT(results.count()>1);


}
