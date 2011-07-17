#ifndef S3_STORE_H
#define S3_STORE_H
#include <libs3.h>
#include <string>
#include <map>
#include "remote_store.h"
class S3Store : public RemoteStore {
public:
	S3Store(std::string&, std::string&, std::string&);
	int lookup(LocalObject&, RemoteObject&);
	std::string& access_key();
	std::string& secret_access_key();
	std::string& bucket_name();
	S3BucketContext& bucket_context();
	typedef std::map<std::string, Timestamp> S3Objects;
	static int insert_into_objects(std::string&, Timestamp&);
	int set_local_uri(LocalObject& root, LocalObject& lo);
	~S3Store();
private:
	class S3ListBucketCallbackData {
	public:
	    bool is_truncated;
	    std::string next_marker;
	    unsigned int objects_count;
		S3ListBucketCallbackData(); 
	};
	std::string s3_access_key;
	std::string s3_secret_access_key;
	std::string s3_bucket_name;
	S3BucketContext s3_bucket_context;
	static S3Objects s3_objects;
	static bool s3_is_initialized;
	int s3_prefetch(std::string&);
	static S3Status s3_list_bucket_properties_callback(const S3ResponseProperties *, void *);
	static void s3_list_bucket_complete_callback(S3Status, const S3ErrorDetails *, void *);
	static S3Status s3_list_bucket_callback(int isTruncated, const char *nextMarker, int contentsCount, const S3ListBucketContent *contents, \
											int commonPrefixesCount, const char **commonPrefixes, void *callbackData);
};
#endif