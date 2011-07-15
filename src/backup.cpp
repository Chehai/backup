#include "backup.h"
int
Backup::backup()
{
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