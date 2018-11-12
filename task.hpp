#ifndef TASK_HPP___
#define TASK_HPP___

#include <chrono>
#include <thread>

namespace lmolly {

class task {
 public:
  task() : mpThread((std::thread *)0), mCounter(0), mIsRunning(false) {}

  virtual ~task() { stop_and_wait(); }

  virtual int init() = 0;
  virtual int clean() = 0;
  virtual int run() = 0;

  static int mThreadCount;

  void start(bool skipInit) {
    mSkipInit = skipInit;
    if (!mIsRunning) {
      // Set the running flag to true
      mIsRunning = true;

      // Start the thread
      mpThread = new std::thread(processing_loop, this);
    }
  }

  void stop_and_wait() {
    if (mIsRunning) {
      // Set the running flag to false
      mIsRunning = false;

      // Join the main thread
      mpThread->join();

      // Delete the thread object
      if (mpThread) delete mpThread;
      mpThread = 0;
    }
  }

 private:
  std::thread *mpThread;

  // Loop counter and running flag
  // main loop exists when mIsRunning is set to false
  uint64_t mCounter;
  bool mIsRunning;
  bool mSkipInit;

  static void processing_loop(task *that) {
    int rval = 0;
    // Try to initialize until it succeeds
    // i.i. Init() returns 0
    if (!(that->mSkipInit)) {
      do {
        rval = that->init();
      } while (rval != 0);
    }

    // Run until either the task is not running anymore or
    // Run returns something else than 0
    do {
      rval = that->run();
      ++(that->mCounter);
    } while (that->mIsRunning && (rval == 0));

    // Clean everything up
    that->clean();
  }
};

};  // namespace lmolly
#endif