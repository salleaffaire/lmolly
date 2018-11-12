#ifndef CTEST_HPP___
#define CTEST_HPP___

namespace lmolly {

class ctest {
 public:
  explicit ctest() : mLog(false), mID(0) {
    if (mLog) {
      std::cout << "CTest :: Default Constructor" << std::endl;
    }
  }
  explicit ctest(unsigned int id) : mLog(false), mID(id) {
    if (mLog) {
      std::cout << "CTest :: Default Constructor" << std::endl;
    }
  }
  explicit ctest(bool log, unsigned int id) : mLog(log), mID(id) {
    if (mLog) {
      std::cout << "CTest :: Default Constructor" << std::endl;
    }
  }
  ctest(const ctest& x) : mLog(x.mLog), mID(x.mID) {
    if (mLog) {
      std::cout << "CTest :: Copy Constructor" << std::endl;
    }
  }
  ctest(ctest&& x) : mLog(x.mLog), mID(x.mID) {
    if (mLog) {
      std::cout << "CTest :: Move Constructor" << std::endl;
    }
  }

  ctest& operator=(ctest x) {
    if (mLog) {
      std::cout << "CTest :: Assignment Operator" << std::endl;
    }
    mLog = x.mLog;
    mID = x.mID;
    return *this;
  }

  ~ctest() {
    if (mLog) {
      std::cout << "CTest :: Destructor" << std::endl;
    }
  }

  unsigned int get_id() { return mID; }

 private:
  unsigned int mID;
  bool mLog;
};

}  // namespace lmolly
#endif