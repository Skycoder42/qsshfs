#ifndef EDITREMOTEDIALOG_H
#define EDITREMOTEDIALOG_H

#include <QDialog>

namespace Ui {
class EditRemoteDialog;
}

class EditRemoteDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EditRemoteDialog(QWidget *parent = 0);
	~EditRemoteDialog();

private:
	Ui::EditRemoteDialog *ui;
};

#endif // EDITREMOTEDIALOG_H
