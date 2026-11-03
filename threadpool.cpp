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
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
		threads.emplace_back(std::move(ptr));
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
	std::unique_lock<std::mutex> lock(taskQueueMutex); // 获取锁

	// 线程通信，等待任务队列有空余
	// 用户提交任务，最长阻塞不能超过一秒，否则判断提交任务失败
	if (!notFull.wait_for(lock, std::chrono::seconds(1)
		, [&]()->bool { return taskQueue.size() < (size_t)maxTaskSize; }))
	{
		std::cerr << "task queue id full, submit task fail" << std::endl;
		return;
	}
	
	// 有空余，把任务放进任务队列中
	taskQueue.emplace(task);
	taskSize++;

	// 新放了任务，所以此时一定不空，在notEmpty进行通知
	notEmpty.notify_all();
}

void ThreadPool::threadFunc() // 线程池所有线程，从任务队列消费任务
{
	//std::cout << "thread begin	id:" << std::this_thread::get_id() << std::endl;
	//std::cout << "thread end	id: " << std::this_thread::get_id() << std::endl;

	for (;;) {
		std::shared_ptr<Task> task;
		{
			// 获取锁
			std::unique_lock<std::mutex> lock(taskQueueMutex);

			// 等待notEmpty条件
			notEmpty.wait(lock, [&]()->bool {return taskQueue.size() > 0; });

			// 从任务队列中取出一个任务
			task = taskQueue.front();
			taskQueue.pop();
			taskSize--;

			// 取完后依旧有剩余任务
			if (taskQueue.size() > 0) { notEmpty.notify_all(); }

			notFull.notify_all(); // 取完后，当然可以继续提交任务，通知
		} // 释放锁

		// 当前线程执行
		if (task != nullptr) {
			task->run();
		}
	}
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
