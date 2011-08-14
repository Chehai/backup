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
	RemoteObject ro0(uri, t, 'r', 50);
	BOOST_CHECK_EQUAL(ro0.uri(), uri);
	BOOST_CHECK_EQUAL(ro0.updated_at(), t);
	BOOST_CHECK_EQUAL(ro0.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(ro0.action(), 'r');
	BOOST_CHECK_EQUAL(ro0.size(), 50);
	
	RemoteObject ro1;
	BOOST_CHECK_EQUAL(ro1.status(), BackupObject::Valid);	
	BOOST_CHECK_EQUAL(ro1.updated_at(), std::time_t(0));
	BOOST_CHECK_EQUAL(ro1.action(), char(0));
	BOOST_CHECK_EQUAL(ro1.size(), 0);		
		
}

BOOST_AUTO_TEST_CASE(populate_remote_objects_table_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	sqlite3 * objects_db_conn;
	sqlite3_open(test_db_path.c_str(), &objects_db_conn);
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
					RemoteObject ro(iter->path().string(), t, 'u', 50);
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
	
	RemoteObject::populate_remote_objects_table(objects_db_conn, &trs, file_path.parent_path(), "");
	std::time_t t = boost::filesystem::last_write_time(file_path, err);
	RemoteObject ro0(file_path.string(), t, 'u', 50);
	RemoteObject res0 = RemoteObject::find_by_uri(objects_db_conn, ro0.uri());
	BOOST_CHECK_EQUAL(res0.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(res0.uri(), ro0.uri());
	BOOST_CHECK_EQUAL(res0.updated_at(), ro0.updated_at());
	BOOST_CHECK_EQUAL(res0.action(), ro0.action());
	BOOST_CHECK_EQUAL(res0.size(), ro0.size());
	sqlite3_close(objects_db_conn);
}

BOOST_AUTO_TEST_CASE(find_to_del_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	sqlite3 * objects_db_conn;
	sqlite3_open(test_db_path.c_str(), &objects_db_conn);
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
					RemoteObject ro(iter->path().string(), t, 'u', 50);
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
	
	RemoteObject::populate_remote_objects_table(objects_db_conn, &trs, file_path.parent_path(), "");
	LocalObject::populate_local_objects_table(objects_db_conn, file_path.parent_path(), file_path.parent_path().parent_path().string() + "/");

	std::list<RemoteObject> ros;
	RemoteObject::find_to_del(objects_db_conn, ros);
	BOOST_CHECK_EQUAL(ros.empty(), true);
	
	LocalObject::populate_local_objects_table(objects_db_conn, file_path.parent_path(), file_path.parent_path().parent_path().string() + "/a/");
	
	ros.clear();
	std::list<RemoteObject> ros2;
	RemoteObject::find_to_del(objects_db_conn, ros2);
	BOOST_CHECK_EQUAL(ros2.empty(), false);
	
	sqlite3_close(objects_db_conn);
}

BOOST_AUTO_TEST_CASE(find_to_get_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	sqlite3 * objects_db_conn;
	sqlite3_open(test_db_path.c_str(), &objects_db_conn);
	boost::filesystem::path file_path = __FILE__;
	
	class TestRemoteStore : public RemoteStore {
		int list(const std::string& prefix, std::list<RemoteObject>& remote_objects) {
			std::time_t t = 1310532657;
			RemoteObject ro0("t.txt", t - 10, 'u', 1);
			RemoteObject ro1("t.txt", t - 9 , 'u', 2);
			RemoteObject ro2("t.txt", t - 8 , 'u', 3);
			RemoteObject ro3("t.txt", t + 3600 , 'u', 4);
			RemoteObject ro4("t.txt", t + 7200 , 'u', 5);
			
			RemoteObject ro5("tt.txt", t - 20 , 'u', 6);
			RemoteObject ro6("tt.txt", t - 10 , 'd', 7);
			
			remote_objects.push_back(ro0);
			remote_objects.push_back(ro1);
			remote_objects.push_back(ro2);
			remote_objects.push_back(ro3);
			remote_objects.push_back(ro4);
			remote_objects.push_back(ro5);
			remote_objects.push_back(ro6);
			return 0;
		}
	};
	TestRemoteStore trs;
	
	RemoteObject::populate_remote_objects_table(objects_db_conn, &trs, file_path.parent_path(), "");
	std::time_t t = 1310532657;
	std::list<RemoteObject> ros;
	RemoteObject::find_to_get(objects_db_conn, t, ros);
	BOOST_CHECK_EQUAL(ros.size(), 1);
	RemoteObject res = ros.front();
	BOOST_CHECK_EQUAL(res.uri(), "t.txt");
	BOOST_CHECK_EQUAL(res.updated_at(), t - 8);
	BOOST_CHECK_EQUAL(res.action(), 'u');
	BOOST_CHECK_EQUAL(res.size(), 3);
	
	sqlite3_close(objects_db_conn);
}