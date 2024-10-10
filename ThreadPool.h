#pragma once
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

class ThreadPool
{
public:
    vector<thread> threads_;
    queue<function<void()>> taskqueue_;
    mutex mutex_;
    condition_variable cond_;
    ThreadPool(size_t threadNum);
    atomic_bool stop_;
};
