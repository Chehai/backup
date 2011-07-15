#ifndef REMOTE_OBJECT_H
#define REMOTE_OBJECT_H
#include "backup_object.h"
class RemoteObject : public BackupObject {
public:
	RemoteObject(std::string&, Timestamp&);
	RemoteObject();
	
private:
};
#endif