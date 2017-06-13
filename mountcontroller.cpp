#include "mountcontroller.h"

#include <QDir>
#include <QDebug>

MountController::MountController(QObject *parent) :
	QObject(parent),
	_mounts()
{}

MountInfo MountController::mountInfo(const QString &name) const
{
	return _mounts.value(name).info;
}

bool MountController::isMounted(const QString &name)
{
	return _mounts.value(name).mounted;
}

void MountController::addMount(const MountInfo &info)
{
	_mounts.insert(info.name, info);

	//check mount state
	QProcess state;
	state.start(QStringLiteral("mount -l -t fuse.sshfs"));
	if(state.waitForFinished(1000)) {
		auto data = state.readAll();
		if(data.contains(QDir::cleanPath(info.localPath).toUtf8()))
			_mounts[info.name].mounted = true;
	} else
		qWarning() << "Unable to get mount state";
}

void MountController::removeMount(const QString &name)
{
	_mounts.remove(name);
}

void MountController::reloadState()
{
	QProcess state;
	state.start(QStringLiteral("mount -l -t fuse.sshfs"));
	if(state.waitForFinished(1000)) {
		auto data = state.readAll();
		for(auto it = _mounts.begin(); it != _mounts.end(); it++) {
			it->mounted = data.contains(QDir::cleanPath(it->info.localPath).toUtf8());
			emit mountChanged(it->info.name);
		}
	} else
		qWarning() << "Unable to get mount state";
}

void MountController::mount(const QString &name)
{
	if(!_mounts.contains(name))
		return;
	auto &state = _mounts[name];
	if(state.mounted)
		return;

	if(state.process) {
		emit mountChanged(name);
		emit mountError(name, tr("Wait for the previous mount/unmount to finish"));
		return;
	}
	auto env = QProcessEnvironment::systemEnvironment();
	if(!env.contains(QStringLiteral("SSH_ASKPASS"))) {
		emit mountChanged(name);
		emit mountError(name, tr("The SSH_ASKPASS environment variable must be set!\n"
								 "You can use for example \"ksshaskpass\""));
		return;
	}

	QDir mntDir(state.info.localPath);
	if(!mntDir.exists()){
		if(!mntDir.mkpath(QStringLiteral("."))) {
			emit mountChanged(name);
			emit mountError(name,
							tr("Failed to create mount directory %1")
							.arg(mntDir.absolutePath()));
			return;
		}
	}

	state.process = createProcess(name, true);
	state.process->setProgram(QStringLiteral("sshfs"));
	QStringList args;
	args.append(state.info.hostName + QLatin1Char(':') + state.info.remotePath);
	args.append(state.info.localPath);
	state.process->setArguments(args);

	state.process->start();
}

void MountController::unmount(const QString &name)
{
	if(!_mounts.contains(name))
		return;
	auto &state = _mounts[name];
	if(!state.mounted)
		return;

	if(state.process) {
		emit mountChanged(name);
		emit mountError(name, tr("Wait for the previous mount/unmount to finish"));
		return;
	}

	state.process = createProcess(name, false);
	state.process->setProgram(QStringLiteral("fusermount"));
	QStringList args;
	args.append(QStringLiteral("-u"));
	args.append(state.info.localPath);
	state.process->setArguments(args);

	state.process->start();
}

QProcess *MountController::createProcess(const QString &name, bool forMount)
{
	auto process = new QProcess(this);
	process->setProcessChannelMode(QProcess::MergedChannels);
	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			this, [=](int exitCode, QProcess::ExitStatus exitStatus){
		if(exitStatus != QProcess::NormalExit) {
			emit mountChanged(name);
			emit mountError(name, process->errorString());
		} else if(exitCode != EXIT_SUCCESS) {
			emit mountChanged(name);
			emit mountError(name, QString::fromUtf8(process->readAll()), exitCode);
		} else {
			_mounts[name].mounted = forMount;
			emit mountChanged(name);
		}
		process->deleteLater();
	});
	return process;
}

MountController::MountState::MountState(const MountInfo &info) :
	info(info),
	mounted(false),
	process(nullptr)
{}
