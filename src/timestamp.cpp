#include "timestamp.h"

Timestamp now()
{
	return boost::posix_time::second_clock::universal_time();
}

Timestamp zero()
{
	return boost::posix_time::from_time_t(0);
}