#include "editremotedialog.h"
#include "ui_editremotedialog.h"

EditRemoteDialog::EditRemoteDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditRemoteDialog)
{
	ui->setupUi(this);
}

EditRemoteDialog::~EditRemoteDialog()
{
	delete ui;
}
