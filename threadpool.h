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
#include<chrono>
#include<thread>

/*
example:
ThreadPool pool;
pool.start(4);

class MyTask : public Task{
public:
	void run() {}
};

pool.submitTask(std::make_shared<MyTask>());
*/

// Any类型：可以接收任意类型的数据
class Any {
public:
	Any() = default;
	~Any() = default;

	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;

	template <typename T>
	Any(T data) : base(std::make_unique<Derive<T>>(data)) {}

	template <typename T>
	T cast() {
		Derive<T>* pd = dynamic_cast<Derive<T>*>(base.get());
		if (pd == nullptr) {
			throw "类型不匹配";
		}
		return pd->Data;
	} 

	class Base { // 基类
	public:
		virtual ~Base() = default;
	};

	template <typename T>
	class Derive : public Base { // 派生类
	public:
		Derive(T data) : Data(data) {}
		T Data;
	};
private:
	std::unique_ptr<Base> base; // 基类指针
};

class Semphore {
public:
	Semphore(int limit = 0) : resLimit(limit) {}
	~Semphore() = default;

	void wait() { // 获取信号量资源
		std::unique_lock<std::mutex> lock(mtx);

		cv.wait(lock, [&]() { return resLimit > 0; }); // 等待条件变量，直到资源可用
		resLimit--;
	}

	void post() { // 增加信号量资源
		std::unique_lock<std::mutex> lock(mtx);
		resLimit++;
		cv.notify_all();
	}
private:
	std::mutex mtx; // 互斥锁
	std::condition_variable cv; // 条件变量
	int resLimit;

};

enum class PoolMode {
	MODE_FIXED, // 固定线程数模式
	MODE_CACHED, // 缓存线程数模式(可动态增长)
};

class Task { // 用户自定义任务类型，从Task类继承并实现重写的run()方法
public:
	virtual Any run() = 0; // 纯虚函数，子类必须实现
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
	std::vector<std::unique_ptr<Thread>> threads; // 线程池中的线程列表
	int initThreadSize; // 初始线程数

	std::queue<std::shared_ptr<Task>> taskQueue; // 任务队列
	std::atomic_int taskSize; // 当前任务队列中的任务数
	int maxTaskSize; // 任务队列的最大容量

	std::mutex taskQueueMutex; // 任务队列的互斥锁
	std::condition_variable notEmpty; // 任务队列非空条件变量
	std::condition_variable notFull; // 任务队列非满条件变量

	PoolMode poolmode; // 当前线程池模式
};


#endif // THREADPOOL_H