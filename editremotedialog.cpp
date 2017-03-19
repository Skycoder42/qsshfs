#include "editremotedialog.h"
#include "ui_editremotedialog.h"
#include <QDebug>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QPushButton>
#include <dialogmaster.h>

MountInfo EditRemoteDialog::editInfo(const MountInfo &oldInfo, QWidget *parent)
{
	EditRemoteDialog dialog(parent);

	if(oldInfo.isValid()) {
		dialog.ui->nameLineEdit->setText(oldInfo.name);
		dialog.ui->hostnameComboBox->setCurrentText(oldInfo.hostName);
		dialog.ui->userLineEdit->setText(oldInfo.userOverwrite);
		dialog.ui->remoteMountpointLineEdit->setText(oldInfo.remotePath);
		dialog.ui->localMountpointPathEdit->setPath(oldInfo.localPath);
	}

	if(dialog.exec() == QDialog::Accepted) {
		MountInfo info;
		info.name = dialog.ui->nameLineEdit->text();
		info.hostName = dialog.ui->hostnameComboBox->currentText();
		info.userOverwrite = dialog.ui->userLineEdit->text();
		info.remotePath = dialog.ui->remoteMountpointLineEdit->text();
		if(info.remotePath.isEmpty())
			info.remotePath = QStringLiteral("/");
		info.localPath = dialog.ui->localMountpointPathEdit->path();
		if(info.localPath.isEmpty())
			info.localPath = dialog.ui->localMountpointPathEdit->placeholder();
		return info;
	} else
		return {};
}

EditRemoteDialog::EditRemoteDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditRemoteDialog)
{
	DialogMaster::masterDialog(this, true);
	ui->setupUi(this);

	QDir home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	readSshConfig(QStringLiteral("/etc/ssh/ssh_config"));
	readSshConfig(home.absoluteFilePath(QStringLiteral(".ssh/config")));

	home.mkdir(QStringLiteral("mnt"));
	ui->localMountpointPathEdit->setDefaultDirectory(home.absoluteFilePath(QStringLiteral("mnt")));

	on_nameLineEdit_textChanged({});
	on_hostnameComboBox_editTextChanged(ui->hostnameComboBox->currentText());
}

EditRemoteDialog::~EditRemoteDialog()
{
	delete ui;
}

void EditRemoteDialog::on_nameLineEdit_textChanged(const QString &text)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void EditRemoteDialog::on_hostnameComboBox_editTextChanged(const QString &text)
{
	QDir home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	auto path = home.absoluteFilePath(QStringLiteral("mnt/") + text);
	ui->localMountpointPathEdit->setPlaceholder(path);
}

void EditRemoteDialog::on_editConfigButton_clicked()
{
	QDir home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	QDesktopServices::openUrl(QUrl::fromLocalFile(home.absoluteFilePath(QStringLiteral("./.ssh/config"))));
}

void EditRemoteDialog::readSshConfig(const QString &fileName)
{
	QFile sshConfig(fileName);
	if(sshConfig.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream stream(&sshConfig);
		QString line;
		while(stream.readLineInto(&line)) {
			if(line.startsWith(QStringLiteral("Host ")))
				ui->hostnameComboBox->addItem(line.mid(5).trimmed());
		}
		sshConfig.close();
	} else {
		DialogMaster::warning(parentWidget(),
							  tr("Failed to read ssh config file \"%1\" with error: \"%2\"\n"
								 "Make shure the file exists and is readable by this user!")
							  .arg(fileName)
							  .arg(sshConfig.errorString()));
	}
}
