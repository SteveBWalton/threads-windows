// Parallel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// System Headers.
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <cmath>

// Application Headers.
#include "thread_pool.h"



// Write the number to the console and pause for a little while.
void showNumber
(
    int number
)
{
    // Write the number to console.
    std::cout << number << " ";
    std::cout.flush();

    // True to sleep afterward, false for actual calculations.
    const bool SLEEP = false;

    // Pause for a little while.
    if (SLEEP)
    {
        // Sleep for 1 second.
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }
    else
    {
        // Do actual work that hurts the processor.
        for (int i = 0; i < 10000000; i++)
        {
            double result = sqrt((double)i);
            result = pow(result, 0.78);
            result = pow(result, 1.32);
            result = pow(result, 0.57);
        }
    }
}



// Compare parallel loops and serial loops.
int main()
{
    // The number loops for the test.
    const int NUM_LOOPS = 30;

    std::cout << "Hello from Parallel.\n";

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
        threadPool.addTask(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    threadPool.waitAllFinish();
    std::cout << std::endl;

    // Lets go again with the thread pool.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.addTask(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    std::cout << std::endl;

    std::cout << "Goodbye." << std::endl;
}
