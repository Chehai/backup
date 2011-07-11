#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE local_object_test
#include <boost/test/unit_test.hpp>
#include "../src/local_object.h"

BOOST_AUTO_TEST_CASE(updated_at_test) 
{
	std::string filename = __FILE__;
	LocalObject lo0(filename);
	std::string ts("2002-01-20 23:59:59.000");
	Timestamp t(boost::posix_time::time_from_string(ts));
	lo0.set_updated_at();
	BOOST_CHECK_GT(lo0.updated_at(), t);
	BOOST_CHECK_EQUAL(lo0.status(), BackupObject::Valid);
	
	LocalObject lo1("no_such_file");
	lo1.set_updated_at();
	BOOST_CHECK_EQUAL(lo1.status(), BackupObject::Invalid);		
}