#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE backup_restore_tasks_test
#include <boost/test/unit_test.hpp>
#include "../src/backup_task.h"
#include "../src/restore_task.h"
#include "../src/s3_store.h"

BOOST_AUTO_TEST_CASE(backup_task_constuctor_test)
{
	ParentTask m;
	ThreadPool tp(4, 2);
	RemoteStore rs;
	boost::filesystem::path file_path = __FILE__;
	boost::filesystem::path dir = file_path.parent_path();
	std::string pre = "";
	BackupTask * bt = new BackupTask(tp, &rs, dir, pre, m);
	BOOST_CHECK_EQUAL(m.children().front(), bt);
}

BOOST_AUTO_TEST_CASE(restore_task_constuctor_test)
{
	ParentTask m;
	ThreadPool tp(4, 2);
	RemoteStore rs;
	boost::filesystem::path file_path = __FILE__;
	boost::filesystem::path dir = file_path.parent_path();
	std::string pre = "";
	std::time_t now = std::time(NULL);
	RestoreTask * rt = new RestoreTask(tp, &rs, dir, pre, now, m);
	BOOST_CHECK_EQUAL(m.children().front(), rt);
}

BOOST_AUTO_TEST_CASE(backup_restore_test)
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
	S3Store ss(ak, sak, bn);
	ParentTask m;
	ThreadPool tp(2, 1);
	tp.start();
	std::string prefix = "";
	boost::filesystem::path backup_dir = filepath.parent_path();
	backup_dir /= "backup";
	BackupTask * bt = new BackupTask(tp, &ss, backup_dir, prefix, m);
	tp.pushs(m.children());
	m.wait_children();
	
	boost::filesystem::remove_all(backup_dir);
	boost::filesystem::create_directories(backup_dir);
	std::time_t now = std::time(NULL);
	RestoreTask * rt = new RestoreTask(tp, &ss, backup_dir, prefix, now, m);
	tp.pushs(m.children());
	m.wait_children();
	
	boost::filesystem::path file1 = backup_dir;
	file1 /= "file1.txt";
	BOOST_CHECK_EQUAL(boost::filesystem::exists(file1), true);
	boost::filesystem::path file2 = backup_dir;
	file2 /= "file2.txt";
	BOOST_CHECK_EQUAL(boost::filesystem::exists(file2), true);
	boost::filesystem::path file3 = backup_dir;
	file3 /= "sub/file3.txt";
	BOOST_CHECK_EQUAL(boost::filesystem::exists(file3), true);
	boost::filesystem::path file4 = backup_dir;
	file4 /= "sub/file4.txt";
	BOOST_CHECK_EQUAL(boost::filesystem::exists(file4), true);
}