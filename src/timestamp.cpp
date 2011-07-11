#include "timestamp.h"

Timestamp now()
{
	return boost::posix_time::second_clock::universal_time();
}