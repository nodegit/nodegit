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

  void Init(std::vector< std::shared_ptr<Worker> > workers);
  void InsertWork(std::unique_ptr<WorkItem> &&work);
  void Shutdown();
  bool Status() { return m_status.workersInitialize && m_status.workersExecute && !m_status.shutdownEarly; }
  bool StatusWorkersInitialize() { return m_status.workersInitialize; }
  bool StatusWorkersExecute() { return m_status.workersExecute; }
  bool StatusShutdownEarly() { return !m_status.shutdownEarly; }

private:
  void DoWork(std::shared_ptr<Worker> worker);
  
  std::mutex m_workQueueMutex {};
  std::condition_variable m_workQueueCondition {};
  std::queue< std::unique_ptr<WorkItem> > m_workQueue {};
  std::vector<std::unique_ptr<std::thread>> m_threads {};
  bool m_stop {true};               // initially the workpool has no worker threads
  struct {
    bool workersInitialize {true};
    bool workersExecute {true};
    bool shutdownEarly {false};   // for instance if the workerPool was ShutDown, and InsertWork called after that
  } m_status;
  std::mutex m_statusMutex {};
};


template<class Worker, class WorkItem>
WorkerPool<Worker,WorkItem>::WorkerPool() {
  static_assert(std::is_base_of<IWorker, Worker>::value, "Worker must inherit from IWorker");
}

// launches the worker threads, if they hadn't been launched already
template<class Worker, class WorkItem>
void WorkerPool<Worker,WorkItem>::Init(std::vector< std::shared_ptr<Worker> > workers)
{
  {
    std::lock_guard<std::mutex> lock(m_workQueueMutex);
    if (!m_stop)
      return;
    m_stop = false;
  }
  
  std::for_each (workers.begin(), workers.end(), [this](std::shared_ptr<Worker> worker) {
    m_threads.emplace_back(std::make_unique<std::thread>(std::bind(&WorkerPool::DoWork, this, worker)));
  });
}

// queues work, or updates shutdownEarly status
template<class Worker, class WorkItem>
void WorkerPool<Worker,WorkItem>::InsertWork(std::unique_ptr<WorkItem> &&work)
{
  {
    std::lock_guard<std::mutex> lock(m_workQueueMutex);
    if (m_stop) {
      // update shutdownEarly status and return
      std::lock_guard<std::mutex> lock(m_statusMutex);
      m_status.shutdownEarly = true;
      return;
    }
    m_workQueue.emplace(std::move(work));
  }
  m_workQueueCondition.notify_one();
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
  if (!worker->Initialize())
  { // update initialized status and stop worker
    std::lock_guard<std::mutex> lock(m_statusMutex);
    m_status.workersInitialize = false;
    return;
  }

  while (true) {
    std::unique_ptr<WorkItem> work {};
    {
      std::unique_lock<std::mutex> lock(m_workQueueMutex);
      m_workQueueCondition.wait(lock, [this] {
        return this->m_stop || !this->m_workQueue.empty();
      });

      // stop all workers if any of them failed on Initialize() or Execute()
      // or the workerPool shutdown early
      if (!Status()) {
        return;
      }

      if (m_stop && m_workQueue.empty()) {
        return;
      }
      
      work = std::move(m_workQueue.front());
      m_workQueue.pop();
    }
    
    if (!worker->Execute(std::move(work)))
    { // update execute status
      std::lock_guard<std::mutex> lock(m_statusMutex);
      m_status.workersExecute = false;
      return;
    }
  }
}

#endif  // WORK_POOL_H

