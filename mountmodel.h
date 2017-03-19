#ifndef MOUNTMODEL_H
#define MOUNTMODEL_H

#include <QAbstractTableModel>
#include "mountinfo.h"

class MountModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit MountModel(QObject *parent = nullptr);

	MountInfo mount(const QModelIndex &index) const;
	void addMount(const MountInfo &info);
	void updateMount(const QModelIndex &index, const MountInfo &info);
	void removeMount(const QModelIndex &index);

	bool isMounted(const QModelIndex &index) const;
	void updateMounted(const QModelIndex &index, bool mounted);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
	typedef QPair<MountInfo, bool> MountState;

	QList<MountState> _data;

	void saveState();
};

#endif // MOUNTMODEL_H
