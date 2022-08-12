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


// Class constructor.
// Initialise the specified number of threads.
ThreadPool::ThreadPool
(
    int numThreads
)
{
    // Create the specified number of threads.
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; i++)
    {
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
    }
    numTasksPending_ = 0;
}



// Class destructor.
// Close all the threads.
ThreadPool::~ThreadPool()
{
    {
        // Wait for all add task functions to finish.
        std::unique_lock <std::mutex> lock(mutexAddTask_);

        // Tell the threads to stop.
        isShutdown_ = true;
        condAddTask_.notify_all();        
    }

    // Wait for all threads to stop.
    std::cout << "Joining threads" << std::endl;
    for (auto& thread : threads_)
    {
        thread.join();
    }
}



// Add a task to the queue.
void ThreadPool::addTask
(
    std::function <void(void)> newTask
)
{
    // Wait for all add task functions to finish.
    std::unique_lock <std::mutex> lock(mutexAddTask_);

    // Another task before we are finished.
    numTasksPending_++;

    // Place the task on the quene.
    tasks_.emplace(std::move(newTask));

    // Unblock one thread.
    condAddTask_.notify_one();
}



// The function executed by the threads.
// Fetch a task from the queue and execute it.
// Otherwise wait for the queue to contains a task.
void ThreadPool::threadEntry
(
    int i
)
{
    // The next task taken from the task queue.
    std::function <void(void)> nextTask;

    // Loop until the isShutdown signal is set.
    while (1)
    {
        // Lock ends after this block.
        {
            // Wait for all the add task functions to finish.
            std::unique_lock <std::mutex> lock(mutexAddTask_);

            // Check if there are any tasks to do.
            while (!isShutdown_ && tasks_.empty())
            {
                // Releases the add task lock and waits for its return.
                condAddTask_.wait(lock);
            }

            if (tasks_.empty())
            {
                // Sleep for 0.5 second, so we can we that the lock releases each thread in turn.  This is debugging only, not needed.
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                // No tasks to do, so we are shutting down.
                std::cout << "Thread " << i << " terminates" << std::endl;
                
                // Close the thread.
                return;
            }

            // Get the next task.
            nextTask = std::move(tasks_.front());
            tasks_.pop();
        }
        // Release the add task lock.

        // Do the task without holding any locks.
        nextTask();

        // The task is finshed.
        numTasksPending_--;
        if (numTasksPending_ == 0)
        {
            // All jobs are finished.
            condNumTasksZero_.notify_all();
        }
    }
}



void ThreadPool::waitAllFinish()
{
    // Wait for the num task functions to finish. 
    std::unique_lock<std::mutex> lock(mutexNumTasks_);
    
    // Check for any pending tasks.
    if (numTasksPending_ > 0)
    {
        // Wait for nofication that numJobsPending_ has reached zero.
        condNumTasksZero_.wait(lock);
    }
}
