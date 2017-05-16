#include "ThreadPool.h"


priority_queue<Task*,vector<Task*>,TaskCom> ThreadPool::taskList;
map<pthread_t,int> ThreadPool::threads;

pthread_mutex_t ThreadPool::mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::map_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::cond=PTHREAD_COND_INITIALIZER;

ThreadPool::ThreadPool(int _threadNum):maxThreadNum(_threadNum){
	poolInit();
}
ThreadPool::~ThreadPool(){
	poolDestroy();
}
void *ThreadPool::threadRoutine(void *arg){
	pthread_t tid=pthread_self();
	pthread_mutex_lock(&map_mutex);
	threads.insert(make_pair(tid,THREAD_WAIT));
	int &threadStat=threads[tid];
	pthread_mutex_unlock(&map_mutex);
	while(1){
		pthread_mutex_lock(&mutex);
		while(taskList.size()==0&&threadStat==THREAD_WAIT){
			pthread_cond_wait(&cond,&mutex);
		}
		if(threadStat==THREAD_SHUT){
			pthread_mutex_unlock(&mutex);
			printf("thread %lu will exit\n",tid);
			pthread_exit(NULL);
		}
//		printf("task num=%d\n",taskList.size());
		Task *task=taskList.top();
		taskList.pop();
//		printf("task num=%d\n",taskList.size());
		setThreadStat(tid,THREAD_RUN);
		printf("thread %lu is running with task--> %s*** %d\n",tid,task->taskName.c_str(),task->getPriority());
		pthread_mutex_unlock(&mutex);

		task->run();
		setThreadStat(tid,THREAD_WAIT);

		printf("thread %lu has done with task--> %s\n",tid,task->taskName.c_str());

	}
	return NULL;
}
void ThreadPool::setThreadStat(pthread_t tid,int stat){
	threads[tid]=stat;
}
void ThreadPool::addThread(){
	pthread_t tid;
	pthread_create(&tid,NULL,threadRoutine,NULL);
	ThreadNum++;
}

void ThreadPool::delThread(int n){
	int num=0;
	int size=getPoolSize();
	if(n>size){
		printf("pool size is less than you input\n");
		return;
	}
	while(num<n){
		for(map<pthread_t,int>::iterator ite=threads.begin();ite!=threads.end();){
			if(ite->second==THREAD_WAIT){
				setThreadStat(ite->first,THREAD_SHUT);
//				printf("**thread %lu \n",ite->first);
				pthread_cond_broadcast(&cond);
				pthread_join(ite->first,NULL);
				map<pthread_t,int>::iterator tmp=++ite;
				pthread_mutex_lock(&map_mutex);
				threads.erase(--ite);
				ThreadNum--;
				if(ThreadNum!=threads.size())
					printf("thread num is wrong\n");
				pthread_mutex_unlock(&map_mutex);
				ite=tmp;
//				printf("**thread %lu \n",ite->first);
//				printf("**thread %d\n",threads.size());
				num++;
				if(num==n)
					break;
			}else{
				++ite;
			}
		}

	}
}
void ThreadPool::poolInit(){
	for(int i=0;i<maxThreadNum;i++)
		addThread();
}
void ThreadPool::poolDestroy(){
	printf("thread pool begin to destory\n");
	while(threads.size()!=0){
		for(map<pthread_t,int>::iterator ite=threads.begin();ite!=threads.end();){
			if(ite->second==THREAD_WAIT){
				setThreadStat(ite->first,THREAD_SHUT);
				pthread_cond_broadcast(&cond);
				pthread_join(ite->first,NULL);
				map<pthread_t,int>::iterator tmp=++ite;
				pthread_mutex_lock(&map_mutex);
				threads.erase(--ite);
				ThreadNum--;
				if(ThreadNum!=threads.size())
					printf("thread num is wrong\n");
				pthread_mutex_unlock(&map_mutex);
				ite=tmp;
			}
		}
	}
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	printf("thread pool has destoryed\n");
}
void ThreadPool::addTask(Task *task){
	taskList.push(task);
	pthread_cond_signal(&cond);
}
int ThreadPool::getTaskListSize(){
	return taskList.size();
}
int ThreadPool::getPoolSize(){
	return ThreadNum;
}

