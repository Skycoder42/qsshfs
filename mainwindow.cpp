#include "editremotedialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionAdd_Host_triggered()
{
	auto info = EditRemoteDialog::editInfo({}, this);
	if(info.isValid()) {

	}
}
