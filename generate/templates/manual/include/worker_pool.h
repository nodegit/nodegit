#ifndef WORK_POOL_H
#define WORK_POOL_H

#include <type_traits>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>

/**
 * \class WorkItem
 * Abstract class for work items in the WorkerPool.
 */
class WorkItem
{
public:
  WorkItem() = default;
  virtual ~WorkItem() = default;
  WorkItem(const WorkItem &other) = default;
  WorkItem(WorkItem &&other) = default;
  WorkItem& operator=(const WorkItem &other) = default;
  WorkItem& operator=(WorkItem &&other) = default;
};

/**
 * \class Worker
 * Interface for Workers in the WorkerPool.
 */
class IWorker
{
public:
  IWorker() = default;
  virtual ~IWorker() = default;
  IWorker(const IWorker &other) = delete;
  IWorker(IWorker &&other) = delete;
  IWorker& operator=(const IWorker &other) = delete;
  IWorker& operator=(IWorker &&other) = delete;

  virtual bool Initialize() = 0;
  virtual bool Execute(std::unique_ptr<WorkItem> &&work) = 0;
};

/**
 * \class WorkerPool
 * To leverage this class, a Worker must inherit from IWorker.
 * WorkItem is an abstract class from which to inherit too.
 */
template<class Worker, class WorkItem>
class WorkerPool {
public:
  WorkerPool();
  ~WorkerPool() = default;
  WorkerPool(const WorkerPool &other) = delete;
  WorkerPool(WorkerPool &&other) = delete;
  WorkerPool& operator=(const WorkerPool &other) = delete;
  WorkerPool& operator=(WorkerPool &&other) = delete;

  bool Init(std::vector< std::shared_ptr<Worker> > workers);
  bool InsertWork(std::unique_ptr<WorkItem> &&work);
  void Shutdown();

private:
  void DoWork(std::shared_ptr<Worker> worker);
  
  std::mutex m_workQueueMutex {};
  std::condition_variable m_workQueueCondition {};
  std::queue< std::unique_ptr<WorkItem> > m_workQueue {};
  std::vector<std::unique_ptr<std::thread>> m_threads {};
  bool m_stop {true}; // initially the workpool has no worker threads
};


template<class Worker, class WorkItem>
WorkerPool<Worker,WorkItem>::WorkerPool() {
  static_assert(std::is_base_of<IWorker, Worker>::value, "Worker must inherit from IWorker");
}

// returns true it it wasn't initialized previously; false otherwise
template<class Worker, class WorkItem>
bool WorkerPool<Worker,WorkItem>::Init(std::vector< std::shared_ptr<Worker> > workers)
{
  {
    std::lock_guard<std::mutex> lock(m_workQueueMutex);
    if (!m_stop)
      return false;
    m_stop = false;
  }
  
  std::for_each (workers.begin(), workers.end(), [this](std::shared_ptr<Worker> worker) {
    m_threads.emplace_back(std::make_unique<std::thread>(std::bind(&WorkerPool::DoWork, this, worker)));
  });
  
  return true;
}

// returns true if successfully inserted work; false otherwise
template<class Worker, class WorkItem>
bool WorkerPool<Worker,WorkItem>::InsertWork(std::unique_ptr<WorkItem> &&work)
{
  {
    std::lock_guard<std::mutex> lock(m_workQueueMutex);
    if (m_stop)
      return false;
    m_workQueue.emplace(std::move(work));
  }
  m_workQueueCondition.notify_one();
  
  return true;
}

template<class Worker, class WorkItem>
void WorkerPool<Worker,WorkItem>::Shutdown()
{
  {
    std::lock_guard<std::mutex> lock(m_workQueueMutex);
    if (m_stop)
      return;
    m_stop = true;
  }
  m_workQueueCondition.notify_all();
  
  std::for_each (m_threads.begin(), m_threads.end(), [](std::unique_ptr<std::thread> &wt) {
    if (wt->joinable()) {
      wt->join();
    }
  });
}

template<class Worker, class WorkItem>
void WorkerPool<Worker,WorkItem>::DoWork(std::shared_ptr<Worker> worker)
{
  if (!worker->Initialize()) {
    return; // signal main that we've exited early
  }
  
  while (true) {
    std::unique_ptr<WorkItem> work {};
    {
      std::unique_lock<std::mutex> lock(m_workQueueMutex);
      m_workQueueCondition.wait(lock, [this] {
        return this->m_stop || !this->m_workQueue.empty();
      });
      
      if (m_stop && m_workQueue.empty())
        return;
      
      work = std::move(m_workQueue.front());
      m_workQueue.pop();
    }
    
    if (!worker->Execute(std::move(work))) {
      return; // signal main that we've exited early
    }
  }
}

#endif  // WORK_POOL_H

