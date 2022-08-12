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
    // Mutex for the add task function.
    std::mutex mutexAddTask_;

    // Condition to signal task in the queue.
    std::condition_variable condAddTask_;
    
    // True when the class is closing and should stop feeding tasks to the threads.
    bool isShutdown_;

    // The queue of tasks to feed to the threads.
    std::queue <std::function <void(void)>> tasks_;
    
    // The threads to actually execute the tasks on.
    std::vector <std::thread> threads_;

    // The number of tasks that are running and in the queue.
    std::atomic<int> numTasksPending_;

    // Mutex for the count tasks calculation.
    std::mutex mutexNumTasks_;

    // Condition to wait for numTasksPending_ to reach zero.
    std::condition_variable condNumTasksZero_;

private:
    // The threads execute this function.
    void threadEntry(int);

public:
    // Add a task to the queue for the thread pool to execute.
    void addTask(std::function <void(void)>);
    // Wait until the queue is empty and the running tasks complete.
    void waitAllFinish();
};
