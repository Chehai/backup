#include "s3_store.h"
unsigned int S3Store::s3_store_usage_count = 0;

S3Store::S3Store(std::string& ak, std::string& sak, std::string& bn)
{
	S3Status status;
	if (!s3_store_usage_count) {
		if ((status = S3_initialize("s3", S3_INIT_ALL)) != S3StatusOK) {
			std::cout << "S3Store::S3Store: Error: " << S3_get_status_name(status) << std::endl;
			set_status(S3Store::Invalid);
			return;
	    }
	}
	s3_access_key = ak;
	s3_secret_access_key = sak;
	s3_bucket_name = bn;
	s3_bucket_context.bucketName = s3_bucket_name.c_str();
	s3_bucket_context.protocol = S3ProtocolHTTPS;
	s3_bucket_context.uriStyle = S3UriStylePath;
	s3_bucket_context.accessKeyId = s3_access_key.c_str();
	s3_bucket_context.secretAccessKey = s3_secret_access_key.c_str();
	++s3_store_usage_count;
}

S3Store::~S3Store()
{
	--s3_store_usage_count;
	if (!s3_store_usage_count) {
		S3_deinitialize();
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

S3Store::S3ListBucketCallbackData::S3ListBucketCallbackData(std::list<RemoteObject>& objects)
: is_truncated(false), next_marker(""), objects_count(0), s3_objects(objects)
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
		std::time_t object_last_modified_at = contents[i].lastModified;
		RemoteObject ro(object_key, object_last_modified_at);
		data->s3_objects.push_back(ro);
	}
	data->objects_count += contents_count;
    return S3StatusOK;
}

int
S3Store::list(const std::string& prefix, std::list<RemoteObject>& remote_objects)
{
	S3Store::S3ListBucketCallbackData list_bucket_callback_data(remote_objects);
	
	S3ListBucketHandler list_bucket_handler =
    {
        { &s3_list_bucket_properties_callback, &s3_list_bucket_complete_callback },
        &s3_list_bucket_callback
    };
    
	S3_list_bucket(&s3_bucket_context, prefix.c_str(), NULL, NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	while (list_bucket_callback_data.is_truncated) {
		S3_list_bucket(&s3_bucket_context, prefix.c_str(), list_bucket_callback_data.next_marker.c_str(), NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	}
	return 0;
}