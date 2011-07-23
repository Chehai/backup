#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE remote_object_test
#include <boost/test/unit_test.hpp>
#include "../src/remote_object.h"
#include "../src/local_object.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	std::string uri = "abc";
	std::time_t t;
	std::time(&t);
	RemoteObject ro0(uri, t, 'r');
	BOOST_CHECK_EQUAL(ro0.uri(), uri);
	BOOST_CHECK_EQUAL(ro0.updated_at(), t);
	BOOST_CHECK_EQUAL(ro0.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(ro0.action(), 'r');
	
	RemoteObject ro1;
	BOOST_CHECK_EQUAL(ro1.status(), BackupObject::Valid);	
	BOOST_CHECK_EQUAL(ro1.updated_at(), std::time_t(0));
	BOOST_CHECK_EQUAL(ro1.action(), char(0));		
}

BOOST_AUTO_TEST_CASE(populate_remote_objects_table_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	BackupObject::init_db(test_db_path);
	boost::filesystem::path file_path = __FILE__;
	
	class TestRemoteStore : public RemoteStore {
		int list(const std::string& prefix, std::list<RemoteObject>& remote_objects) {
			boost::filesystem::path file_path = __FILE__;
			boost::filesystem::path dir = file_path.parent_path();
			boost::filesystem::recursive_directory_iterator iter(dir), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					boost::system::error_code err;
					std::time_t t = boost::filesystem::last_write_time(iter->path(), err);
					RemoteObject ro(iter->path().string(), t, 'u');
					ro.set_status(BackupObject::Valid);
					if (err.value()) {
						ro.set_status(BackupObject::Invalid);
					}
					remote_objects.push_back(ro);
				}
			}
			return 0;
		}
	};
	TestRemoteStore trs;
	
	RemoteObject::populate_remote_objects_table(&trs, file_path.parent_path(), "");
	std::time_t t = boost::filesystem::last_write_time(file_path, err);
	RemoteObject ro0(file_path.string(), t, 'u');
	RemoteObject res0 = RemoteObject::find_by_uri(ro0.uri());
	BOOST_CHECK_EQUAL(res0.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(res0.uri(), ro0.uri());
	BOOST_CHECK_EQUAL(res0.updated_at(), ro0.updated_at());
	BOOST_CHECK_EQUAL(res0.action(), ro0.action());
	BackupObject::close_db();
}

BOOST_AUTO_TEST_CASE(find_to_delete_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	BackupObject::init_db(test_db_path);
	boost::filesystem::path file_path = __FILE__;
	
	class TestRemoteStore : public RemoteStore {
		int list(const std::string& prefix, std::list<RemoteObject>& remote_objects) {
			boost::filesystem::path file_path = __FILE__;
			boost::filesystem::path dir = file_path.parent_path();
			boost::filesystem::recursive_directory_iterator iter(dir), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					boost::system::error_code err;
					std::time_t t = boost::filesystem::last_write_time(iter->path(), err);
					RemoteObject ro(iter->path().string(), t, 'u');
					ro.set_status(BackupObject::Valid);
					if (err.value()) {
						ro.set_status(BackupObject::Invalid);
					}
					remote_objects.push_back(ro);
				}
			}
			return 0;
		}
	};
	TestRemoteStore trs;
	
	RemoteObject::populate_remote_objects_table(&trs, file_path.parent_path(), "");
	LocalObject::populate_local_objects_table(file_path.parent_path(), file_path.parent_path().parent_path().string() + "/");

	std::list<RemoteObject>& ros = RemoteObject::find_to_delete();
	BOOST_CHECK_EQUAL(ros.empty(), true);
	
	LocalObject::populate_local_objects_table(file_path.parent_path(), file_path.parent_path().parent_path().string() + "/a/");
	
	ros.clear();
	std::list<RemoteObject>& ros2 = RemoteObject::find_to_delete();
	BOOST_CHECK_EQUAL(ros2.empty(), false);
	
	BackupObject::close_db();
}