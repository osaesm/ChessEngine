// thread_pool.hpp
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <queue>
#include <thread>

// ------------------------------------------------------------------
// Simple thread pool for parallel perft / search tasks
// ------------------------------------------------------------------
class ThreadPool {
public:
  explicit ThreadPool(size_t numThreads);
  ~ThreadPool();

  // Enqueue a callable that takes no arguments and returns void.
  void enqueue(std::function<void()> task);

  // Number of worker threads.
  size_t size() const { return workers.size(); }

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
};

#endif
