#include "remote_store.h"

RemoteStore::RemoteStore()
{
	set_status(RemoteStore::Valid);
}

RemoteStore::~RemoteStore()
{
}

int 
RemoteStore::set_status(RemoteStore::Status st)
{
	store_status = st;
	return 0;
}

RemoteStore::Status
RemoteStore::status()
{
	return store_status;
}

int
RemoteStore::lookup(LocalObject& lo, RemoteObject& ro)
{
	return 0;
}