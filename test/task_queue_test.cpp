#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE task_queue_test
#include <boost/test/unit_test.hpp>
#include "../src/task_queue.h"

BOOST_AUTO_TEST_CASE(push_pop_test) 
{
	TaskQueue queue;
	Task t;
	t.set_priority(Task::High);
	queue.push(&t);
	Task * ret;
	ret = queue.pop(Task::High);
	BOOST_CHECK_EQUAL(ret, &t);
	
	queue.push(&t);
	ret = queue.pop(Task::Low);
	BOOST_CHECK_EQUAL(ret, &t);
	
	t.set_priority(Task::Low);
	queue.push(&t);
	ret = queue.pop(Task::Low);
	BOOST_CHECK_EQUAL(ret, &t);
}

BOOST_AUTO_TEST_CASE(pushs_pop_test) 
{
	TaskQueue queue;
	Task t1;
	Task t2;
	t1.set_priority(Task::High);
	t2.set_priority(Task::Low);
	std::list<Task *> ts;
	ts.push_back(&t1);
	ts.push_back(&t2);
	queue.pushs(ts);
	Task * ret;
	ret = queue.pop(Task::High);
	BOOST_CHECK_EQUAL(ret, &t1);
	ret = queue.pop(Task::Low);
	BOOST_CHECK_EQUAL(ret, &t2);
	queue.pushs(ts);
	ret = queue.pop(Task::Low);
	BOOST_CHECK_EQUAL(ret, &t1);
	ret = queue.pop(Task::Low);
	BOOST_CHECK_EQUAL(ret, &t2);
}