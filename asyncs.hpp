#ifndef ASYNC_SOURCE_HPP___
#define ASYNC_SOURCE_HPP___

#include <list>
#include <memory>
#include <mutex>
#include "semaphore.hpp"

namespace lmolly {

template <typename T>
class asyncsource {
 public:
  // Constructors
  // -------------------------------------------------------------------
  asyncsource() {
    mObjectCounter = std::make_shared<lmolly::semaphore>(0);
    mObjectList = std::make_shared<std::list<T>>();
    mMutex = std::make_shared<std::mutex>();
  }

  asyncsource(const asyncsource<T>& x)
      : mObjectCounter(x.mObjectCounter),
        mObjectList(x.mObjectList),
        mMutex(x.mMutex) {}

  asyncsource(asyncsource<T>&& x)
      : mObjectCounter(x.mObjectCounter),
        mObjectList(x.mObjectList),
        mMutex(x.mMutex) {}

  // Assignment
  // -------------------------------------------------------------------
  asyncsource<T>& operator=(asyncsource<T> x) {}

  // Destructor
  // -------------------------------------------------------------------
  virtual ~asyncsource() {}

  // Interface
  // -------------------------------------------------------------------
  bool wait(T& x) {
    mObjectCounter->wait();
    x = std::move(secure_pop());
    return true;
  }
  template <typename R, typename P>
  bool wait_for(T& x, const std::chrono::duration<R, P>& crRelTime) {
    bool rval = true;
    if (mObjectCounter->wait(crRelTime)) {
      x = std::move(secure_pop());
    } else {
      rval = false;
    }
    return rval;
  }

  bool push(T x) {
    // Add the object to the list
    bool rval = secure_push(x);
    if (rval) {
      // Signal the objct presence
      mObjectCounter->signal();
    }
    return rval;
  }

 private:
  std::shared_ptr<lmolly::semaphore> mObjectCounter;
  std::shared_ptr<std::list<T>> mObjectList;
  std::shared_ptr<std::mutex> mMutex;

  bool secure_push(T& x) {
    std::unique_lock<std::mutex> lock(*mMutex);
    mObjectList->push_back(std::move(x));
    return true;
  }

  T secure_pop() {
    std::unique_lock<std::mutex> lock(*mMutex);
    T rval = std::move(mObjectList->front());
    mObjectList->pop_front();
    return rval;
  }
};

}  // namespace lmolly

#endif
