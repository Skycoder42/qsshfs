#ifndef MOUNTDIALOG_H
#define MOUNTDIALOG_H

#include "mountinfo.h"

#include <QDialog>
#include <QProcess>

namespace Ui {
class MountDialog;
}

class MountDialog : public QDialog
{
	Q_OBJECT

public:
	static bool mount(const MountInfo &info, QWidget *parent = nullptr);
	static bool unmount(const MountInfo &info, QWidget *parent = nullptr);

private slots:
	void readReady();
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

	void on_hideInputButton_clicked(bool checked);

	void on_inputEdit_returnPressed();

private:
	Ui::MountDialog *ui;
	QProcess *process;

	QByteArray procBuffer;
	bool success;

	explicit MountDialog(QWidget *parent = nullptr);
	~MountDialog();
};

#endif // MOUNTDIALOG_H
