#include "editremotedialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
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

	connect(model->controller(), &MountController::mountError,
			this, &MainWindow::mountError);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mountError(const QString &name, const QString &errorLog, int exitCode)
{
	reloadCurrent(ui->treeView->currentIndex());

	auto conf = DialogMaster::createCritical(tr("Failed to mount/unmount %1").arg(name));
	conf.title = conf.text;
	if(exitCode == -1){
		conf.text = tr("The mount or unmount operation failed! Check the details for the "
					   "generated error log");
	} else {
		conf.text = tr("The mount or unmount operation failed with exit code %1! Check the details for the "
					   "generated error log")
					.arg(exitCode);
	}
	conf.details = errorLog;
	DialogMaster::messageBox(conf);
}

void MainWindow::reloadCurrent(const QModelIndex &uiIndex)
{
	auto index = sortModel->mapToSource(uiIndex);
	ui->actionMount->setEnabled(index.isValid());

	if(index.isValid()) {
		auto mounted = model->isMounted(index);
		ui->actionEdit_Host->setEnabled(!mounted);
		ui->actionRemove_Host->setEnabled(!mounted);
		ui->actionMount->setChecked(mounted);
	} else {
		ui->actionEdit_Host->setEnabled(false);
		ui->actionRemove_Host->setEnabled(false);
		ui->actionMount->setChecked(false);
	}
}

void MainWindow::on_actionAdd_Host_triggered()
{
	auto info = EditRemoteDialog::editInfo({}, this);
	if(info.isValid())
		model->addMountInfo(info);
}

void MainWindow::on_actionEdit_Host_triggered()
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		auto info = EditRemoteDialog::editInfo(model->mountInfo(index), this);
		if(info.isValid())
			model->updateMountInfo(index, info);
	}
}

void MainWindow::on_actionRemove_Host_triggered()
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		if(DialogMaster::question(this, tr("Do you really want to remove the selected mount?")))
			model->removeMountInfo(index);
	}
}

void MainWindow::on_actionMount_triggered(bool checked)
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		if(checked)
			model->mount(index);
		else
			model->unmount(index);
	}
}

void MainWindow::on_actionOpen_Folder_triggered()
{
	auto index = sortModel->mapToSource(ui->treeView->currentIndex());
	if(index.isValid()) {
		auto info = model->mountInfo(index);
		QDesktopServices::openUrl(QUrl::fromLocalFile(info.localPath));
	}
}

void MainWindow::on_actionAbout_triggered()
{
	DialogMaster::about(this,
						tr("A gui wrapper around sshfs"),
						true,
						QUrl(QStringLiteral("https://github.com/Skycoder42")));
}
