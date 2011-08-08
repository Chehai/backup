#ifndef PUT_TASK_H
#define PUT_TASK_H
#include "common.h"
#include "task.h"
#include "backup_task.h"
#include "remote_store.h"
#include "local_object.h"
class BackupTask;
class PutTask : public Task {
public:
	PutTask(RemoteStore *, LocalObject&, BackupTask&);
	int run();
private:
	RemoteStore * remote_store;
	LocalObject& local_object;
	BackupTask& parent_task;
};
#endif