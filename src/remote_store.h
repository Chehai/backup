#ifndef REMOTE_STORE_H
#define REMOTE_STORE_H
#include "common.h"
#include "remote_object.h"
#include "local_object.h"
class RemoteObject;
class LocalObject;
class RemoteStore {
public:
	enum Status {
    	Invalid = -1,
		Valid = 0
    };
	int set_status(Status);
	Status status();
	RemoteStore();
	virtual ~RemoteStore(); 
	virtual int list(const std::string& prefix, std::list<RemoteObject>& remote_objects);
	int upload(std::list<LocalObject>& local_objects);
	int unload(std::list<RemoteObject>&);
	int download(std::list<RemoteObject>&, const boost::filesystem::path&);
	
	virtual int upload(LocalObject&);
	virtual int unload(RemoteObject&);
	virtual int download(RemoteObject&, const boost::filesystem::path&);
	
private:
	Status store_status;
};
#endif