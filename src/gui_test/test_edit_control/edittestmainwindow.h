#ifndef EDITTESTMAINWINDOW_H
#define EDITTESTMAINWINDOW_H

#define USING_ITEM_HASH_CLASS
#define USING_ITEM_HASH_CLASS
#define USING_STRING_HASH

#include <QMainWindow>
#include <QMessageBox>
#include "list_item.h"
#include "fancylabel.h"
#include "previewpane.h"


namespace Ui {
    class EditTestMainWindow;
}

class EditTestMainWindow : public QMainWindow {
    Q_OBJECT
public:
    EditTestMainWindow(QWidget *parent = 0);
    void createTransparentWindows();
    void FillItemList(){

    }

    ~EditTestMainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditTestMainWindow *m_ui;

    FancyLabel* m_fancyLabel;
    QRect m_initialListGeo;
    PreviewPane* m_previewPane;

protected slots:

    void loadHasFinished(bool f){
        QMessageBox msgBox;
        QString msg= QString("loaded with %1").arg((int)f);
        msgBox.setText(msg);
        msgBox.exec();

        m_previewPane->update();

    }

public slots:

    void addParentItems();
    void addChildItems();

    void expandListWindow();
    void shrinkListWindow();

};

#endif // EDITTESTMAINWINDOW_H
