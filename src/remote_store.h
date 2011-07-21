#ifndef REMOTE_STORE_H
#define REMOTE_STORE_H
#include "common.h"
#include "remote_object.h"
class RemoteObject;
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
private:
	Status store_status;
};
#endif