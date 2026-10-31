#include"threadpool.h"

#include<iostream>

const int MAX_TASK_QUEUE = 1024;

ThreadPool::ThreadPool()
	: initThreadSize(0)
	, taskSize(0)
	, maxTaskSize(MAX_TASK_QUEUE)
	, poolmode(PoolMode::MODE_FIXED)
{
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::start(int init) // 开启线程池
{
	initThreadSize = init; // 记录初始线程数

	for (int i = 0; i < initThreadSize; i++) { // 创建线程对象
		threads.emplace_back(new Thread(std::bind(&ThreadPool::threadFunc, this)));
		// threads.emplace_back(new Thread([this](){ threadFunc(); }));
	}

	for (int i = 0; i < initThreadSize; i++) { // 启动所有线程
		threads[i]->start();
	}
}

void ThreadPool::setMode(PoolMode mode)
{
	poolmode = mode;
}

void ThreadPool::settaskQueueMaxSize(int maxSize)
{
	maxTaskSize = maxSize;
}

void ThreadPool::submitTask(std::shared_ptr<Task> task) // 用户调用接口，传入任务对象，生产任务
{
}

void ThreadPool::threadFunc() // 线程池所有线程，从任务队列消费任务
{
	std::cout << "thread begin	id:" << std::this_thread::get_id() << std::endl;
	std::cout << "thread end	id: " << std::this_thread::get_id() << std::endl;
}

///////////////////////////////////////////// 线程方法

Thread::Thread(ThreadFunc func)
	: Func(func)
{
}

Thread::~Thread()
{
}

void Thread::start()
{
	// 创建一个线程，执行Func函数
	std::thread t(Func);
	t.detach(); // 分离线程，使其在后台运行
}
