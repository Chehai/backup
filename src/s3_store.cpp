#include <iostream>
#include "s3_store.h"

S3Store::S3Store(std::string& ak, std::string& sak, std::string& bn)
{
	S3Status status;

    if ((status = S3_initialize("s3", S3_INIT_ALL)) != S3StatusOK) {
		std::cout << "S3Store::S3Store: Failed to initialize libs3: " << S3_get_status_name(status) << std::endl;
		set_status(S3Store::Invalid);
    } else {
		s3_access_key = ak;
		s3_secret_access_key = sak;
		s3_bucket_name = bn;
		s3_bucket_context.bucketName = s3_bucket_name.c_str();
		s3_bucket_context.protocol = S3ProtocolHTTPS;
		s3_bucket_context.uriStyle = S3UriStylePath;
		s3_bucket_context.accessKeyId = s3_access_key.c_str();
		s3_bucket_context.secretAccessKey = s3_secret_access_key.c_str();
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
S3Store::lookup(LocalObject& lo, RemoteObject& ro)
{
	// let us do list bucket objects first, then put the list of objects into some place.
	// get the last modified date from the list
	// in order to cut s3 cost
	// S3_list_bucket(&s3_context, lo.path().c_str(), data.nextMarker,
	//                        delimiter, maxkeys, 0, &listBucketHandler, &data);
	return 0;
}