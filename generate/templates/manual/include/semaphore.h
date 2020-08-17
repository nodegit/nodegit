#ifndef NODEGIT_SEMAPHORE
#define NODEGIT_SEMAPHORE

#include <condition_variable>
#include <mutex>

class Semaphore {
public:
  Semaphore();

  void post();
  void wait();

private:
  std::mutex mutex;
  std::condition_variable condition;
  unsigned long count;
};

#endif
