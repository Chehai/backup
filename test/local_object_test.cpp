#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE local_object_test
#include <boost/test/unit_test.hpp>
#include "../src/local_object.h"

BOOST_AUTO_TEST_CASE(updated_at_test) 
{
	boost::system::error_code err;
	boost::filesystem::path filepath = __FILE__;
	std::time_t t = boost::filesystem::last_write_time(filepath, err);
	LocalObject lo0(filepath.parent_path().string(), filepath.filename().string());
	Timestamp tt = boost::posix_time::from_time_t(t);
	BOOST_CHECK_EQUAL(lo0.root(), filepath.parent_path().string());
	BOOST_CHECK_EQUAL(lo0.updated_at(), tt);
	BOOST_CHECK_EQUAL(lo0.status(), BackupObject::Valid);
	
	LocalObject lo1("no_such_dir", "no_such_file");
	BOOST_CHECK_EQUAL(lo1.status(), BackupObject::Invalid);		
}