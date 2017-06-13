#ifndef MOUNTMODEL_H
#define MOUNTMODEL_H

#include <QAbstractTableModel>
#include <QMenu>
#include "mountinfo.h"
#include "mountcontroller.h"

class MountModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit MountModel(QObject *parent = nullptr);

	MountController *controller();

	QMenu *createMountMenu(QWidget *parent);

	MountInfo mountInfo(const QModelIndex &index) const;
	void addMountInfo(const MountInfo &info);
	void updateMountInfo(const QModelIndex &index, const MountInfo &info);
	void removeMountInfo(const QModelIndex &index);

	bool isMounted(const QModelIndex &index) const;
	void mount(const QModelIndex &index);
	void unmount(const QModelIndex &index);
	void reload();

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private slots:
	void updateMounted(const QString &name);

	void triggered(bool checked);

private:
	MountController *_controller;
	QStringList _names;

	QMenu *_mntMenu;
	QHash<QString, QAction*> _mntActions;

	void addMntAction(const QString &name);
	void saveState();
};

#endif // MOUNTMODEL_H
