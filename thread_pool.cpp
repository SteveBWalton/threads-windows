/// Implmentation for the TheadPool class.
/// Class to represent a pool of threads to execute functions.
///@file thread_pool.cpp

// File Header.
#include "thread_pool.h"

// System Headers.
#include <iostream> // cout() Remove this, debugging only.
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


/// Class constructor.
/// Initialise the specified number of threads.
ThreadPool::ThreadPool
(
    int numThreads
)
{
    // Create the specified number of threads.
    threads_.reserve(numThreads);
    numJobsPending = 0;
    for (int i = 0; i < numThreads; i++)
    {
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
    }
}



/// Class destructor.
/// Close all the threads.
ThreadPool::~ThreadPool()
{
    {
        // Unblock any threads and tell them to stop.
        std::unique_lock <std::mutex> l(lock_);

        shutdown_ = true;
        condVar_.notify_all();
    }

    // Wait for all threads to stop.
    std::cout << "Joining threads" << std::endl;
    for (auto& thread : threads_)
    {
        thread.join();
    }
}



void ThreadPool::doJob
(
    std::function <void(void)> func
)
{
    // Another job before we are finished.
    numJobsPending++;

    // Place a job on the queue and unblock a thread.
    std::unique_lock <std::mutex> l(lock_);

    jobs_.emplace(std::move(func));
    condVar_.notify_one();
}



void ThreadPool::threadEntry
(
    int i
)
{
    std::function <void(void)> job;

    while (1)
    {
        {
            std::unique_lock <std::mutex> l(lock_);

            while (!shutdown_ && jobs_.empty())
            {
                condVar_.wait(l);
            }

            if (jobs_.empty())
            {
                // No jobs to do, so we are shutting down.
                std::cout << "Thread " << i << " terminates" << std::endl;
                return;
            }

            job = std::move(jobs_.front());
            jobs_.pop();
        }

        // Do the job without holding any locks.        
        job();

        // A job is finshed.
        numJobsPending--;
        if (numJobsPending == 0)
        {
            // All jobs are finished.
            mainCondition.notify_one();
        }
    }
}



void ThreadPool::waitAllFinish()
{
    std::unique_lock<std::mutex> lock(mainMutex);
    mainCondition.wait(lock);
}
