#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <boost/date_time/posix_time/posix_time.hpp>
typedef boost::posix_time::ptime Timestamp;
Timestamp now();
Timestamp zero();
#endif