#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "mountmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void mountError(const QString &name, const QString &errorLog, int exitCode);

	void reloadCurrent(const QModelIndex &uiIndex);

	void on_actionAdd_Host_triggered();
	void on_actionEdit_Host_triggered();
	void on_actionRemove_Host_triggered();
	void on_actionMount_triggered(bool checked);

private:
	Ui::MainWindow *ui;
	MountModel *model;
	QSortFilterProxyModel *sortModel;
};

#endif // MAINWINDOW_H
