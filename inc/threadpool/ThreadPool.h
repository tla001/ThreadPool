#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

extern "C"{
	#include <stdio.h>

	#include <pthread.h>
	#include <unistd.h>
	#include <sys/types.h>
}

#include <queue>
#include <string>
#include <map>
#include <vector>

using namespace std;


class Task{
public:
	string taskName;

public:
	Task(){}
	Task(string _taskName):taskName(_taskName){
		priority=1;
	}
	void setPriority(int pri){
		priority=pri;
	}
	int getPriority(){
		return priority;
	}

	virtual ~Task(){}

	virtual void run()=0;
public:
	int priority;
};
struct TaskCom{
	bool operator()(const Task* t1,const Task* t2){
		return t1->priority<t2->priority;
	}
};
class CbTask:public Task{//回调版本的task
public:
	CbTask(string name,int pri):Task(name){
		setPriority(pri);
	}
	void setCallBack(void *(*_process) (void *_arg), void *_arg){
		process=_process;
		arg=_arg;
	}
	void run(){
		 (*process) (arg);
	}
private:
	void*(*process)(void *arg);
	void *arg;
};

class ThreadPool{
private:
	static priority_queue<Task*,vector<Task*>,TaskCom > taskList;//带优先级
	static map<pthread_t,int> threads;
	bool shutdown;
	int maxThreadNum;
	int ThreadNum;

	static pthread_mutex_t mutex;
	static pthread_mutex_t map_mutex;
	static pthread_cond_t cond;

protected:
	static void *threadRoutine(void *arg);
	static void setThreadStat(pthread_t tid,int stat);
public:
	void poolInit();
	void poolDestroy();
	void addThread();
	void delThread(int n);
	void addTask(Task *task);
	int getTaskListSize();
	int getPoolSize();

	ThreadPool(int _threadNum);
	~ThreadPool();
enum ThreadStat{
	THREAD_RUN=1,
	THREAD_WAIT,
	THREAD_SHUT
};


};


#endif
