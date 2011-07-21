#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE backup_object_test
#include <boost/test/unit_test.hpp>
#include "../src/backup_object.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	BackupObject bo0;
	BOOST_CHECK_EQUAL(bo0.uri(), "");
	BOOST_CHECK_EQUAL(bo0.status(), BackupObject::Valid);
	BOOST_CHECK_LE(bo0.updated_at(), std::time_t(0));
	
	BackupObject bo1("");
	BOOST_CHECK_EQUAL(bo1.uri(), "");
	BOOST_CHECK_EQUAL(bo1.status(), BackupObject::Valid);
	BOOST_CHECK_LE(bo1.updated_at(), std::time_t(0));
	
	BackupObject bo2("abc");
	BOOST_CHECK_EQUAL(bo2.uri(), "abc");
	BOOST_CHECK_EQUAL(bo2.status(), BackupObject::Valid);
	BOOST_CHECK_LE(bo2.updated_at(), std::time_t(0));
	
	BackupObject bo3(NULL);
	BOOST_CHECK_EQUAL(bo3.uri(), "");
	BOOST_CHECK_EQUAL(bo3.status(), BackupObject::Valid);
	BOOST_CHECK_LE(bo3.updated_at(), std::time_t(0));
	
	std::string str = "haha";
	BackupObject bo4(str);
	BOOST_CHECK_EQUAL(bo4.uri(), str);
	BOOST_CHECK_EQUAL(bo4.status(), BackupObject::Valid);
	BOOST_CHECK_LE(bo4.updated_at(), std::time_t(0));
	
	std::time_t t;
	std::time(&t);
	BackupObject bo5(str, t);
	BOOST_CHECK_EQUAL(bo5.uri(), str);
	BOOST_CHECK_EQUAL(bo5.status(), BackupObject::Valid);	
	BOOST_CHECK_EQUAL(bo5.updated_at(), t);
	
	BackupObject bo6("str", t);
	BOOST_CHECK_EQUAL(bo6.uri(), "str");
	BOOST_CHECK_EQUAL(bo6.status(), BackupObject::Valid);	
	BOOST_CHECK_EQUAL(bo6.updated_at(), t);
}

BOOST_AUTO_TEST_CASE(setters_test)
{
	BackupObject bo("abc");
	
	bo.set_status(BackupObject::Invalid);
	BOOST_CHECK_EQUAL(bo.status(), BackupObject::Invalid);
	
	bo.set_status(BackupObject::Valid);
	BOOST_CHECK_EQUAL(bo.status(), BackupObject::Valid);
	
	bo.set_uri("abc");
	BOOST_CHECK_EQUAL(bo.uri(), "abc");
	
	bo.set_uri(NULL);
	BOOST_CHECK_EQUAL(bo.uri(), "");
	
	bo.set_uri(std::string("haha"));
	BOOST_CHECK_EQUAL(bo.uri(), std::string("haha"));
	
	std::time_t t;
	std::time(&t);
	bo.set_updated_at(t);
	BOOST_CHECK_EQUAL(bo.updated_at(), t);
}