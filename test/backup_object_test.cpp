#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE backup_object_test
#include <boost/test/unit_test.hpp>
#include <backup_object.h>

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	BackupObject bo1("");
	BOOST_CHECK_EQUAL(bo1.local_path(), "");
	
	BackupObject bo2("abc");
	BOOST_CHECK_EQUAL(bo2.local_path(), "abc");
}