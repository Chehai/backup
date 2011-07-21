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

static bool 
compare(RemoteObject first, RemoteObject second) 
{
	return first.uri() < second.uri();
}

BOOST_AUTO_TEST_CASE(list_test) 
{
	std::string ak, sak, bn;
	boost::filesystem::path filepath = __FILE__;
	boost::filesystem::path s3_config_path = filepath.parent_path();
	s3_config_path /= "s3_config.txt";
	std::ifstream s3_config(s3_config_path.string().c_str());
	std::getline(s3_config, ak);
	std::getline(s3_config, sak);
	s3_config.close();
	bn = "wuchehaitest";
	S3Store ss0(ak, sak, bn);
	std::list<RemoteObject> objects;
	std::string prefix = "test";
	ss0.list(prefix, objects);
	
	objects.sort(compare);
	RemoteObject t = objects.front();
	BOOST_CHECK_EQUAL(t.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(t.uri(), "test/t.txt");
	BOOST_CHECK_LT(t.updated_at(), std::time(NULL));
	RemoteObject tt = objects.back();
	BOOST_CHECK_EQUAL(tt.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(tt.uri(), "test/tt.txt");
	BOOST_CHECK_LT(tt.updated_at(), std::time(NULL));
}