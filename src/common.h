#ifndef COMMON_H
#define COMMON_H
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqlite3.h>
#include <libs3.h>
#include <glog/logging.h>
extern void failure_function_for_glog();
#endif