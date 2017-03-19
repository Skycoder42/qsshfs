#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->treeView->setParent(this);
	centralWidget()->deleteLater();
	setCentralWidget(ui->treeView);
}

MainWindow::~MainWindow()
{
	delete ui;
}
