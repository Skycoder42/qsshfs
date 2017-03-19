#ifndef MOUNTINFO_H
#define MOUNTINFO_H

#include <QString>

struct MountInfo
{
	QString name;
	QString hostName;
	QString userOverwrite;
	QString remotePath;
	QString localPath;

	bool isValid() const;
};

#endif // MOUNTINFO_H
