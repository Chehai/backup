#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE s3_store_test
#include <boost/test/unit_test.hpp>
#include "../src/s3_store.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	std::string ak = "abc";
	std::string sak = "cdf";
	std::string bn = "xyz";
	S3Store ss0(ak, sak, bn);
	BOOST_CHECK_EQUAL(ss0.status(), RemoteStore::Valid);
	BOOST_CHECK_EQUAL(ss0.access_key(), ak);		
	BOOST_CHECK_EQUAL(ss0.secret_access_key(), sak);		
	BOOST_CHECK_EQUAL(ss0.bucket_name(), bn);
	BOOST_CHECK_EQUAL(bn, ss0.bucket_context().bucketName);		
	BOOST_CHECK_EQUAL(ss0.bucket_context().protocol, S3ProtocolHTTPS);		
	BOOST_CHECK_EQUAL(ss0.bucket_context().uriStyle, S3UriStylePath);
	BOOST_CHECK_EQUAL(ak, ss0.bucket_context().accessKeyId);
	BOOST_CHECK_EQUAL(sak, ss0.bucket_context().secretAccessKey);
}