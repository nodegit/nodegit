#include "../include/semaphore.h"

Semaphore::Semaphore()
  : count(0)
{}

void Semaphore::post() {
  std::lock_guard<std::mutex> lock(mutex);
  ++count;
  condition.notify_one();
}

void Semaphore::wait() {
  std::unique_lock<std::mutex> lock(mutex);
  while (!count) condition.wait(lock);
  --count;
}
