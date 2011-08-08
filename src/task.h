#ifndef TASK_H
#define TASK_H
#include "common.h"
class Task {
public:
	enum Priority {
    	Low,
		High
    };
	enum Status {
		Failed,
		Successful
	};
	Priority priority();
	int set_priority(Priority);
	Status status();
	int set_status(Status);
	Task();
	virtual int run();
	virtual ~Task();
protected:
	Priority task_priority;
	Status task_status;
};
#endif