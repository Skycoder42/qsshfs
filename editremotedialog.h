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

private:
	Ui::EditRemoteDialog *ui;

	explicit EditRemoteDialog(QWidget *parent = nullptr);
	~EditRemoteDialog();
};

#endif // EDITREMOTEDIALOG_H
