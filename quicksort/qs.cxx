
// use g++ -O3

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>
#include <thread>
#include <stdlib.h>

template<class It>
void qs(It first, It last) {
    if(first >= last) return;
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
    
    qs(first, i);
    qs(std::next(i, 1),  last);
}
#define N 1e8

int main() {
  srand(time(NULL));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, N);
  std::vector<unsigned int> vec;
  // vec.reserve(N);

  for (unsigned int i = 0; i < N; ++i) {
    vec.push_back(dis(gen));
    // vec.push_back((unsigned int)rand() * N);
  }

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  qs(vec.begin(), vec.end());
  // qs(fl.begin(), fl.end());
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << "ms" << std::endl;

  // for (auto &x : fl) {
    // std::cout << x << ' ';
  // }
  
}
