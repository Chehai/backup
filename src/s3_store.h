#ifndef S3_STORE_H
#define S3_STORE_H
#include "common.h"
#include "remote_store.h"
class S3Store : public RemoteStore {
public:
	S3Store(std::string&, std::string&, std::string&);
	int list(const std::string& prefix, std::list<RemoteObject>& remote_objects);
	int upload(LocalObject& lo);
	std::string& access_key();
	std::string& secret_access_key();
	std::string& bucket_name();
	S3BucketContext& bucket_context();
	~S3Store();
private:
	class S3ListBucketCallbackData {
	public:
	    bool is_truncated;
	    std::string next_marker;
	    unsigned int objects_count;
		std::list<RemoteObject>& s3_objects;
		S3ListBucketCallbackData(std::list<RemoteObject>&); 
	};
	std::string s3_access_key;
	std::string s3_secret_access_key;
	std::string s3_bucket_name;
	S3BucketContext s3_bucket_context;
	static unsigned int s3_store_usage_count;
	static S3Status s3_response_properties_callback(const S3ResponseProperties *, void *);
	static void s3_response_complete_callback(S3Status, const S3ErrorDetails *, void *);
	static int s3_upload_object_callback(int bufferSize, char *buffer, void *callbackData);
	static S3Status s3_list_bucket_callback(int isTruncated, const char *nextMarker, int contentsCount, const S3ListBucketContent *contents, \
											int commonPrefixesCount, const char **commonPrefixes, void *callbackData);
};
#endif