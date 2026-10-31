// #pragma once // 防止头文件被重复包含

#ifndef THREADPOOL_H
#define THREADPOOL_H // 通用的防止重复包含的宏定义

#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
#include<functional>

enum class PoolMode {
	MODE_FIXED, // 固定线程数模式
	MODE_CACHED, // 缓存线程数模式(可动态增长)
};

class Task { // 用户自定义任务类型，从Task类继承并实现重写的run()方法
public:
	virtual void run() = 0; // 纯虚函数，子类必须实现
};

class Thread {
public:
	using ThreadFunc = std::function<void()>; // 线程执行函数类型

	Thread(ThreadFunc func);
	~Thread();

	void start(); // 启动线程
private:
	ThreadFunc Func; // 线程执行函数
};

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void start(int init = 4); // 启动线程池
	void setMode(PoolMode mode); // 设置线程池模式
	void settaskQueueMaxSize(int maxSize); // 设置任务队列的最大容量
	void submitTask(std::shared_ptr<Task> task); // 添加任务到任务队列

	ThreadPool(const ThreadPool&) = delete; // 禁止拷贝构造函数
	ThreadPool& operator=(const ThreadPool&) = delete; // 禁止拷贝赋值运算符

private:
	void threadFunc(); // 线程执行函数

private:
	std::vector<Thread*> threads; // 线程池中的线程列表
	int initThreadSize; // 初始线程数

	std::queue<std::shared_ptr<Task>> taskQueue; // 任务队列
	std::atomic_int taskSize; // 当前任务队列中的任务数
	int maxTaskSize; // 任务队列的最大容量

	std::mutex taskQueueMutex; // 任务队列的互斥锁
	std::condition_variable taskQueueNotEmpty; // 任务队列非空条件变量
	std::condition_variable taskQueueNotFull; // 任务队列非满条件变量

	PoolMode poolmode; // 当前线程池模式
};


#endif // THREADPOOL_H