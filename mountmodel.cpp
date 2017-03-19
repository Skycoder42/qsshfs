#include "mountmodel.h"

#include <QSettings>

MountModel::MountModel(QObject *parent) :
	QAbstractTableModel(parent),
	_controller(new MountController(this)),
	_names(),
	_mntMenu(nullptr),
	_mntActions()
{
	connect(_controller, &MountController::mountChanged,
			this, &MountModel::updateMounted);

	QSettings settings;
	auto max = settings.beginReadArray(QStringLiteral("mounts"));
	for(auto i = 0; i < max; i++) {
		MountInfo data;
		settings.setArrayIndex(i);
		data.name = settings.value(QStringLiteral("name")).toString();
		data.hostName = settings.value(QStringLiteral("hostName")).toString();
		data.userOverwrite = settings.value(QStringLiteral("userOverwrite")).toString();
		data.remotePath = settings.value(QStringLiteral("remotePath")).toString();
		data.localPath = settings.value(QStringLiteral("localPath")).toString();
		_controller->addMount(data);
		_names.append(data.name);
	}
	settings.endArray();
}

MountController *MountModel::controller()
{
	return _controller;
}

QMenu *MountModel::createMountMenu(QWidget *parent)
{
	if(!_mntMenu) {
		_mntMenu = new QMenu(tr("Mounts"), parent);
		_mntMenu->setIcon(QIcon::fromTheme(QStringLiteral("gtk-connect")));

		foreach (auto name, _names)
			addMntAction(name);

	}

	return _mntMenu;
}

MountInfo MountModel::mountInfo(const QModelIndex &index) const
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return {};
	else
		return _controller->mountInfo(_names[index.row()]);
}

void MountModel::addMountInfo(const MountInfo &info)
{
	if(_names.contains(info.name)) {
		//TODO show error
		return;
	}

	beginInsertRows(QModelIndex(), _names.size(), _names.size());
	_controller->addMount(info);
	_names.append(info.name);
	addMntAction(info.name);
	endInsertRows();

	saveState();
}

void MountModel::updateMountInfo(const QModelIndex &index, const MountInfo &info)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return;

	if(_names[index.row()] != info.name) {
		//TODO show error
		return;
	}
	if(_controller->isMounted(info.name)) {
		//TODO show error
		return;
	}

	_controller->removeMount(_names[index.row()]);
	_controller->addMount(info);
	emit dataChanged(index.sibling(index.row(), 0),
					 index.sibling(index.row(), 2));

	saveState();
}

void MountModel::removeMountInfo(const QModelIndex &index)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return;

	auto name = _names[index.row()];
	if(_controller->isMounted(name)) {
		//TODO show error
		return;
	}

	beginRemoveRows(index.parent(), index.row(), index.row());
	auto act = _mntActions.take(name);
	if(act)
		act->deleteLater();
	_names.removeAt(index.row());
	_controller->removeMount(name);
	endRemoveRows();

	saveState();
}

bool MountModel::isMounted(const QModelIndex &index) const
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return false;
	else
		return _controller->isMounted(_names[index.row()]);
}

void MountModel::mount(const QModelIndex &index)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return;

	_controller->mount(_names[index.row()]);
}

void MountModel::unmount(const QModelIndex &index)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return;

	_controller->unmount(_names[index.row()]);
}

QVariant MountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
		return {};

	switch (section) {
	case 0:
		return tr("Name");
	case 1:
		return tr("Host");
	case 2:
		return tr("Mounted");
	default:
		Q_UNREACHABLE();
		return {};
	}
}

int MountModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return _names.size();
}

int MountModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return 3;
}

QVariant MountModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _names.size())
		return {};

	auto data = _controller->mountInfo(_names[index.row()]);
	switch (index.column()) {
	case 0:
		if(role == Qt::DisplayRole)
			return data.name;
		break;
	case 1:
		if(role == Qt::DisplayRole) {
			auto host = data.hostName;
			if(!data.userOverwrite.isEmpty())
				host.prepend(data.userOverwrite + QLatin1Char('@'));
			return host;
		}
		break;
	case 2:
		if(role == Qt::CheckStateRole)
			return _controller->isMounted(data.name) ? Qt::Checked : Qt::Unchecked;
		else if(role == Qt::DisplayRole)
			return _controller->isMounted(data.name) ? data.localPath : QString();
		break;
	default:
		break;
	}

	return {};
}

void MountModel::updateMounted(const QString &name)
{
	auto mIndex = index(_names.indexOf(name), 2);
	if(mIndex.isValid())
		emit dataChanged(mIndex, mIndex, {Qt::CheckStateRole});

	auto act = _mntActions.value(name);
	if(act)
		act->setChecked(_controller->isMounted(name));
}

void MountModel::triggered(bool checked)
{
	auto act = qobject_cast<QAction*>(sender());
	if(act) {
		auto name = _mntActions.key(act);
		if(!name.isEmpty()) {
			if(checked)
				_controller->mount(name);
			else
				_controller->unmount(name);
			act->setChecked(!checked);
		}
	}
}

void MountModel::addMntAction(const QString &name)
{
	if(_mntMenu) {
		auto act = _mntMenu->addAction(name);
		act->setCheckable(true);
		act->setChecked(_controller->isMounted(name));
		connect(act, &QAction::triggered,
				this, &MountModel::triggered);
		_mntActions.insert(name, act);
	}
}

void MountModel::saveState()
{
	QSettings settings;
	settings.beginWriteArray(QStringLiteral("mounts"), _names.size());
	for(auto i = 0; i < _names.size(); i++) {
		const auto &data = _controller->mountInfo(_names[i]);
		settings.setArrayIndex(i);
		settings.setValue(QStringLiteral("name"), data.name);
		settings.setValue(QStringLiteral("hostName"), data.hostName);
		settings.setValue(QStringLiteral("userOverwrite"), data.userOverwrite);
		settings.setValue(QStringLiteral("remotePath"), data.remotePath);
		settings.setValue(QStringLiteral("localPath"), data.localPath);
	}
	settings.endArray();
}
