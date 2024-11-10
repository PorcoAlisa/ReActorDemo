#pragma once
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>

using namespace std;

class ThreadPool
{
private:
    vector<thread> threads_;
    queue<function<void()>> taskqueue_;
    mutex mutex_;
    condition_variable cond_;
    atomic_bool stop_;
    string type_;
public:
    ThreadPool(size_t threadNum, const string& type);
    ~ThreadPool();
    void Stop();
    void AddTask(function<void()> fn);
    size_t Size();
};
