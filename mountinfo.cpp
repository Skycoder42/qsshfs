#include "mountinfo.h"

bool MountInfo::isValid() const
{
	return !name.isNull();
}
