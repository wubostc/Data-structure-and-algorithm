
// use g++ -O3

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>
#include <thread>
#include <stdlib.h>
#include <cstdint>
#include <future>
#include <atomic>

std::atomic<std::uint64_t> threads_count (std::thread::hardware_concurrency());
std::atomic_ulong threads(0);

template<class It>
void qs(It first, It last, std::promise<void> acc_promise) {
    if(first >= last) {
      acc_promise.set_value();
      return;
    }
    auto piovt = *first;
    auto i = first;
    auto j = std::next(last, -1);

    while (i < j) {
        while (*j >= piovt && i < j) {
            std::advance(j, -1);
            // --j;
        }
        *i = *j;
        while (*i <= piovt && i < j) {
            std::advance(i , 1);
            // ++i;
        }
        *j = *i;
    }

    *i = piovt;

    std::promise<void> promise1{};
    std::promise<void> promise2{};

    while (threads == threads_count) {
      std::this_thread::yield();
    }
    threads += 2;
    std::thread(qs<It>, first, std::next(i, -1), 
        std::move(promise1)).join();
    std::thread(qs<It>, std::next(i, 1), last,
        std::move(promise2)).join();

    promise1.get_future().wait();
    promise2.get_future().wait();
    threads -= 2; 
    acc_promise.set_value();

    // qs(first, std::next(i, -1));
    // qs(std::next(i, 1),  last);
}
#define N 1e1

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, N);
  std::vector<unsigned int> vec;
  vec.reserve(N);

  for (unsigned int i = 0; i < N; ++i) {
    vec.push_back(dis(gen));
  }


  std::promise<void> acc_promise;
  std::future<void> acc_future = acc_promise.get_future();

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  std::thread mthread(
      qs<decltype(vec)::iterator>,
      vec.begin(), vec.end(), std::move(acc_promise));
  acc_future.wait();
  // qs(vec.begin(), vec.end());

  // qs(fl.begin(), fl.end());
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << "ms" << std::endl;

  mthread.join();

  // for (auto &x : fl) {
    // std::cout << x << ' ';
  // }

}
