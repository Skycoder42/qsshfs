#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QSystemTrayIcon>
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
	void updateAutostart(bool checked);

	void on_actionAdd_Host_triggered();
	void on_actionEdit_Host_triggered();
	void on_actionRemove_Host_triggered();
	void on_actionMount_triggered(bool checked);
	void on_actionOpen_Folder_triggered();
	void on_actionAbout_triggered();

private:
	Ui::MainWindow *ui;
	MountModel *model;
	QSortFilterProxyModel *sortModel;

	QSystemTrayIcon *trayIco;

	bool isAutostart();
};

#endif // MAINWINDOW_H
