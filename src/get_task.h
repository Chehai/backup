#ifndef GET_TASK_H
#define GET_TASK_H
#include "common.h"
#include "task.h"
#include "restore_task.h"
#include "remote_store.h"
#include "remote_object.h"
class RestoreTask;
class GetTask : public Task {
public:
	GetTask(RemoteStore *, RemoteObject&, boost::filesystem::path&, RestoreTask&, bool, std::string&);
	int run();
private:
	RemoteStore * remote_store;
	RemoteObject& remote_object;
	RestoreTask& parent_task;
	boost::filesystem::path& restore_dir;
	bool use_gpg;
	std::string gpg_recipient;
};
#endif