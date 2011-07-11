#ifndef REMOTE_STORE_H
#define REMOTE_STORE_H
#include "local_object.h"
#include "remote_object.h"
class RemoteStore {
public:
	virtual ~RemoteStore(); 
	int lookup(LocalObject&, RemoteObject&);
protected:
};
#endif