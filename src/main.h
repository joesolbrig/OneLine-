#ifndef THE_APP_MAIN
#include "is_test.h"
#define THE_APP_MAIN
#include <QKeyEvent>
#include "globals.h"
#include "marginwindow.h"
#include "constants.h"

#ifndef USE_MARGIN_WINDOW
#define BASE_WINDOW_TYPE MainUserWindow
#else
#define BASE_WINDOW_TYPE MarginWindow
#endif

#ifndef USE_THREAD_POOL
#define RUNNING_THREAD QRunnable
#define START_THREAD(X,Y) QThreadPool::globalInstance()->start(X,Y)
#else
#define RUNNING_THREAD QThread
#define START_THREAD(X,Y) X->start(Y)
#endif

#include "marginwindow.h"
#include "appearance_window.h"

inline QRect gAppArea(){
#ifndef USE_MARGIN_WINDOW
    gGlobalArea();
#else
    Q_ASSERT(gMarginWidget);
    return gMarginWidget->geometry();
#endif
}


inline QRect gGlobalArea(){
    int scrnNum;
    if(UI_USE_PRIMARY_SCRN){
        scrnNum = qApp->desktop()->primaryScreen();
    } else {
        scrnNum = UI_SCRN_NUMBER;
    }

    QRect scr = qApp->desktop()->screenGeometry(scrnNum);
//    if(UI_RANGEOVER_ENTIRE_SCREEN){
//        scr = qApp->desktop()->screenGeometry(scrnNum);
//    } else {
//        scr = qApp->desktop()->availableGeometry(scrnNum);
//    }
    return scr;
}

inline void gSetAppPos(int vOrient, int hOrient){
#ifndef USE_MARGIN_WINDOW
    vOrient = vOrient;
    hOrient = hOrient;
    return;
#else
    gMarginWidget->gSetAppPos(vOrient,hOrient);
#endif
}


class KeyRouter : public QObject {
     Q_OBJECT

 protected:
     bool eventFilter(QObject *obj, QEvent *event);
 };

QString getBaseDir(QApplication * app);



#endif
