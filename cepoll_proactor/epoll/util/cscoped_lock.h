/***********************************************************************************************
	created: 		2019-02-26
	
	author:			chensong
					
	purpose:		scoped_lock
	
	Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
************************************************************************************************/
#ifndef _C_SCOPED_LOCK_H_
#define _C_SCOPED_LOCK_H_
#include "cnoncopyable.h"
namespace chen {


// Helper class to lock and unlock a mutex automatically.  
template <typename Mutex>
class cscoped_lock  // ==>std::lock_guard
  : private cnoncopyable
{
public:
  // Tag type used to distinguish constructors.
  enum adopt_lock_t { adopt_lock };

  // Constructor adopts a lock that is already held.
  cscoped_lock(Mutex& m, adopt_lock_t)
    : mutex_(m),
      locked_(true)
  {
  }

  // Constructor acquires the lock.
  explicit cscoped_lock(Mutex& m)
    : mutex_(m)
  {
    mutex_.lock();
    locked_ = true;
  }

  // Destructor releases the lock.
  ~cscoped_lock()
  {
    if (locked_)
      mutex_.unlock();
  }

  // Explicitly acquire the lock.
  void lock()
  {
    if (!locked_)
    {
      mutex_.lock();
      locked_ = true;
    }
  }

  // Explicitly release the lock.
  void unlock()
  {
    if (locked_)
    {
      mutex_.unlock();
      locked_ = false;
    }
  }

  // Test whether the lock is held.
  bool locked() const
  {
    return locked_;
  }

  // Get the underlying mutex.
  Mutex& mutex()
  {
    return mutex_;
  }

private:
  // The underlying mutex.
  Mutex& mutex_;

  // Whether the mutex is currently locked or unlocked.
  bool locked_;
};

} // namespace chen

#endif // _C_SCOPED_LOCK_H_
