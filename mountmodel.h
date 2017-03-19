#ifndef MOUNTMODEL_H
#define MOUNTMODEL_H

#include <QAbstractTableModel>
#include "mountinfo.h"
#include "mountcontroller.h"

class MountModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit MountModel(QObject *parent = nullptr);

	MountController *controller();

	MountInfo mountInfo(const QModelIndex &index) const;
	void addMountInfo(const MountInfo &info);
	void updateMountInfo(const QModelIndex &index, const MountInfo &info);
	void removeMountInfo(const QModelIndex &index);

	bool isMounted(const QModelIndex &index) const;
	void mount(const QModelIndex &index);
	void unmount(const QModelIndex &index);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private slots:
	void updateMounted(const QString &name);

private:
	MountController *_controller;
	QStringList _names;

	void saveState();
};

#endif // MOUNTMODEL_H
