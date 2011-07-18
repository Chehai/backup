#include "backup.h"

int
Backup::backup(boost::filesystem::path& backup_parent_dir, boost::filesystem::path& backup_dir_prefix)
{
	boost::filesystem::path local_dir = backup_parent_dir;
	local_dir /= backup_dir_prefix;
	
	boost::system::error_code err;
	if (boost::filesystem::exists(local_dir, err)) {
		if (err.value()) {
			std::cout << "Backup::backup: Error: " << err.message() << std::endl;
			return -1;
		}
		if (boost::filesystem::is_directory(local_dir)) {
			LocalObject::populate_local_objects_table(local_dir, backup_dir_prefix); // multi-thread
			RemoteObject::populate_remote_objects_table(remote_store, backup_dir_prefix);
			// backup
		} else {
			return -1;
		}
			
	} else {
		return -1;
	}
}

// int
// Backup::backup(LocalObject& local_object, RemoteStore * remote_store)
// {
// 	boost::system::error_code err;
// 	boost::filesystem::path local_object_path = local_object.fs_path();
// 	
// 	if (boost::filesystem::exists(local_object_path, err)) {
// 		if (err.value()) {
// 			std::cout << "Backup::backup: Error: " << err.message() << std::endl;
// 			return -1;
// 		}
// 		if (boost::filesystem::is_regular_file(local_object_path)) {
// 			local_objects.push_back(local_object);
// 		} else if (boost::filesystem::is_directory(local_object_path)) {
// 			boost::filesystem::recursive_directory_iterator iter(local_object_path), end_of_dir;
// 			for (; iter != end_of_dir; ++iter) {
// 				if (boost::filesystem::is_regular_file(iter->path())) {
// 					LocalObject lo = LocalObject(iter->path());
// 					remote_store->set_local_uri(local_object, lo);
// 					local_objects.push_back(lo);
// 				}
// 			}
// 		} else {
// 			std::cout << "Backup::backup: Error: Do not support file type: " << local_object_path.string() << std::endl;
// 		}
// 	}
// 	
// 	for (std::list<LocalObject>::iterator iter = local_objects.begin(); iter != local_objects.end(); ++iter) {
// 		RemoteObject remote;
// 		LocalObject& local = *iter;
// 		if (remote_store->lookup(local, remote)) {
// 			std::cout << "Backup::backup: Error: backup " << local.fs_path().string() << std::endl;
// 		} else {
// 			if (remote.updated_at() < local.updated_at()) {
// 				//remote_store->upload(local, remote);
// 			}
// 		}
// 	}
// 	
// 	// if local_object is a directory
// 	// list all sub objects, run backup for each one of them
// 	// enqueue the local object
// 	// dequeue the first element
// 	// get all sub objects from the dequeued element
// 	// if a sub object is a file, put it into a another list
// 	// the list is monitored by a pool of upload threads. They can pick it up to upload.
// 	// if a sub object is a directory, put it back to the queue
// 	// loop to dequeue line until the queue is empty.
// // 	remote_store->lookup(local_object, remote_object);
// // 	if (remote_object.updated_at() < local_object.updated_at()) {
// // 		remote_store->upload(local_object, remote_object);
// // 	}
// 	return 0;
// }