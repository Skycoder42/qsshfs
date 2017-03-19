#include "mountmodel.h"

MountModel::MountModel(QObject *parent) :
	QAbstractTableModel(parent),
	_data()
{}

QVariant MountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
		return {};

	switch (section) {
	case 0:
		return tr("Name");
	case 1:
		return tr("Mounted");
	case 2:
		return tr("Host");
	case 3:
		return tr("Local Path");
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
		return 4;
}

QVariant MountModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	// FIXME: Implement me!
	return QVariant();
}
