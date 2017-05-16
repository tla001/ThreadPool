#include <iostream>
#include "threadpool/ThreadPool.h"
using namespace std;

class MyTask1:public Task{
public:
	MyTask1(string name,int pri):Task(name){
		setPriority(pri);
	}
	void run(){
		printf("hello,this is MyTask1\n");
		sleep(2);
	}
};
void *printStr(void * str){
	printf("%s\n",str);
}
int main(){
	ThreadPool *pool=new ThreadPool(10);
//	for(int i=0;i<2;i++){
//		MyTask1 *task1=new MyTask1("mytask *",i+1);
//		pool->addTask(task1);
//	}
	MyTask1 *task1=new MyTask1("mytask1",4);
	pool->addTask(task1);
	MyTask1 *task2=new MyTask1("mytask2",3);
	pool->addTask(task2);
	MyTask1 *task3=new MyTask1("mytask3",1);
	pool->addTask(task3);
	MyTask1 *task4=new MyTask1("mytask4",5);
	pool->addTask(task4);

	cout<<"pool size "<<pool->getPoolSize()<<endl;
	sleep(1);
	pool->delThread(1);

	MyTask1 *task5=new MyTask1("mytask5",6);
	pool->addTask(task5);
	CbTask *task6=new CbTask("mytask6",1);
	char *str="你好";
	task6->setCallBack(printStr,static_cast<void*>(str));
	pool->addTask(task6);
	cout<<"pool size "<<pool->getPoolSize()<<endl;
	sleep(3);
//	pool->poolDestroy();
	pool->~ThreadPool();
	return 0;
}

