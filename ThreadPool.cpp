#include "ThreadPool.h"

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

                    task = this->taskqueue_.front();
                    this->taskqueue_.pop();
                }

                task();
            }
        });
    }
}