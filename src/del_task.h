#ifndef DEL_TASK_H
#define DEL_TASK_H
#include "common.h"
#include "task.h"
#include "remote_store.h"
#include "remote_object.h"
#include "backup_task.h"
class BackupTask;
class DelTask : public Task {
public:
	DelTask(RemoteStore *, RemoteObject&, BackupTask&);
	int run();
private:
	RemoteStore * remote_store;
	RemoteObject& remote_object;
	BackupTask& parent_task;	
};
#endif