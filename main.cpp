#include "common.h"
#include "src/backup_task.h"
#include "src/restore_task.h"
#include "src/s3_store.h"

void failure_function_for_glog() {
	std::cerr << "exiting..." << std::endl;
 	exit(-1);
}

int
main(int argc, char** argv)
{
	google::InitGoogleLogging(argv[0]);
	google::InstallFailureFunction(&failure_function_for_glog);
  
	std::string config_file;
	std::vector<std::string> backup_dirs;
	std::string backup_prefix = "";
	std::string s3_access_key = "";
	std::string s3_secret_key = "";
	std::string s3_bucket_name = "";
	boost::filesystem::path db_path = "objects.db";
	bool restore = false;
	
	boost::program_options::options_description basic_options("Basic Options");
    basic_options.add_options()
        ("help,h", "Show help message")
		("restore,r", "Restore")
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
		LOG(FATAL) << "parse program options error: " << err.what();
	}
	if (vm.count("help")) {
		std::cout << "./my_backup --help|-h --config|-c --backup-dir|-b --backup-prefix|-p --backup-database|-d" << std::endl;
		return 0;
	}
	
	if (vm.count("restore")) {
		restore = true;
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
	std::time_t now = std::time(NULL);	
	for (std::vector<std::string>::iterator iter = backup_dirs.begin(); iter != backup_dirs.end(); ++iter) {
		boost::filesystem::path backup_dir = *iter;
		if (restore) {
			new RestoreTask(tp, &remote_store, backup_dir, backup_prefix, now, m);
			LOG(INFO) << "restore directory: " << backup_dir << " prefix: " << backup_prefix;
		} else {
			new BackupTask(tp, &remote_store, backup_dir, backup_prefix, m);
			LOG(INFO) << "backup directory: " << backup_dir << " prefix: " << backup_prefix;
		}
	}
	tp.pushs(m.children());
	tp.start();
	m.wait_children();
	LOG(INFO) << "done";
	return 0;
}