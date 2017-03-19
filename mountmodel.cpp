#include "mountmodel.h"

#include <QSettings>

MountModel::MountModel(QObject *parent) :
	QAbstractTableModel(parent),
	_data()
{
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
		_data.append({data, false});
	}
	settings.endArray();
}

MountInfo MountModel::mount(const QModelIndex &index) const
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _data.size())
		return {};
	else
		return _data[index.row()].first;
}

void MountModel::addMount(const MountInfo &info)
{
	beginInsertRows(QModelIndex(), _data.size(), _data.size());
	_data.append({info, false});
	endInsertRows();
	saveState();
}

void MountModel::updateMount(const QModelIndex &index, const MountInfo &info)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _data.size())
		return;

	_data[index.row()].first = info;
	emit dataChanged(index, index);
	saveState();
}

void MountModel::removeMount(const QModelIndex &index)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _data.size())
		return;

	beginRemoveRows(index.parent(), index.row(), index.row());
	_data.removeAt(index.row());
	endRemoveRows();
	saveState();
}

bool MountModel::isMounted(const QModelIndex &index) const
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _data.size())
		return false;
	else
		return _data[index.row()].second;
}

void MountModel::updateMounted(const QModelIndex &index, bool mounted)
{
	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= _data.size())
		return;

	_data[index.row()].second = mounted;
	emit dataChanged(index, index);
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
		return _data.size();
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
		index.row() >= _data.size())
		return {};

	auto data = _data[index.row()];
	switch (index.column()) {
	case 0:
		if(role == Qt::DisplayRole)
			return data.first.name;
		break;
	case 1:
		if(role == Qt::DisplayRole) {
			auto host = data.first.hostName;
			if(!data.first.userOverwrite.isEmpty())
				host.prepend(data.first.userOverwrite + QLatin1Char('@'));
			return host;
		}
		break;
	case 2:
		if(role == Qt::CheckStateRole)
			return data.second ? Qt::Checked : Qt::Unchecked;
		else if(role == Qt::DisplayRole)
			return data.second ? data.first.localPath : QString();
		break;
	default:
		break;
	}

	return {};
}

void MountModel::saveState()
{
	QSettings settings;
	settings.beginWriteArray(QStringLiteral("mounts"), _data.size());
	for(auto i = 0; i < _data.size(); i++) {
		const auto &data = _data[i].first;
		settings.setArrayIndex(i);
		settings.setValue(QStringLiteral("name"), data.name);
		settings.setValue(QStringLiteral("hostName"), data.hostName);
		settings.setValue(QStringLiteral("userOverwrite"), data.userOverwrite);
		settings.setValue(QStringLiteral("remotePath"), data.remotePath);
		settings.setValue(QStringLiteral("localPath"), data.localPath);
	}
	settings.endArray();
}
