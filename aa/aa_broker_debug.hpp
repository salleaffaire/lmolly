#ifndef AA_BROKER_DEBUG_HTTP___
#define AA_BROKER_DEBUG_HTTP___

#include <chrono>
#include <thread>

// Synchronized Queue
#include "aa_sq.hpp"

#include "aa_tools.hpp"

namespace lmolly {

class aa_broker_debug {
 public:
  aa_broker_debug(lmolly::aa_sync_queue<std::string> *sq)
      : mIsRunning(true), mpSQ(sq) {
    // Start the listening loop
    run();
  }

  ~aa_broker_debug() {
    std::cout << "Deleting broker" << std::endl;
    mIsRunning = false;
    mpBrokerThread->join();
    delete mpBrokerThread;
  }

  void run() {
    mpBrokerThread = new std::thread(&aa_broker_debug::broker, this);
  }

  // Dispatcher function
  // Simulates and pushes a message every 2 seconds
  void broker() {
    // std::cout << "Dispatcher running .... " << std::endl;
    // std::cout << mIsRunning << std::endl;
    mCounter = 0;

    const unsigned int period_in_millisecond = 500;

    while (mIsRunning) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(period_in_millisecond));
      std::stringstream ss;
      ss << "Elapsed : " << mCounter++ * period_in_millisecond
         << " milliseconds" << std::endl;
      // std::cout << ss.str();
      mpSQ->push(ss.str());
    }
    std::cout << "Done .... " << std::endl;
  }

 private:
  std::thread *mpBrokerThread;

  unsigned int mCounter;
  bool mIsRunning;

  lmolly::aa_sync_queue<std::string> *mpSQ;
};

}  // namespace lmolly

#endif
