#include "common.h"
#include "src/backup_task.h"
#include "src/restore_task.h"
#include "src/list_task.h"
#include "src/s3_store.h"

void failure_function_for_glog() {
	std::cerr << "exiting..." << std::endl;
 	exit(-1);
}

static boost::posix_time::time_duration get_utc_offset() {
    const boost::posix_time::ptime utc_now = boost::posix_time::second_clock::universal_time();
    const boost::posix_time::ptime now = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc_now);
    return utc_now - now;
}

static std::time_t convert_to_timestamp(const std::string& timestamp_str)
{
	boost::posix_time::ptime timestamp_pt = boost::posix_time::time_from_string(timestamp_str);
	boost::posix_time::ptime timestamp_utc = timestamp_pt + get_utc_offset();
	boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
	boost::posix_time::time_duration::sec_type timestamp_sec = (timestamp_utc - epoch).total_seconds();
	return (std::time_t)timestamp_sec;
}

int
main(int argc, char** argv)
{
	google::InitGoogleLogging(argv[0]);
	google::InstallFailureFunction(&failure_function_for_glog);
  	enum Command {
    	Backup,
		Restore,
		List
    };
	std::string config_file;
	std::vector<std::string> backup_dirs;
	std::string backup_prefix = "";
	std::string s3_access_key = "";
	std::string s3_secret_key = "";
	std::string s3_bucket_name = "";
	boost::filesystem::path db_path = "objects.db";
	Command command = Backup;
	std::time_t timestamp = std::time(NULL);
	std::string timestamp_str = boost::posix_time::to_simple_string(boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(boost::posix_time::from_time_t(timestamp)));
	
	boost::program_options::options_description basic_options("Basic Options");
    basic_options.add_options()
        ("help,h", "Show help message")
		("restore,r", "Restore")
		("list,l", "List")
		("timestamp,t", boost::program_options::value<std::string>(), "Timestamp")
        ("config,c", boost::program_options::value<std::string>(&config_file)->default_value("backup.cfg"),
                  "name of a file of a configuration.")
    ;
	boost::program_options::options_description backup_options("Backup Options");
	backup_options.add_options()
		("backup-dirs,b", boost::program_options::value< std::vector<std::string> >(), "Backup Directories")
		("backup-prefix,p", boost::program_options::value<std::string>(), "Backup Prefix")
		("backup-database,d", boost::program_options::value<std::string>(), "Backup Database Path")
	;
	boost::program_options::options_description s3_options("S3 Options");
	s3_options.add_options()
		("s3-access-key", boost::program_options::value<std::string>(), "S3 Access Key")
		("s3-secret-key", boost::program_options::value<std::string>(), "S3 Secret Key")
		("s3-bucket-name", boost::program_options::value<std::string>(), "S3 Bucket Name")
	;
	
	boost::program_options::options_description cmdline_options;
    cmdline_options.add(basic_options).add(backup_options);

    boost::program_options::options_description config_file_options;
    config_file_options.add(backup_options).add(s3_options);
  	
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), vm);
		boost::program_options::notify(vm);
	} catch (boost::program_options::error& err) {
		LOG(FATAL) << "main: parse program options error " << err.what();
	}
	if (vm.count("help")) {
		std::cout << "./my_backup --help|-h --config|-c --backup-dir|-b --backup-prefix|-p --backup-database|-d --restore|-r --timestamp|-t" << std::endl;
		return 0;
	}
	
	if (vm.count("restore")) {
		command = Restore;
	}
	
	if (vm.count("list")) {
		command = List;
	}
	
	if (vm.count("timestamp")) {
		try {
			timestamp_str = vm["timestamp"].as<std::string>();
			timestamp = convert_to_timestamp(timestamp_str);
		} catch (std::exception& e) {
			LOG(FATAL) << "main: invalid timestamp " << e.what();
		}
	}

	std::ifstream ifs(config_file.c_str());
    if (!ifs) {
        std::cout << "can not open config file: " << config_file << std::endl;
        return -1;
    } else {
        boost::program_options::store(parse_config_file(ifs, config_file_options), vm);
        boost::program_options::notify(vm);
    }

    if (vm.count("backup-dirs")) {
		backup_dirs = vm["backup-dirs"].as< std::vector<std::string> >();
    } else {
		std::cout << "no backup dirs" << std::endl;
		return -1;
    }

	if (vm.count("backup-prefix")) {
		backup_prefix = vm["backup-prefix"].as<std::string>();
	}
	
	if (vm.count("backup-database")) {
		db_path = vm["backup-prefix"].as<std::string>();
	}
	
	if (vm.count("s3-access-key")) {
		s3_access_key = vm["s3-access-key"].as<std::string>();
	} else {
		std::cout << "no s3 access key" << std::endl;
		return -1;
	}
	
	if (vm.count("s3-secret-key")) {
		s3_secret_key = vm["s3-secret-key"].as<std::string>();
	} else {
		std::cout << "no s3 secret key" << std::endl;
		return -1;
	}
	
	if (vm.count("s3-bucket-name")) {
		s3_bucket_name = vm["s3-bucket-name"].as<std::string>();
	} else {
		std::cout << "no s3 bucket name" << std::endl;
		return -1;
	}
	
	S3Store remote_store(s3_access_key, s3_secret_key, s3_bucket_name);
	
	if (remote_store.status() == RemoteStore::Invalid) {
		LOG(FATAL) << "main: cannot initialize S3Store";
	}
	
	ParentTask m;
	ThreadPool tp(8, 4);
	for (std::vector<std::string>::iterator iter = backup_dirs.begin(); iter != backup_dirs.end(); ++iter) {
		boost::filesystem::path backup_dir = *iter;
		Task * task = NULL;
		switch (command) {
			case Backup:
				task = new BackupTask(tp, &remote_store, backup_dir, backup_prefix, m);
				if (!task) {
					LOG(FATAL) << "main: new BackupTask failed";
				} else {
					LOG(INFO) << "backup directory: " << backup_dir << " prefix: " << backup_prefix;
				}
				break;
			case Restore:
				task = new RestoreTask(tp, &remote_store, backup_dir, backup_prefix, timestamp, m);
				if (!task) {
					LOG(FATAL) << "main: new RestoreTask failed";
				} else {
					LOG(INFO) << "restore directory: " << backup_dir << " at " << timestamp_str << " with prefix " << backup_prefix ;
				}
				break;
			case List:
				task = new ListTask(&remote_store, backup_dir, backup_prefix, timestamp, m);
				if (!task) {
					LOG(FATAL) << "main: new ListTask failed";
				} else {
					LOG(INFO) << "list directory: " << backup_dir << " at " << timestamp_str << " with prefix " << backup_prefix ;
				}
				break;
		}
	}
	tp.pushs(m.children());
	tp.start();
	m.wait_children();
	tp.stop();
	LOG(INFO) << "done";
	return 0;
}