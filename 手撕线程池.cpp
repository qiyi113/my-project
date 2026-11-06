#include"threadpool.h"
#include<iostream>

class MyTask : public Task {
public:
	MyTask(int b, int e)
		: begin(b)
		, end(e)
	{}
	Any run() { // 1. 如何返回任意类型？
		std::cout << "id:" << std::this_thread::get_id() << "begin!" << std::endl;
		/*std::this_thread::sleep_for(std::chrono::seconds(2));*/

		int sum = 0;
		for (int i = begin; i <= end; i++) sum += i;

		std::cout << "id:" << std::this_thread::get_id() << "end!" << std::endl;
		return sum;
	}
private:
	int begin, end;
};



int main() {
	ThreadPool pool;
	pool.start(4);


	//2. Result机制怎么设计？
	//Result res = pool.submitTask(std::make_shared<MyTask>());
	//res.get();

	auto task = std::make_shared<MyTask>();
	pool.submitTask(task);

	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());

	getchar();
}