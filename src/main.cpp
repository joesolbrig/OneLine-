/*
Oneline: Multibrowser
Copyright (C) 2012  Hans Joseph Solbrig

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

using namespace std;

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


#include "icon_delegate.h"
#include "edit_controls.h"
#include "main.h"
#include "globals.h"
#include "options.h"
//#include "dsingleapplication.h"
#include "plugin_interface.h"
#include "test.h"


bool KeyRouter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent* kEvent = (QKeyEvent*) event;
        //Qt::KeyboardModifiers m = kEvent->modifiers();
        int k = kEvent->key();
        if(gMainWidget && gMainWidget->st_altKeyDown){
            qDebug() << "got release key" << QChar(k);
            gMainWidget->clearModifierKeys();
        }
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* kEvent = (QKeyEvent*) event;
        Qt::KeyboardModifiers m = kEvent->modifiers();
        int k = kEvent->key();
        qDebug() << "got key" << QChar(k);
        if(gMainWidget ){ //test always taking this... was && gMainWidget!=obj
            if(gMainWidget->takeAllText()){
                gMainWidget->setFocus();
                //return QObject::eventFilter(gMainWidget, event);
                if(gMainWidget->keyPressed(kEvent)) {
                    return true;
                }
            } else {
                if(!kEvent->text().isEmpty() ||(m == Qt::NoModifier)||(m == Qt::ShiftModifier)) {
                    return QObject::eventFilter(obj, event);
                }

                if( !((m == Qt::NoModifier)||(m == Qt::ShiftModifier))){
                    if(gMainWidget->keyPressed(kEvent)) {
                        return true;
                    }
                }

                if( m == Qt::ControlModifier && (k == Qt::UpArrow || k == Qt::DownArrow)){
                    if(gMainWidget->keyPressed(kEvent)) {
                        return true;
                    }
                }
            }
            gMainWidget->setFocus();
        } else {
            qDebug() << "not yet ready for key events...";
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

QString getBaseDir(QApplication * app){

    QStringList args = app->arguments();
    for(int i=0; i< args.count(); i++){
        if(args[i].length()>2 && (args[i])[0]=='-' && (args[i])[1]=='d'){
            QString path = args[i].mid(2);
            return path;
        }
    }
    return DEFAULT_APP_DIR;
}


int main(int argc, char *argv[]) {

//This is done because of the order in which QT and Gnome need to be loaded.
#ifdef Q_WS_WIN
	QApplication * app = new QApplication(argc, argv);
#endif
    PlatformBase* plat = loadPlatform();
    gPlatform = plat;


#ifdef Q_WS_X11
    QApplication * app = plat->init(argc, argv);
#endif

    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_NAME);
    QString baseDir = getBaseDir(app);
    QString iniBase = QDir::homePath() + "/" + baseDir + "/oneline.ini";
    qDebug() << "main:: loading: " << iniBase;
    gSettings = new QSettings(iniBase, QSettings::IniFormat);

    QHash<QString, QList<QString> > theDirs = (plat)->GetDirectories(baseDir,
                                                                     gSettings);
    qDebug() << "----------------" ;
    qDebug() << "got dirs:" << theDirs;
    qDebug() << "----------------" ;


    qDebug() << "config dir: " << theDirs.value("config")[0];
    gDirs = &theDirs;
    app->setQuitOnLastWindowClosed(false);
    bool rescue = false;

    if (plat->isAlreadyRunning()) {
        qDebug() << "application already running, exiting";
        exit(1);
    }

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("tr/online_" + locale));
    app->installTranslator(&translator);

    BASE_WINDOW_TYPE baseWin(NULL, gPlatform, rescue);
    KeyRouter kr;
    QCoreApplication* apinst =  QCoreApplication::instance();
    Q_ASSERT(apinst);
    apinst->installEventFilter(&kr);

#ifdef TEST
    TheApplicationTester t;
    QTest::qExec(&t);
    qDebug() << "tests finished";
    app->closeAllWindows();
    qDebug() << "windows close";
    app->quit();
    qDebug() << "quit, now to delete app";
#else
    //Don't do times threads and timers unless not testing...
    gMainWidget->startTasks();
    gMainWidget->show();
    app->exec();

#endif
    // Just like qt itself, don't bother deleting memory here that will be freed by the system anyway
    // CatBuilder::destroyCatalog();
    // delete app;
} 
