#include "../include/reference_counter.h"

void ReferenceCounter::incrementCountForPointer(void *ptr) {
  LockMaster(true, &referenceCountByPointer);
  if (referenceCountByPointer.find(ptr) == referenceCountByPointer.end()) {
    referenceCountByPointer[ptr] = 1;
  } else {
    referenceCountByPointer[ptr] = referenceCountByPointer[ptr] + 1;
  }
}

unsigned long ReferenceCounter::decrementCountForPointer(void *ptr) {
  LockMaster(true, &referenceCountByPointer);
  unsigned long referenceCount = referenceCountByPointer[ptr];
  if (referenceCount == 1) {
    referenceCountByPointer.erase(ptr);
    return 0;
  } else {
    referenceCountByPointer[ptr] = referenceCount - 1;
    return referenceCountByPointer[ptr];
  }
}

std::unordered_map<void *, unsigned long> ReferenceCounter::referenceCountByPointer;
