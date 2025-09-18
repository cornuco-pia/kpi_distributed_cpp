#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <cstdint>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " <num_threads> [total_iterations]\n"; return 1;
    }
    int num_threads = std::stoi(argv[1]);
    if (num_threads <= 0) {
        std::cout << "number of threads must be higher than 0\n";
        return 1;
    }
    uint64_t num_iterations_total = (argc >=3) ? std::stoull(argv[2]) : 2000000000ULL; //2 mil
    uint64_t num_iterations_thread = num_iterations_total / static_cast<uint64_t>(num_threads);
    std::vector<double> results(num_threads, 0.0);
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i){
        threads.emplace_back(
            [i, num_iterations_thread, &results](){
                double acc = 0.0;
                for (uint64_t k=1; k<num_iterations_thread;++k){
                    acc += (k&1) ? std::sqrt(static_cast<double>(k)) : std::cbrt(static_cast<double>(k));
                }
                results[i] = acc;
            }
        );
    }
    for(auto &th: threads) th.join();
    auto t1 = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(t1-t0).count();
    double total_result = std::accumulate(results.begin(), results.end(), 0.0);

    std::cout << "threads=" << num_threads << \
    "\n per_thread_iters=" << num_iterations_thread << \
    "\n total_iters=" << (num_iterations_thread * static_cast<uint64_t>(num_threads)) << \
    "\n wall_sec=" << duration << \
    "\n result=" << total_result << "\n";
    return 0;
}