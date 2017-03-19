#ifndef MOUNTMODEL_H
#define MOUNTMODEL_H

#include <QAbstractTableModel>
#include "mountinfo.h"

class MountModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit MountModel(QObject *parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
	typedef QPair<MountInfo, bool> MountState;

	QList<MountState> _data;
};

#endif // MOUNTMODEL_H
