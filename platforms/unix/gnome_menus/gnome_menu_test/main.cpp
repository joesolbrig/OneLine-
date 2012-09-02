#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTimer>
#include "process_menu.h"

void test1(){
    QList<CatItem> res = parseXdgMenus();
    for(int i=0; i< res.count();i++)
        { res[i].printForDebug(); }
}



//Minimal code to start QObject machinery
class Tester: public QObject
{
    Q_OBJECT

public:
    Tester(){}
    ~Tester(){}

public slots:
    void execute();
};

void Tester::execute(){
    test1();
    QCoreApplication::instance()->quit();

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Tester tester;
    QTimer::singleShot(0, &tester, SLOT(execute()));
    return a.exec();

}

#include "main.moc"
