/// Definitions for the ThreadPool class.
///@file thread_pool.h

#pragma once

// System Headers.
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>



class ThreadPool
{
public:
    ThreadPool(int);
    ~ThreadPool();

private:
    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void(void)>> jobs_;
    std::vector <std::thread> threads_;

    std::atomic<int> numJobsPending;
    std::mutex mainMutex;
    std::condition_variable mainCondition;

private:
    void threadEntry(int);

public:
    void doJob(std::function <void(void)>);
    void waitAllFinish();
};
