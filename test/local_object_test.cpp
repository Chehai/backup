#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE local_object_test
#include <boost/test/unit_test.hpp>
#include "../src/local_object.h"
#include "../src/remote_object.h"

BOOST_AUTO_TEST_CASE(constructor_test) 
{
	boost::system::error_code err;
	boost::filesystem::path file_path = __FILE__;
	std::time_t t = boost::filesystem::last_write_time(file_path, err);
	LocalObject lo0(file_path, file_path.parent_path(), "");
	BOOST_CHECK_EQUAL(lo0.fs_path(), file_path);
	boost::filesystem::path file_uri = file_path.parent_path().filename();
	file_uri /= file_path.filename(); 
	BOOST_CHECK_EQUAL(lo0.uri(), file_uri.string());
	BOOST_CHECK_EQUAL(lo0.updated_at(), t);
	BOOST_CHECK_EQUAL(lo0.status(), BackupObject::Valid);
	
	std::string backup_prefix = "aaa";
	LocalObject lo1(file_path, file_path.parent_path().parent_path(), backup_prefix);
	BOOST_CHECK_EQUAL(lo1.fs_path(), file_path);
	file_uri = file_path.parent_path().parent_path().filename();
	file_uri /= file_path.parent_path().filename();
	file_uri /= file_path.filename();
	BOOST_CHECK_EQUAL(lo1.uri(), backup_prefix + file_uri.string());
	BOOST_CHECK_EQUAL(lo1.updated_at(), t);
	BOOST_CHECK_EQUAL(lo1.status(), BackupObject::Valid);
	
	file_path = "no_such_dir/no_such_file";
	LocalObject lo2(file_path, file_path.parent_path(), backup_prefix);
	BOOST_CHECK_EQUAL(lo2.status(), BackupObject::Invalid);
}

BOOST_AUTO_TEST_CASE(populate_local_objects_table_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	BackupObject::init_db(test_db_path);
	boost::filesystem::path file_path = __FILE__;
	LocalObject::populate_local_objects_table(file_path.parent_path(), "");
	std::time_t t = boost::filesystem::last_write_time(file_path, err);
	LocalObject lo0(file_path, file_path.parent_path(), "");
	LocalObject res0 = LocalObject::find_by_uri(lo0.uri());
	BOOST_CHECK_EQUAL(res0.status(), BackupObject::Valid);
	BOOST_CHECK_EQUAL(res0.uri(), lo0.uri());
	BOOST_CHECK_EQUAL(res0.updated_at(), lo0.updated_at());
	BOOST_CHECK_EQUAL(res0.fs_path(), lo0.fs_path());
	BackupObject::close_db();
}

BOOST_AUTO_TEST_CASE(find_to_upload_test)
{
	boost::system::error_code err;
	boost::filesystem::path test_db_path = "test_objects.db";
	BackupObject::init_db(test_db_path);
	boost::filesystem::path file_path = __FILE__;
	LocalObject::populate_local_objects_table(file_path.parent_path(), file_path.parent_path().parent_path().string() + "/");
	
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
	
	std::list<LocalObject>& los = LocalObject::find_to_upload();
	BOOST_CHECK_EQUAL(los.empty(), true);
	
	class TestRemoteStore2 : public RemoteStore {
		int list(const std::string& prefix, std::list<RemoteObject>& remote_objects) {
			boost::filesystem::path file_path = __FILE__;
			boost::filesystem::path dir = file_path.parent_path();
			boost::filesystem::recursive_directory_iterator iter(dir), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					boost::system::error_code err;
					std::time_t t = boost::filesystem::last_write_time(iter->path(), err) + 10;
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
	TestRemoteStore2 trs2;
	RemoteObject::populate_remote_objects_table(&trs2, file_path.parent_path(), "");
	los.clear();
	std::list<LocalObject>& los2 = LocalObject::find_to_upload();
	BOOST_CHECK_EQUAL(los2.empty(), false);
	
	BackupObject::close_db();
}