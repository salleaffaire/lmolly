// Here is a mouthful
// Synchronized Symmetrical Deterministic Buffer State Handler
// ---------------------------------------------------------------------

#ifndef LMOLLY_SSDBSHANDLER_HPP___
#define LMOLLY_SSDBSHANDLER_HPP___

#include "semaphore.hpp"

#include <queue>
#include <memory>
#include <list>

namespace lmolly {

template <class T>
class ssdbsh {
public:
   explicit ssdbsh(const std::list<T *> &l) : 
      mInitSize(l.size()), 
      mEmptyQueueSemaphore(l.size()),
      mFullQueueSemaphore(0) {

      // Push all buffer in the empty queue
      for (auto &e: l) {
         mEmptyQueue.push(std::shared_ptr<T>(e));   
      }
   }
   virtual ~ssdbsh() {}

   std::shared_ptr<T> get_empty() {
      std::shared_ptr<T> rval;
      // Wait for an empty element to be available
      mEmptyQueueSemaphore.Wait();
      {
         std::lock_guard<std::mutex> lock(mEmptyQueueMutex);
         rval = mEmptyQueue.front();
         mEmptyQueue.pop();
      }
      return rval;
   }

   std::shared_ptr<T> get_full() {
      std::shared_ptr<T> rval;
      // Wait for a full element to be available
      mFullQueueSemaphore.Wait();
      {
         std::lock_guard<std::mutex> lock(mFullQueueMutex);
         rval = mFullQueue.front();
         mFullQueue.pop();
      }
      return rval;
   }

   void put_empty(std::shared_ptr<T>) {

   }

   void put_full(std::shared_ptr<T>) {
      
   }

private:
   // Initial size
   unsigned int mInitSize;

   // Empty queue 
   std::queue<std::shared_ptr<T>> mEmptyQueue;
   lmolly::semaphore              mEmptyQueueSemaphore;
   std::mutex                     mEmptyQueueMutex;

   // Full queue
   std::queue<std::shared_ptr<T>> mFullQueue;
   lmolly::semaphore              mFullQueueSemaphore;
   std::mutex                     mFullQueueMutex;

};

} // end of lmolly namespace 

#endif
