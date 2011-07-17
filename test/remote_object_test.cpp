#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE remote_object_test
#include <boost/test/unit_test.hpp>
#include "../src/remote_object.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	std::string ts("2002-01-20 23:59:59.000");
	Timestamp t(boost::posix_time::time_from_string(ts));
	std::string uri = "abc";
	RemoteObject ro0(uri, t);
	BOOST_CHECK_EQUAL(ro0.uri(), uri);
	BOOST_CHECK_EQUAL(ro0.updated_at(), t);
	BOOST_CHECK_EQUAL(ro0.status(), BackupObject::Valid);
	
	RemoteObject ro1;
	BOOST_CHECK_EQUAL(ro1.status(), BackupObject::Valid);	
	BOOST_CHECK_EQUAL(ro1.updated_at(), zero());		
		
}