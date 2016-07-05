#ifndef AA_SQ_HPP___
#define AA_SQ_HPP___

#include <iostream>
#include <sstream>

#include <string>
#include <queue>
#include <mutex>

#include "../semaphore.hpp"
#include "aa_tools.hpp"

namespace lmolly {

// Synchronization Queue
// ---------------------------------------------------------------------
template <class T>
class aa_sync_queue {
public:
   aa_sync_queue() : 
   mName("No Name"), mMessageSemaphore(0) {
   }

   aa_sync_queue(std::string name) : 
   mName(name), mMessageSemaphore(0) {
   }

   ~aa_sync_queue() {
      unsigned int thrownAwayMessages = 0;
      // Some cleanup needs to be done here
      // Wait for the queue to be empty?
      bool valid;
      pop(std::chrono::milliseconds(1000), valid); 
      while (valid) {
         ++thrownAwayMessages;
         pop(std::chrono::milliseconds(1000), valid);
      }
      std::cout << "Discarting " << thrownAwayMessages << " messages." << std::endl;
   }

   // Blocking pop
   T pop() {
      T rval;
      // Wait for a message to be present 
      mMessageSemaphore.Wait();
      {
         std::lock_guard<std::mutex> lock(mMessageQueueLock);
         rval = mMessageQueue.front();
         mMessageQueue.pop();
      }
      return rval;
   }

   // Waiting pop
   template <typename R, typename P>
   T pop(const std::chrono::duration<R,P>& time, bool &valid) {      
      T rval;

      // Wait for a message to be present 
      valid = mMessageSemaphore.Wait(time);

      // Only if we actually got a message, not just timed out
      if (valid) {
         std::lock_guard<std::mutex> lock(mMessageQueueLock);
         rval = mMessageQueue.front();
         mMessageQueue.pop();
      }

      return rval;
   }

   // Synchronous push
   void push(T s) {
      std::lock_guard<std::mutex> lock(mMessageQueueLock);
      mMessageQueue.emplace(s);

      // Signal the listeners
      mMessageSemaphore.Signal();
   }

private:

   std::string             mName;

   std::queue<T>           mMessageQueue;
   std::mutex              mMessageQueueLock;
   lmolly::semaphore       mMessageSemaphore;
};

}

#endif
