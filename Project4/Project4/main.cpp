#include <barrier>
#include <chrono>
#include <functional>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "actions.h"

constexpr int NT = 5; 
constexpr int STEPS = 12;  

struct Action
{
    char set;
    int idx;
};

const Action schedule[STEPS][NT] = {
 {{'a', 1}, {'a', 2}, {'a', 3}, {'a', 4}, {'a', 5}},
 {{'a', 6}, {'a', 7}, {0, 0},   {0, 0},   {0, 0}},
 {{'c', 1}, {'c', 2}, {'c', 3}, {'c', 4}, {'c', 5}},
 {{'b', 1}, {'b', 2}, {'b', 3}, {'b', 4}, {'b', 5}},
 {{'b', 6}, {'d', 1}, {'d', 2}, {'d', 3}, {'d', 4}},
 {{'d', 5}, {'d', 6}, {'g', 1}, {'g', 2}, {'g', 3}},
 {{'e', 1}, {'e', 2}, {'e', 3}, {'e', 4}, {'g', 4}},
 {{'g', 5}, {'i', 1}, {'i', 2}, {'i', 3}, {'i', 4}},
 {{'i', 5}, {'i', 6}, {'i', 7}, {'i', 8}, {'j', 1}},
 {{'h', 1}, {'h', 2}, {'h', 3}, {'h', 4}, {'h', 5}},
 {{'j', 2}, {'j', 3}, {'j', 4}, {'j', 5}, {'k', 1}},
 {{'k', 2}, {'k', 3}, {'k', 4}, {0, 0},   {0, 0}}
};

void worker(int id, std::barrier<>& sync)
{
    for (int step = 0; step < STEPS; ++step)
    {
        Action a = schedule[step][id];
        if (a.set != 0)
        {
            f(a.set, a.idx);
        }
        sync.arrive_and_wait();
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::osyncstream(std::cout) << "Обчислення розпочато.\n";
    auto start = std::chrono::steady_clock::now();

    {
        std::barrier sync(NT);
        std::vector<std::jthread> threads;
        threads.reserve(NT);

        for (int id = 0; id < NT; ++id)
        {
            threads.emplace_back(worker, id, std::ref(sync));
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::osyncstream(std::cout) << "Обчислення завершено. Тривалість обчислення "
        << elapsed.count() << " секунд.\n";
    return 0;
}