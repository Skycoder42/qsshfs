#include "editremotedialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QSettings>
#include <dialogmaster.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	model(new MountModel(this)),
	sortModel(new QSortFilterProxyModel(this)),
	trayIco(new QSystemTrayIcon(windowIcon(), this))
{
	ui->setupUi(this);
	ui->treeView->setParent(this);
	centralWidget()->deleteLater();
	setCentralWidget(ui->treeView);

	sortModel->setSourceModel(model);
	ui->treeView->setModel(sortModel);

	trayIco->setToolTip(QApplication::applicationDisplayName());
	auto menu = new QMenu(this);
	menu->addAction(QIcon::fromTheme(QStringLiteral("window-new")), tr("Show main window"),
					this, &MainWindow::show);
	menu->addMenu(model->createMountMenu(menu));
	menu->addSeparator();
	auto runAction = menu->addAction(QIcon::fromTheme(QStringLiteral("games-config-options")), tr("Keep running"),
									 qApp, [](bool triggered){
		QApplication::setQuitOnLastWindowClosed(!triggered);
	});
	runAction->setCheckable(true);
	auto startAction = menu->addAction(QIcon::fromTheme(QStringLiteral("system-run")), tr("Autostart"),
									   this, &MainWindow::updateAutostart);
	startAction->setCheckable(true);
	menu->addAction(QIcon::fromTheme(QStringLiteral("gtk-quit")), tr("Quit"),
					qApp, &QApplication::quit);
	trayIco->setContextMenu(menu);
	trayIco->setVisible(true);

	QSettings settings;
	settings.beginGroup(QStringLiteral("gui"));
	restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	restoreState(settings.value(QStringLiteral("state")).toByteArray());
	ui->treeView->header()->restoreState(settings.value(QStringLiteral("header")).toByteArray());
	runAction->setChecked(settings.value(QStringLiteral("background"), true).toBool());
	startAction->setChecked(isAutostart());
	QApplication::setQuitOnLastWindowClosed(!runAction->isChecked());
	settings.endGroup();

	connect(ui->actionExit, &QAction::triggered,
			qApp, &QApplication::quit);
	connect(ui->actionAbout_Qt, &QAction::triggered,
			qApp, &QApplication::aboutQt);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::reloadCurrent);

	connect(model->controller(), &MountController::mountError,
			this, &MainWindow::mountError);

	connect(qApp, &QApplication::commitDataRequest,
			this, &MainWindow::commitShutdown);
	connect(qApp, &QApplication::saveStateRequest,
			this, &MainWindow::commitShutdown);
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("gui"));
	settings.setValue(QStringLiteral("geom"), saveGeometry());
	settings.setValue(QStringLiteral("state"), saveState());
	settings.setValue(QStringLiteral("header"), ui->treeView->header()->saveState());
	settings.setValue(QStringLiteral("background"), !QApplication::quitOnLastWindowClosed());
	settings.endGroup();

	delete ui;
}

void MainWindow::mountError(const QString &name, const QString &errorLog, int exitCode)
{
	reloadCurrent(ui->treeView->currentIndex());

	auto conf = DialogMaster::createCritical(tr("Failed to mount/unmount %1").arg(name));
	conf.parent = isVisible() ? this : nullptr;
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

void MainWindow::updateAutostart(bool checked)
{
	auto resPath = QStringLiteral("%1/autostart/%2.sh")
					.arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
					.arg(QCoreApplication::applicationName());
	if(checked) {
		QFile file(resPath);
		if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			file.write(QStringLiteral("#!/bin/sh\n%1 --hidden")
					   .arg(QCoreApplication::applicationName())
					   .toUtf8());
			file.close();
			file.setPermissions(file.permissions() | QFileDevice::ExeUser);
		}
	} else
		QFile::remove(resPath);
}

void MainWindow::commitShutdown(QSessionManager &sm)
{
	auto args = sm.restartCommand();
	if(!isVisible()) {
		if(!args.contains(QStringLiteral("--hidden")))
			args.append(QStringLiteral("--hidden"));
	} else
		args.removeAll(QStringLiteral("--hidden"));
	sm.setRestartCommand(args);
	sm.setRestartHint(QSessionManager::RestartIfRunning);
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

bool MainWindow::isAutostart()
{
	auto resPath = QStringLiteral("%1/autostart/%2.sh")
					.arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
					.arg(QCoreApplication::applicationName());
	return QFile::exists(resPath);
}
