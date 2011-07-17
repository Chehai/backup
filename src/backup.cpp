#include "backup.h"
#include <list>
#include <boost/filesystem.hpp>
int
Backup::backup(LocalObject& local_object, RemoteStore * remote_store)
{
	boost::system::error_code err;
	boost::filesystem::path local_object_path = local_object.fs_path();
	
	if (boost::filesystem::exists(local_object_path, err)) {
		if (err.value()) {
			std::cout << "Backup::backup: Error: " << err.message() << std::endl;
			return -1;
		}
		if (boost::filesystem::is_regular_file(local_object_path)) {
			local_objects.push_back(local_object);
		} else if (boost::filesystem::is_directory(local_object_path)) {
			boost::filesystem::recursive_directory_iterator iter(local_object_path), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					local_objects.push_back(LocalObject(iter->path()));
				}
			}
		} else {
			std::cout << "Backup::backup: Error: Do not support file type: " << local_object_path.string() << std::endl;
		}
	}
	
	// if local_object is a directory
	// list all sub objects, run backup for each one of them
	// enqueue the local object
	// dequeue the first element
	// get all sub objects from the dequeued element
	// if a sub object is a file, put it into a another list
	// the list is monitored by a pool of upload threads. They can pick it up to upload.
	// if a sub object is a directory, put it back to the queue
	// loop to dequeue line until the queue is empty.
// 	remote_store->lookup(local_object, remote_object);
// 	if (remote_object.updated_at() < local_object.updated_at()) {
// 		remote_store->upload(local_object, remote_object);
// 	}
	return 0;
}