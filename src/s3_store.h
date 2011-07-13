#ifndef S3_STORE_H
#define S3_STORE_H
#include <libs3.h>
#include <string>
#include "remote_store.h"
class S3Store : public RemoteStore {
public:
	S3Store(std::string&, std::string&, std::string&);
	int lookup(LocalObject&, RemoteObject&);
	std::string& access_key();
	std::string& secret_access_key();
	std::string& bucket_name();
	S3BucketContext& bucket_context();
private:
	std::string s3_access_key;
	std::string s3_secret_access_key;
	std::string s3_bucket_name;
	S3BucketContext s3_bucket_context;
};
#endif