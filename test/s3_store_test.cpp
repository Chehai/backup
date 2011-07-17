#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE s3_store_test
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
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

BOOST_AUTO_TEST_CASE(lookup_test) 
{
	std::string ak = "abc";
	std::string sak = "cdf";
	std::string bn = "xyz";
	S3Store ss0(ak, sak, bn);
	
	boost::filesystem::path filepath = __FILE__;
	std::string uri = filepath.filename().string();
	boost::filesystem::path current_dir = filepath.parent_path();
	LocalObject lo0(filepath, uri);

	std::string ts("2002-01-20 23:59:59.000");
	Timestamp uploaded_at(boost::posix_time::time_from_string(ts));
	S3Store::insert_into_objects(uri, uploaded_at);
	RemoteObject ro0(uri, uploaded_at);
	RemoteObject res0;
	ss0.lookup(lo0, res0);
	BOOST_CHECK_EQUAL(res0.uri(), ro0.uri());
	BOOST_CHECK_EQUAL(res0.updated_at(), ro0.updated_at());
	
	boost::filesystem::path s3_config_path = current_dir;
	s3_config_path /= "s3_config.txt";
	std::ifstream s3_config(s3_config_path.string().c_str());
	std::getline(s3_config, ak);
	std::getline(s3_config, sak);
	s3_config.close();
	bn = "wuchehaitest";
	S3Store ss1(ak, sak, bn);
	boost::filesystem::path t_path = current_dir;
	t_path /= "t.txt";
	LocalObject lo1(t_path, "test/t.txt");
	RemoteObject res1;
	ss1.lookup(lo1, res1);
	BOOST_CHECK_EQUAL(res1.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(res1.uri(), lo1.uri());
	BOOST_CHECK_GT(lo1.updated_at(), res1.updated_at());
	boost::filesystem::path tt_path = current_dir;
	tt_path /= "tt.txt";
	LocalObject lo2(tt_path, "test/tt.txt");
	RemoteObject res2;
	std::cout << "The following should be fast" << std::endl;
	ss1.lookup(lo2, res2);
	BOOST_CHECK_EQUAL(res2.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(res2.uri(), lo2.uri());
	BOOST_CHECK_GT(lo2.updated_at(), res2.updated_at());
	std::cout << "The above should be fast" << std::endl;
	
}