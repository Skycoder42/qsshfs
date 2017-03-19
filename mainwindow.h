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
	void reloadCurrent(const QModelIndex &index);

	void on_actionAdd_Host_triggered();
	void on_actionEdit_Host_triggered();
	void on_actionRemove_Host_triggered();

private:
	Ui::MainWindow *ui;
	MountModel *model;
	QSortFilterProxyModel *sortModel;
};

#endif // MAINWINDOW_H
