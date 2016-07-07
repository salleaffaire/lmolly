#ifndef AA_LISTENER_HPP___
#define AA_LISTENER_HPP___


#include <thread>
#include <chrono>

#include <list>
#include <sstream>

//#include <sched.h>

// Synchronized Queue
#include "aa_sq.hpp"

#include "aa_tools.hpp"

namespace lmolly {

static unsigned int num_of_cpu() {
   unsigned int rval = std::thread::hardware_concurrency();
   return rval;
}

// IO mutex instance
std::mutex gIOMutex;

template <class T> 
class aa_listener {
public:
   typedef bool (*consume_function)(T &);
   explicit aa_listener(lmolly::aa_sync_queue<T> *q, consume_function cf) : 
      mName(""), mCounter(0), mIsRunning(true), mpSQ(q), mpfC(cf)  {
      run();
   }   
   explicit aa_listener(lmolly::aa_sync_queue<T> *q, consume_function cf, std::string name) : 
      mName(name), mCounter(0), mIsRunning(true), mpSQ(q), mpfC(cf) {
      run();
   }
   virtual ~aa_listener() {
      mIsRunning = false;
      mpListeningThread->join();
      delete mpListeningThread;
   }

   void run() {
      mpListeningThread = new std::thread(&aa_listener::listen, this);
   }

   // Dispatcher function 
   // Simulates and pushes a message every 2 seconds
   void listen() {
      {
         std::lock_guard<std::mutex> iolock(gIOMutex);
         std::cout << "Listener " << mName << " running .... " << std::endl;
      }
      mCounter = 0;
      while (mIsRunning) {
         bool valid;
         T message = mpSQ->pop(std::chrono::milliseconds(1000), valid);
         if (valid) {
            std::cout << " " << mName << " intercepted a message on CPU " << std::endl;
            mpfC(message);
         }
      }
      std::cout << "Done .... " << std::endl;
   }

private:
   // Name of the listener
   std::string              mName;

   // Thread in which the consume function runs
   std::thread             *mpListeningThread;

   // Loop counter and running flag 
   // main loop exists when mIsRunning is set to false
   unsigned int             mCounter;
   bool                     mIsRunning;

   // Pointer to a synchronization queue 
   // where messages are poped from 
   lmolly::aa_sync_queue<T>   *mpSQ;

   // Pointer to a funcion that will be called on each intercepted message
   // typedef bool (*consume_function)(T &);
   consume_function         mpfC;
};

template <class T>
class aa_listener_pool {
public:
   typedef bool (*consume_function)(T &);

   explicit aa_listener_pool(lmolly::aa_sync_queue<T> *q, consume_function cf) :
      mName(""), mpSQ(q), mpfC(cf) {
      instanciate_pool();

   }
   explicit aa_listener_pool(lmolly::aa_sync_queue<T> *q, consume_function cf, std::string name) : 
      mName(name), mpSQ(q), mpfC(cf) {
      instanciate_pool();
   }
   ~aa_listener_pool() {
      for (auto &x : mListenerList) {
         delete x;
      }
   }

private:
   void instanciate_pool() {
      mpNumberOfListeners = num_of_cpu();
      for (int i=0;i<mpNumberOfListeners;++i) {
         std::stringstream ss;
         ss << mName;
         ss << "-" << i;
         mListenerList.push_back(new aa_listener<T>(mpSQ, mpfC, ss.str()));
      }
   }

   // Name of the listener pool
   std::string                   mName;

   // List of pointers to a listener
   std::list<aa_listener<T> *>   mListenerList;
   unsigned int                  mpNumberOfListeners;

   // Pointer to a synchronization queue 
   // where messages are poped from    
   lmolly::aa_sync_queue<T>        *mpSQ;

   // Pointer to a funcion that will be called on each intercepted message
   // typedef bool (*consume_function)(T &);   
   consume_function              mpfC;   
};

}

#endif
