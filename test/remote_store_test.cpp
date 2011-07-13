#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE remote_store_test
#include <boost/test/unit_test.hpp>
#include "../src/remote_store.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	RemoteStore rs0;
	BOOST_CHECK_EQUAL(rs0.status(), RemoteStore::Valid);
	rs0.set_status(RemoteStore::Invalid);
	BOOST_CHECK_EQUAL(rs0.status(), RemoteStore::Invalid);		
}