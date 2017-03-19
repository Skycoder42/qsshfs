#ifndef EDITREMOTEDIALOG_H
#define EDITREMOTEDIALOG_H

#include "mountinfo.h"

#include <QDialog>

namespace Ui {
class EditRemoteDialog;
}

class EditRemoteDialog : public QDialog
{
	Q_OBJECT

public:
	static MountInfo editInfo(const MountInfo &oldInfo = {}, QWidget *parent = nullptr);

private slots:
	void on_nameLineEdit_textChanged(const QString &text);
	void on_hostnameComboBox_editTextChanged(const QString &text);
	void on_editConfigButton_clicked();


private:
	Ui::EditRemoteDialog *ui;

	explicit EditRemoteDialog(QWidget *parent = nullptr);
	~EditRemoteDialog();

	void readSshConfig(const QString &fileName);
};

#endif // EDITREMOTEDIALOG_H
