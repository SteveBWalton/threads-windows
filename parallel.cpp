// Parallel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// System Headers.
#include <iostream>
#include <chrono>
#include <thread>

// Application Headers.
#include "thread_pool.h"



void showNumber
(
    int number
)
{
    std::cout << number << " ";
    std::cout.flush();

    
    // Not available in Windows.    
    // usleep(1000000);

    // Sleep for 1 second.
    std::this_thread::sleep_for(std::chrono::microseconds(1000000));
}



int main()
{
    const int NUM_LOOPS = 30;

    std::cout << "Hello World!\n";

    // Call the function n times in serial.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        showNumber(i);
    }
    std::cout << std::endl;

    // Call the function n times on a new thread each time.
    std::thread* myThreads[NUM_LOOPS];
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        myThreads[i] = new std::thread(showNumber, i);
    }

    // Wait for each thread to finish.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        // Wait for this thread to finish.
        myThreads[i]->join();
    }
    std::cout << std::endl;

    // Call the function n times with a thread pool objecct.
    ThreadPool threadPool(4);
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.doJob(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    std::cout << std::endl;

    // Lets go again with the thread pool.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.doJob(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    std::cout << std::endl;

    std::cout << "Goodbye." << std::endl;
}
