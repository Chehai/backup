#ifndef REMOTE_STORE_H
#define REMOTE_STORE_H
#include "local_object.h"
#include "remote_object.h"
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
	virtual int lookup(LocalObject&, RemoteObject&);
private:
	Status store_status;
};
#endif