#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <numeric>

std::vector<long long> primes;
std::mutex primesMutex;
bool is_prime(int num)
{
    if (num <= 1)
        return false;

    if (num == 2 || num == 3)
        return true;

    if (num % 2 == 0 || num % 3 == 0)
        return false;

    for (int i = 5; i * i <= num; i += 6)
    {
        if (num % i == 0 || num % (i + 2) == 0)
        {
            return false;
        }
    }

    return true;
}

void find_primes_in_range(int start, int end)
{
    for (int i = start + (start % 2 == 0 ? 1 : 0); i <= end; i += 2)
    {
        if (is_prime(i))
        {
            std::lock_guard<std::mutex> lock(primesMutex);
            primes.push_back(i);
        }
    }
}

int main()
{
    const int range_start = 1;
    const int range_end = 100000000;
    const int num_threads = 8;
    const int chunk_size = (range_end - range_start + 1) / num_threads;

    std::vector<std::thread> threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        int start = range_start + i * chunk_size;
        int end = (i == num_threads - 1) ? range_end : start + chunk_size - 1;

        threads.emplace_back(find_primes_in_range, start, end);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto execution_time = end_time - start_time;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(execution_time);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(execution_time - seconds);

    // Output results to primes.txt
    std::ofstream output_file("primes.txt");
    output_file << seconds.count() << "." << milliseconds.count() << " " << primes.size() << " " << std::accumulate(primes.begin(), primes.end(), 0LL) << "\n";

    std::sort(primes.begin(), primes.end());
    for (int i = 0; i < std::min(10, static_cast<int>(primes.size())); i++)
    {
        output_file << primes[i + (primes.size() - 10)] << " ";
    }

    output_file.close();

    return 0;
}