#include "editremotedialog.h"
#include "ui_editremotedialog.h"

MountInfo EditRemoteDialog::editInfo(const MountInfo &oldInfo, QWidget *parent)
{
	EditRemoteDialog dialog(parent);

	if(oldInfo.isValid()) {
		dialog.ui->nameLineEdit->setText(oldInfo.name);
		dialog.ui->hostnameComboBox->setCurrentText(oldInfo.hostName);
		dialog.ui->changeUserLineEdit->setText(oldInfo.userOverwrite);
		dialog.ui->remoteMountpointLineEdit->setText(oldInfo.remotePath);
		dialog.ui->localMountpointPathEdit->setPath(oldInfo.localPath, true);
	}

	if(dialog.exec() == QDialog::Accepted) {
		//TODO
		return oldInfo;
	} else
		return {};
}

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
