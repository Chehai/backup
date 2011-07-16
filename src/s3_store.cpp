#include <iostream>
#include "s3_store.h"
S3Store::S3Objects S3Store::s3_objects;
bool S3Store::s3_is_initialized = false;

S3Store::S3Store(std::string& ak, std::string& sak, std::string& bn)
{
	S3Status status;
	if (!s3_is_initialized) {
		if ((status = S3_initialize("s3", S3_INIT_ALL)) != S3StatusOK) {
			std::cout << "S3Store::S3Store: Error: " << S3_get_status_name(status) << std::endl;
			set_status(S3Store::Invalid);
			return;
	    }
		s3_is_initialized = true;
	}
	s3_access_key = ak;
	s3_secret_access_key = sak;
	s3_bucket_name = bn;
	s3_bucket_context.bucketName = s3_bucket_name.c_str();
	s3_bucket_context.protocol = S3ProtocolHTTPS;
	s3_bucket_context.uriStyle = S3UriStylePath;
	s3_bucket_context.accessKeyId = s3_access_key.c_str();
	s3_bucket_context.secretAccessKey = s3_secret_access_key.c_str();
}

S3Store::~S3Store()
{
	if (s3_is_initialized) {
		S3_deinitialize();
		s3_is_initialized = false;
	}
}

std::string&
S3Store::access_key()
{
	return s3_access_key;
}

std::string&
S3Store::secret_access_key()
{
	return s3_secret_access_key;
}

std::string&
S3Store::bucket_name()
{
	return s3_bucket_name;
}

S3BucketContext&
S3Store::bucket_context()
{
	return s3_bucket_context;
}

int 
S3Store::insert_into_objects(std::string& key, Timestamp& value)
{
	s3_objects[key] = value;
	return 0;
}



int
S3Store::lookup(LocalObject& lo, RemoteObject& ro)
{
	S3Store::S3Objects::iterator iter;
	std::string path = lo.path();
	// need lock below
	iter = s3_objects.find(path);
 	if (iter == s3_objects.end()) {
		s3_prefetch(path);
		iter = s3_objects.find(path);
		if (iter == s3_objects.end()) {
			ro.set_status(BackupObject::Invalid);
			// need unlock
			return -1;
		} 
	}
	// unlock
	ro = RemoteObject(path, iter->second);
	return 0;
}

S3Status
S3Store::s3_list_bucket_properties_callback(const S3ResponseProperties *properties, void *callback_data)
{
    return S3StatusOK;
}

void 
S3Store::s3_list_bucket_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
{
    if (error && error->message) {
		std::cout << "S3Store::s3_list_bucket_complete_callback: Error: Message: " << error->message << std::endl;
        std::cout << "S3Store::s3_list_bucket_complete_callback: Error: Resource: " << error->resource << std::endl;
		std::cout << "S3Store::s3_list_bucket_complete_callback: Error: Further Detail: " << error->furtherDetails << std::endl;
	}
}

S3Store::S3ListBucketCallbackData::S3ListBucketCallbackData()
: is_truncated(false), next_marker(""), objects_count(0)
{
	
}

S3Status
S3Store::s3_list_bucket_callback(int is_truncated, const char *next_marker, int contents_count, const S3ListBucketContent *contents, \
                                 int common_prefixes_count, const char **common_prefixes, void *callback_data)
{
    S3ListBucketCallbackData * data = (S3ListBucketCallbackData *) callback_data;
	
	data->is_truncated = is_truncated;
	if (data->is_truncated) {
		data->next_marker = next_marker ? next_marker : contents[contents_count - 1].key;
	}

	for (int i = 0; i < contents_count; ++i) {
		std::string object_key = contents[i].key;
		Timestamp object_last_modified_at = boost::posix_time::from_time_t(contents[i].lastModified);
		if (s3_objects.find(object_key) ==  s3_objects.end()) {
			s3_objects[object_key] = object_last_modified_at;
		} else {
			std::cout << "S3Store::s3_list_bucket_callback: Error: Duplicates found" << std::endl;
		}
	}
	data->objects_count += contents_count;
    return S3StatusOK;
}



int
S3Store::s3_prefetch(std::string& path)
{
	S3Store::S3ListBucketCallbackData list_bucket_callback_data;
	
	S3ListBucketHandler list_bucket_handler =
    {
        { &s3_list_bucket_properties_callback, &s3_list_bucket_complete_callback },
        &s3_list_bucket_callback
    };
    
	std::string prefix = path.substr(0, path.find('/'));
	S3_list_bucket(&s3_bucket_context, prefix.c_str(), NULL, NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	while (list_bucket_callback_data.is_truncated) {
		S3_list_bucket(&s3_bucket_context, prefix.c_str(), list_bucket_callback_data.next_marker.c_str(), NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	}
	return 0;
}