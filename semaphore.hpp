#ifndef SEMAPHORE_HPP___
#define SEMAPHORE_HPP___

#include <mutex>               // std::mutex, std::unique_lock
#include <condition_variable>  // std::condition_variable
#include <chrono>
 
// LM Library
namespace lmolly {

class semaphore
{   
public:
   inline semaphore()
      : mCount(0) { }

   inline semaphore(unsigned int count)
      : mCount(count) { }
   
   inline void Wait()
   {
      std::unique_lock<std::mutex> lock(mMutex);
      mCondition.wait(lock, [&]()->bool{ return mCount>0; });
      --mCount;
   }
   
   template <typename R, typename P>
   bool Wait(const std::chrono::duration<R,P>& crRelTime)
   {
      std::unique_lock<std::mutex> lock(mMutex);
      if (!mCondition.wait_for(lock, crRelTime, [&]()->bool{ return mCount>0; }))
         return false;
      --mCount;
      return true;
   }
   
   inline void Signal()
   {
      std::unique_lock<std::mutex> lock(mMutex);
      ++mCount;
      mCondition.notify_one();
   }

private:
   unsigned int            mCount;
   std::mutex              mMutex;
   std::condition_variable mCondition;
};

}

#endif
