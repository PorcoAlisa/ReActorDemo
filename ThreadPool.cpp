#include "ThreadPool.h"

/* 消费者 */
ThreadPool::ThreadPool(size_t threadNum):stop_(false)
{
    for (size_t i = 0; i < threadNum; i++) {
        threads_.emplace_back([this]{
            while(!stop_) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(this->mutex_);
                    // while(this->stop_ != true && this->taskqueue_.empty() != false) {
                    //     this->cond_.wait(lock);
                    // }
                    this->cond_.wait(lock, [this]{
                        return (this->stop_ == true) || (this->taskqueue_.empty() == false);
                    });
                    /* 当被唤醒后，如果stop为true，或者任务队列不为空，跳出循环往下继续执行 */
                    /* 所以当代码流程来到这里时，任务队列不为空 */
                    if (this->taskqueue_.empty() == true) {
                        cout << "error: task queue empty!" << endl;
                    }
                    if (this->stop_ == true) {
                        return;
                    }

                    task = this->taskqueue_.front();
                    this->taskqueue_.pop();
                }

                task();
            }
        });
    }
}

void ThreadPool::Stop()
{
    if (stop_) { /* 如果已经stop，说明该函数被重复执行了，不再继续往下执行 */
        return;
    }
    stop_ = true;
    cond_.notify_all();
    for (thread &th : threads_) {
        th.join();
    }
}

ThreadPool::~ThreadPool()
{
    Stop();
}

void ThreadPool::AddTask(function<void()> fn)
{
    {
        unique_lock<mutex> lock(this->mutex_);
        taskqueue_.push(fn);
    }
    /* notify_one 不用加锁，因为不再操作taskqueue_*/
    cond_.notify_one();
}

size_t ThreadPool::Size()
{
    return taskqueue_.size();
}