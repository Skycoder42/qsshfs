#include "editremotedialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <dialogmaster.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	model(new MountModel(this)),
	sortModel(new QSortFilterProxyModel(this))
{
	ui->setupUi(this);
	ui->treeView->setParent(this);
	centralWidget()->deleteLater();
	setCentralWidget(ui->treeView);

	sortModel->setSourceModel(model);
	ui->treeView->setModel(sortModel);

	connect(ui->actionExit, &QAction::triggered,
			qApp, &QApplication::quit);
	connect(ui->actionAbout_Qt, &QAction::triggered,
			qApp, &QApplication::aboutQt);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::reloadCurrent);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reloadCurrent(const QModelIndex &index)
{
	ui->actionEdit_Host->setEnabled(index.isValid());

	if(index.isValid()) {
		auto mounted = model->isMounted(index);
		ui->actionRemove_Host->setEnabled(!mounted);
		ui->actionMount->setEnabled(!mounted);
		ui->actionMount->setChecked(mounted);
	} else {
		ui->actionRemove_Host->setEnabled(false);
		ui->actionMount->setEnabled(false);
		ui->actionMount->setChecked(false);
	}
}

void MainWindow::on_actionAdd_Host_triggered()
{
	auto info = EditRemoteDialog::editInfo({}, this);
	if(info.isValid())
		model->addMount(info);
}

void MainWindow::on_actionEdit_Host_triggered()
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		auto info = EditRemoteDialog::editInfo(model->mount(index), this);
		if(info.isValid())
			model->updateMount(index, info);
	}
}

void MainWindow::on_actionRemove_Host_triggered()
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		if(DialogMaster::question(this, tr("Do you really want to remove the selected mount?")))
			model->removeMount(index);
	}
}
