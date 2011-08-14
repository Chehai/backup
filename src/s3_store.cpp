#include "s3_store.h"
unsigned int S3Store::s3_store_usage_count = 0;

S3Store::S3Store(std::string& ak, std::string& sak, std::string& bn)
{
	S3Status status;
	if (!s3_store_usage_count) {
		if ((status = S3_initialize("s3", S3_INIT_ALL)) != S3StatusOK) {
			LOG(ERROR) << "S3Store::S3Store: constructor " << S3_get_status_name(status);
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
S3Store::s3_list_bucket_response_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
{
    if (error && error->message) {
	    S3ListBucketCallbackData * data = (S3ListBucketCallbackData *) callback_data;
		data->is_successful = false;
		LOG(ERROR) << "S3Store::s3_list_bucket_response_complete_callback: message: " << error->message;
        LOG(ERROR) << "S3Store::s3_list_bucket_response_complete_callback: resource: " << error->resource;
		LOG(ERROR) << "S3Store::s3_list_bucket_response_complete_callback: further detail: " << error->furtherDetails;
	}
}

S3Store::S3ListBucketCallbackData::S3ListBucketCallbackData(std::list<RemoteObject>& objects)
: is_successful(true), is_truncated(false), next_marker(""), objects_count(0), s3_objects(objects)
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
				RemoteObject ro(key, updated_at, action, contents[i].size);
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
        { &s3_response_properties_callback, &s3_list_bucket_response_complete_callback },
        &s3_list_bucket_callback
    };
    LOG(INFO) << "S3Store::list start list " << prefix;
	S3_list_bucket(&s3_bucket_context, prefix.c_str(), NULL, NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	while (list_bucket_callback_data.is_truncated) {
		S3_list_bucket(&s3_bucket_context, prefix.c_str(), list_bucket_callback_data.next_marker.c_str(), NULL, 2000, NULL, &list_bucket_handler, &list_bucket_callback_data);
	}
	if (list_bucket_callback_data.is_successful) {
		LOG(INFO) << "S3Store::list finish list " << prefix;
		return 0;
	} else {
		LOG(ERROR) << "S3Store::list failed list " << prefix;
		return -1;
	}
}

void 
S3Store::s3_put_response_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
{
    if (error && error->message) {
		S3PutObjectCallbackData * data = (S3PutObjectCallbackData *)callback_data;
		data->local_object.set_status(BackupObject::Invalid);
		LOG(ERROR) << "S3Store::s3_put_response_complete_callback: message: " << error->message;
        LOG(ERROR) << "S3Store::s3_put_response_complete_callback: resource: " << error->resource;
		LOG(ERROR) << "S3Store::s3_put_response_complete_callback: further detail: " << error->furtherDetails;
	}
}

S3Store::S3PutObjectCallbackData::S3PutObjectCallbackData(std::ifstream& f, std::size_t s, LocalObject& lo)
: read_count(0), file_size(s), file(f), local_object(lo)
{
}

int
S3Store::s3_put_object_callback(int buffer_size, char * buffer, void * callback_data)
{
	S3PutObjectCallbackData * put_object_callback_data = (S3PutObjectCallbackData *)callback_data;
	
	if (put_object_callback_data->read_count == put_object_callback_data->file_size) {
		return 0;
	}
	std::streamsize count = put_object_callback_data->file.readsome(buffer, buffer_size);
	if (put_object_callback_data->file.fail()) {
		LocalObject& lo = put_object_callback_data->local_object;
		lo.set_status(BackupObject::Invalid);
		LOG(ERROR) << "S3Store::s3_put_object_callback ifstream::readsome " << lo.fs_path() << " " << strerror(errno);
		return -1;
	}
	put_object_callback_data->read_count += count;
	return count;
}

std::string 
S3Store::s3_object_key(const LocalObject& lo)
{
	std::string key = lo.uri();
	key += '.';
	key += boost::lexical_cast<std::string>(lo.updated_at());
	key += ".u";
	return key;
}

std::string 
S3Store::s3_object_key(const RemoteObject& ro)
{
	std::string key = ro.uri();
	key += '.';
	key += boost::lexical_cast<std::string>(ro.updated_at());
	key += '.';
	key += ro.action();
	return key;
}


int
S3Store::put(LocalObject& lo)
{	
    S3PutObjectHandler put_object_handler =
    {
        { &s3_response_properties_callback, &s3_put_response_complete_callback },
        &s3_put_object_callback
    };
	std::string key = s3_object_key(lo); 	
	std::ifstream local_file;
	local_file.open(lo.fs_path().string().c_str(), std::ios::binary);
	if (local_file.fail()) {
		LOG(ERROR) << "S3Store::put ifstream::open " << lo.fs_path() << " " << strerror(errno);
		return -1;
	}
	LOG(INFO) << "S3Store::put start upload " << lo.fs_path();
	S3Store::S3PutObjectCallbackData put_object_callback_data(local_file, lo.size(), lo);
	S3_put_object(&s3_bucket_context, key.c_str(), lo.size(), NULL, NULL, &put_object_handler, &put_object_callback_data);
	local_file.close();
	if (lo.status() == BackupObject::Invalid) {
		LOG(INFO) << "S3Store::put failed upload " << lo.fs_path();
		return -1;
	} else {
		LOG(INFO) << "S3Store::put finish upload " << lo.fs_path();
		return 0;
	}
}

void 
S3Store::s3_del_response_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
{
    if (error && error->message) {
		S3DelObjectCallbackData * data = (S3DelObjectCallbackData *)callback_data;
		data->remote_object.set_status(BackupObject::Invalid);
		LOG(ERROR) << "S3Store::s3_del_response_complete_callback: message: " << error->message;
        LOG(ERROR) << "S3Store::s3_del_response_complete_callback: resource: " << error->resource;
		LOG(ERROR) << "S3Store::s3_del_response_complete_callback: further detail: " << error->furtherDetails;
	}
}

S3Store::S3DelObjectCallbackData::S3DelObjectCallbackData(std::string& key, RemoteObject& ro)
: content(key), read_count(0), remote_object(ro)
{
}

int
S3Store::s3_del_object_callback(int buffer_size, char * buffer, void * callback_data)
{
	S3DelObjectCallbackData * del_object_callback_data = (S3DelObjectCallbackData *)callback_data;
	
	if (del_object_callback_data->read_count == del_object_callback_data->content.length()) {
		return 0;
	}
	std::size_t count = del_object_callback_data->content.copy(buffer, buffer_size, del_object_callback_data->read_count);
	del_object_callback_data->read_count += count;
	return count;
}

int
S3Store::del(RemoteObject& ro)
{
    S3PutObjectHandler del_object_handler =
    {
        { &s3_response_properties_callback, &s3_del_response_complete_callback },
        &s3_del_object_callback
    };
	std::string key = ro.uri();
	key += '.';
	key += boost::lexical_cast<std::string>(std::time(NULL));
	key += ".d";
	LOG(INFO) << "S3Store::del start delete " << ro.uri();
	S3Store::S3DelObjectCallbackData del_object_callback_data(key, ro);
	S3_put_object(&s3_bucket_context, key.c_str(), key.length(), NULL, NULL, &del_object_handler, &del_object_callback_data);
	if (ro.status() == BackupObject::Invalid) {
		LOG(ERROR) << "S3Store::del failed delete " << ro.uri();
		return -1;
	} else {
		LOG(INFO) << "S3Store::del finish delete " << ro.uri();
		return 0;	
	}	
}

void 
S3Store::s3_get_response_complete_callback(S3Status status, const S3ErrorDetails *error, void *callback_data)
{
    if (error && error->message) {
		S3GetObjectCallbackData * data = (S3GetObjectCallbackData *)callback_data;
		data->remote_object.set_status(BackupObject::Invalid);
		LOG(ERROR) << "S3Store::s3_get_response_complete_callback: message: " << error->message;
        LOG(ERROR) << "S3Store::s3_get_response_complete_callback: resource: " << error->resource;
		LOG(ERROR) << "S3Store::s3_get_response_complete_callback: further detail: " << error->furtherDetails;
	}
}

S3Store::S3GetObjectCallbackData::S3GetObjectCallbackData(std::ofstream& f, RemoteObject& ro) 
: file(f), remote_object(ro)
{	
}


S3Status
S3Store::s3_get_object_callback(int buffer_size, const char * buffer, void * callback_data)
{
	S3GetObjectCallbackData * get_object_callback_data = (S3GetObjectCallbackData *)callback_data;
	
	get_object_callback_data->file.write(buffer, buffer_size);
	if (get_object_callback_data->file.fail()) {
		RemoteObject& ro = get_object_callback_data->remote_object;
		ro.set_status(BackupObject::Invalid);
		LOG(ERROR) << "S3Store::s3_get_object_callback ifstream::write " << ro.uri() << " " << strerror(errno);
		return S3StatusAbortedByCallback;
	}
	
	return S3StatusOK;
}

int 
S3Store::get(RemoteObject& ro, const boost::filesystem::path& dir)
{
	S3GetObjectHandler get_object_handler = 
    {
        { &s3_response_properties_callback, &s3_get_response_complete_callback },
        &s3_get_object_callback
    };
	std::string key = s3_object_key(ro);
	boost::filesystem::path p = dir.parent_path();
	p /= ro.uri();
	boost::system::error_code err;

	if (boost::filesystem::exists(p, err) && !err.value()) {
		err.clear();
		std::size_t file_size = boost::filesystem::file_size(p, err);
		if (!err.value() && file_size == ro.size()) {
			LOG(INFO) << "S3Store::get finish get (already exists)" << ro.uri();
			return 0;
		}
	}
	
	err.clear();
	boost::filesystem::create_directories(p.parent_path(), err);
	if (err.value()) {
		LOG(ERROR) << "S3Store::get: create_directories " << err.message();
		return -1;
	}
	std::ofstream local_file;
	local_file.open(p.string().c_str(), std::ios::binary | std::ios::trunc);
	if (local_file.fail()) {
		LOG(ERROR) << "S3Store::get ofstream::open " << ro.uri() << " " << strerror(errno);
		return -1;
	}
	LOG(INFO) << "S3Store::get start get " << ro.uri();
	S3Store::S3GetObjectCallbackData get_object_callback_data(local_file, ro);
	S3_get_object(&s3_bucket_context, key.c_str(), NULL, 0, 0, NULL, &get_object_handler, &get_object_callback_data);
	local_file.close();
	if (ro.status() == BackupObject::Invalid) {
		LOG(ERROR) << "S3Store::get failed get " << ro.uri();
		return -1;
	} else {
		LOG(INFO) << "S3Store::get finish get " << ro.uri();
		return 0;
	}
}

