#include "mountdialog.h"
#include "ui_mountdialog.h"
#include <dialogmaster.h>
#include <QPushButton>
#include <QDebug>

MountDialog::MountDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MountDialog),
	process(new QProcess(this)),
	procBuffer(),
	success(false)
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this);

	process->setProcessChannelMode(QProcess::MergedChannels);

	auto env = QProcessEnvironment::systemEnvironment();
	if(env.value(QStringLiteral("SSH_ASKPASS")).simplified().isEmpty())
		env.insert(QStringLiteral("SSH_ASKPASS"), QStringLiteral("/usr/bin/ksshaskpass"));
	process->setEnvironment(env.toStringList());

	connect(process, &QProcess::readyRead,
			this, &MountDialog::readReady);
	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			this, &MountDialog::finished);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
}

MountDialog::~MountDialog()
{
	delete ui;
}

bool MountDialog::mount(const MountInfo &info, QWidget *parent)
{
	MountDialog dialog(parent);
	dialog.setWindowTitle(tr("Mount %1").arg(info.name));

	QDir(info.localPath).mkpath(QStringLiteral("."));

	dialog.process->setProgram(QStringLiteral("/usr/bin/sshfs"));
	QStringList args;
	args.append(info.hostName + QLatin1Char(':') + info.remotePath);
	args.append(info.localPath);
	dialog.process->setArguments(args);

	dialog.process->start();
	dialog.exec();
	return dialog.success;
}

bool MountDialog::unmount(const MountInfo &info, QWidget *parent)
{
	MountDialog dialog(parent);
	dialog.setWindowTitle(tr("Unmount %1").arg(info.name));

	dialog.process->setProgram(QStringLiteral("fusermount"));
	QStringList args;
	args.append(QStringLiteral("-u"));
	args.append(info.localPath);
	dialog.process->setArguments(args);
	dialog.process->start();

	dialog.exec();

	if(dialog.success)
		QDir(info.localPath).rmdir(QStringLiteral("."));

	return dialog.success;
}

void MountDialog::readReady()
{
	procBuffer.append(process->readAll());
	ui->outputBrowser->setPlainText(QString::fromUtf8(procBuffer));
}

void MountDialog::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitStatus != QProcess::NormalExit) {
		success = false;
	} else {
		success = exitCode == EXIT_SUCCESS;
		if(success)
			this->accept();
		else
			ui->outputBrowser->append(QStringLiteral("\nCommand failed with exit code %1").arg(exitCode));
	}

	ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(false);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
}

void MountDialog::on_hideInputButton_clicked(bool checked)
{
	ui->inputEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}

void MountDialog::on_inputEdit_returnPressed()
{
	process->write(ui->inputEdit->text().toUtf8() + '\n');
	ui->inputEdit->clear();
}
