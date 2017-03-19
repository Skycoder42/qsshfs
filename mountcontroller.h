#ifndef MOUNTCONTROLLER_H
#define MOUNTCONTROLLER_H

#include "mountinfo.h"

#include <QObject>
#include <QPointer>
#include <QProcess>

class MountController : public QObject
{
	Q_OBJECT

public:
	explicit MountController(QObject *parent = nullptr);

	MountInfo mountInfo(const QString &name) const;
	bool isMounted(const QString &name);

public slots:
	void addMount(const MountInfo &info);
	void removeMount(const QString &name);

	void mount(const QString &name);
	void unmount(const QString &name);

signals:
	void mountChanged(const QString &name);
	void mountError(const QString &name, const QString &errorLog, int exitCode = -1);

private:
	struct MountState
	{
		MountInfo info;
		bool mounted;
		QPointer<QProcess> process;

		MountState(const MountInfo &info = {});
	};

	QHash<QString, MountState> _mounts;

	QProcess *createProcess(const QString &name, bool forMount);
};

#endif // MOUNTCONTROLLER_H
