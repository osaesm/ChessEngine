#include "thread_pool.hpp"

// ------------------------------------------------------------------
// ThreadPool implementation
// ------------------------------------------------------------------
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back([this] {
      for (;;) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->queueMutex);
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });
          if (this->stop && this->tasks.empty())
            return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread &worker : workers) {
    if (worker.joinable())
      worker.join();
  }
}

void ThreadPool::enqueue(std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (stop)
      throw std::runtime_error("enqueue on stopped ThreadPool");
    tasks.emplace(std::move(task));
  }
  condition.notify_one();
}
