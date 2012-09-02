#include "actionpreview.h"
#include "ui_actionpreview.h"

ActionPreview::ActionPreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionPreview)
{
    ui->setupUi(this);
}

ActionPreview::~ActionPreview()
{
    delete ui;
}

void ActionPreview::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
