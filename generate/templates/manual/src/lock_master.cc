#include <nan.h>
#include <git2.h>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <mutex>
#include <iostream>
#include <memory>

#include "../include/lock_master.h"
namespace nodegit {
  // information about a lockable object
  // - the mutex used to lock it and the number of outstanding locks
  struct ObjectInfo {
    std::shared_ptr<std::mutex> mutex;
    unsigned useCount;

    ObjectInfo(unsigned useCount)
      : mutex(new std::mutex), useCount(useCount)
    {}
  };

  // LockMaster implementation details
  // implemented in a separate class to keep LockMaster opaque
  class LockMasterImpl {
    // STATIC variables / methods

    // A map from objects that are locked (or were locked), to information on their mutex
    static std::map<const void *, ObjectInfo> mutexes;
    // A mutex used for the mutexes map
    static std::mutex mapMutex;

    // A thread local storage slot for the current thread-specific LockMasterImpl instance
    thread_local static LockMasterImpl* currentLockMaster;

    // Cleans up any mutexes that are not currently used
    static NAN_GC_CALLBACK(CleanupMutexes);

  public:
    static void InitializeContext();

    // INSTANCE variables / methods

  private:
    // The set of objects this LockMaster is responsible for locking
    std::set<const void *> objectsToLock;

    // Mutexes locked by this LockMaster on construction and unlocked on destruction
    std::vector<std::shared_ptr<std::mutex>> GetMutexes(int useCountDelta);
    void Register();
    void Unregister();

  public:
    static LockMasterImpl *CurrentLockMasterImpl() {
      return (LockMasterImpl *)currentLockMaster;
    }

    LockMasterImpl() {
      Register();
    }

    ~LockMasterImpl() {
      Unregister();
      Unlock(true);
    }

    void ObjectToLock(const void *objectToLock) {
      objectsToLock.insert(objectToLock);
    }

    void Lock(bool acquireMutexes);
    void Unlock(bool releaseMutexes);
  };

  std::map<const void *, ObjectInfo> LockMasterImpl::mutexes;
  std::mutex LockMasterImpl::mapMutex;
  thread_local LockMasterImpl* LockMasterImpl::currentLockMaster = nullptr;

  LockMaster::LockMaster(LockMaster &&other) {
    impl = other.impl;
    other.impl = nullptr;
  }

  LockMaster &LockMaster::operator=(LockMaster &&other) {
    if (&other == this) {
      return *this;
    }

    impl = other.impl;
    other.impl = nullptr;

    return *this;
  }

  void LockMasterImpl::InitializeContext() {
    Nan::AddGCEpilogueCallback(CleanupMutexes);
  }

  NAN_GC_CALLBACK(LockMasterImpl::CleanupMutexes) {
    std::lock_guard<std::mutex> lock(mapMutex);

    for (auto it = mutexes.begin(); it != mutexes.end(); )
    {
      // if the mutex is not used by any LockMasters,
      // we can destroy it
      unsigned useCount = it->second.useCount;
      if (!useCount) {
        auto to_erase = it;
        it++;
        mutexes.erase(to_erase);
      } else {
        it++;
      }
    }
  }

  void LockMaster::InitializeContext() {
    LockMasterImpl::InitializeContext();
  }

  std::vector<std::shared_ptr<std::mutex>> LockMasterImpl::GetMutexes(int useCountDelta) {
    std::vector<std::shared_ptr<std::mutex>> objectMutexes;
    std::lock_guard<std::mutex> lock(mapMutex);

    for (auto object : objectsToLock) {
      if (object) {
        // ensure we have an initialized mutex for each object
        auto mutexIt = mutexes.find(object);
        if (mutexIt == mutexes.end()) {
          mutexIt = mutexes.insert(
            std::make_pair(
              object,
              ObjectInfo(0U)
            )
          ).first;
        }

        objectMutexes.push_back(mutexIt->second.mutex);
        mutexIt->second.useCount += useCountDelta;
      }
    }

    return objectMutexes;
  }

  void LockMasterImpl::Register() {
    currentLockMaster = this;
  }

  void LockMasterImpl::Unregister() {
    currentLockMaster = nullptr;
  }

  void LockMasterImpl::Lock(bool acquireMutexes) {
    std::vector<std::shared_ptr<std::mutex>> objectMutexes = GetMutexes(acquireMutexes * 1);

    auto alreadyLocked = objectMutexes.end();
    std::vector<std::shared_ptr<std::mutex>>::iterator it;

    // we will attempt to lock all the mutexes at the same time to avoid deadlocks
    // note in most cases we are locking 0 or 1 mutexes. more than 1 implies
    // passing objects with different repos/owners in the same call.
    do {
      // go through all the mutexes and try to lock them
      for (it = objectMutexes.begin(); it != objectMutexes.end(); it++) {
        // if we already locked this mutex in a previous pass via std::mutex::lock,
        // we don't need to lock it again
        if (it == alreadyLocked) {
          continue;
        }

        // first, try to lock (non-blocking)
        bool success = (*it)->try_lock();
        if (!success) {
          // we have failed to lock a mutex... unlock everything we have locked
          std::for_each(objectMutexes.begin(), it, [](std::shared_ptr<std::mutex> mutex) {
            mutex->unlock();
          });

          if (alreadyLocked > it && alreadyLocked != objectMutexes.end()) {
            (*alreadyLocked)->unlock();
          }

          // now do a blocking lock on what we couldn't lock
          (*it)->lock();
          // mark that we have already locked this one
          // if there are more mutexes than this one, we will go back to locking everything
          alreadyLocked = it;
          break;
        }
      }
    } while (it != objectMutexes.end());
  }

  void LockMasterImpl::Unlock(bool releaseMutexes) {
    // Get the mutexes but don't decrement their use count until after we've
    // unlocked them all.
    std::vector<std::shared_ptr<std::mutex>> objectMutexes = GetMutexes(0);

    std::for_each(objectMutexes.begin(), objectMutexes.end(), [](std::shared_ptr<std::mutex> mutex) {
      mutex->unlock();
    });

    GetMutexes(releaseMutexes * -1);
  }

  // LockMaster

  void LockMaster::ConstructorImpl() {
    impl = new LockMasterImpl();
  }

  void LockMaster::DestructorImpl() {
    delete impl;
  }

  void LockMaster::ObjectToLock(const void *objectToLock) {
    impl->ObjectToLock(objectToLock);
  }

  void LockMaster::ObjectsToLockAdded() {
    impl->Lock(true);
  }

  // LockMaster::TemporaryUnlock

  void LockMaster::TemporaryUnlock::ConstructorImpl() {
    impl = LockMasterImpl::CurrentLockMasterImpl();
    if (impl) {
      impl->Unlock(false);
    }
  }

  void LockMaster::TemporaryUnlock::DestructorImpl() {
    impl->Lock(false);
  }

}
