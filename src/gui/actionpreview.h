#ifndef ACTIONPREVIEW_H
#define ACTIONPREVIEW_H

#include <QDialog>

namespace Ui {
    class ActionPreview;
}

class ActionPreview : public QDialog {
    Q_OBJECT
public:
    ActionPreview(QWidget *parent = 0);
    ~ActionPreview();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ActionPreview *ui;
};

#endif // ACTIONPREVIEW_H
