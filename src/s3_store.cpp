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
S3Store::s3_response_properties_callback(const S3ResponseProperties *properties, void *callback_data)
{
    return S3StatusOK;
}

void 
S3Store::s3_response_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
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
		std::string full_key = contents[i].key;
		const boost::regex object_key_pattern("\\A(.*)\\.(\\d+)\\.([a-z])\\z");
		boost::smatch what; 
		if (boost::regex_match(full_key, what, object_key_pattern)) { 
			if (what[0].matched) {
				std::string key, updated_at_str;
				key.assign(what[1].first, what[1].second);
				updated_at_str.assign(what[2].first, what[2].second);
				std::time_t updated_at = boost::lexical_cast<std::time_t>(updated_at_str);
				char action = *what[3].first;
				RemoteObject ro(key, updated_at, action);
				data->s3_objects.push_back(ro);
			}
		} 
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
        { &s3_response_properties_callback, &s3_response_complete_callback },
        &s3_list_bucket_callback
    };
    
	S3_list_bucket(&s3_bucket_context, prefix.c_str(), NULL, NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	while (list_bucket_callback_data.is_truncated) {
		S3_list_bucket(&s3_bucket_context, prefix.c_str(), list_bucket_callback_data.next_marker.c_str(), NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	}
	return 0;
}

S3Store::S3UploadObjectCallbackData::S3UploadObjectCallbackData(std::ifstream& f, std::streamsize s)
: read_count(0), file_size(s), file(f)
{
}

int
S3Store::s3_upload_object_callback(int buffer_size, char * buffer, void * callback_data)
{
	S3UploadObjectCallbackData * upload_object_callback_data = (S3UploadObjectCallbackData *)callback_data;
	
	if (upload_object_callback_data->read_count == upload_object_callback_data->file_size) {
		return 0;
	}
	std::streamsize count = upload_object_callback_data->file.readsome(buffer, buffer_size);
	if (count < 0) {
		return -1;
	}
	upload_object_callback_data->read_count += count;
	return count;
}

int
S3Store::upload(LocalObject& lo)
{	
    S3PutObjectHandler upload_object_handler =
    {
        { &s3_response_properties_callback, &s3_response_complete_callback },
        &s3_upload_object_callback
    };
	std::string key = lo.uri();
	key += '.';
	key += boost::lexical_cast<std::string>(lo.updated_at());
	key += ".u";
	std::ifstream local_file;
	local_file.open(lo.fs_path().string().c_str(), std::ios::binary);
	S3Store::S3UploadObjectCallbackData upload_object_callback_data(local_file, lo.size());
	S3_put_object(&s3_bucket_context, key.c_str(), lo.size(), NULL, NULL, &upload_object_handler, &upload_object_callback_data);
	local_file.close();
	return 0;
}

S3Store::S3UnloadObjectCallbackData::S3UnloadObjectCallbackData(std::string& key)
: content(key), read_count(0)
{
}

int
S3Store::s3_unload_object_callback(int buffer_size, char * buffer, void * callback_data)
{
	S3UnloadObjectCallbackData * unload_object_callback_data = (S3UnloadObjectCallbackData *)callback_data;
	
	if (unload_object_callback_data->read_count == unload_object_callback_data->content.length()) {
		return 0;
	}
	std::size_t count = unload_object_callback_data->content.copy(buffer, buffer_size, unload_object_callback_data->read_count);
	unload_object_callback_data->read_count += count;
	return count;
}

int
S3Store::unload(RemoteObject& ro)
{
    S3PutObjectHandler unload_object_handler =
    {
        { &s3_response_properties_callback, &s3_response_complete_callback },
        &s3_unload_object_callback
    };
	std::string key = ro.uri();
	key += '.';
	key += boost::lexical_cast<std::string>(std::time(NULL));
	key += ".d";
	S3Store::S3UnloadObjectCallbackData unload_object_callback_data(key);
	S3_put_object(&s3_bucket_context, key.c_str(), key.length(), NULL, NULL, &unload_object_handler, &unload_object_callback_data);
	return 0;	
}
