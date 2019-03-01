/***********************************************************************************************
	created: 		2019-02-26
	
	author:			chensong
					
	purpose:		scoped_ptr
	
	Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
************************************************************************************************/

#ifndef _C_SCOPED_PTR_H_
#define _C_SCOPED_PTR_H_
#include "cnoncopyable.h"

namespace chen {

template <typename T>
class cscoped_ptr: private cnoncopyable
{
public:
  // Constructor.
  explicit cscoped_ptr(T* p = 0)
    : p_(p)
  {
  }

  // Destructor.
  ~cscoped_ptr()
  {
    delete p_;
  }

  // Access.
  T* get()
  {
    return p_;
  }

  // Access.
  T* operator->()
  {
    return p_;
  }

  // Dereference.
  T& operator*()
  {
    return *p_;
  }

  // Reset pointer.
  void reset(T* p = 0)
  {
    delete p_;
    p_ = p;
  }

private:
  // Disallow copying and assignment.
  //cscoped_ptr(const scoped_ptr&);
  //cscoped_ptr& operator=(const scoped_ptr&);

  T* p_;
};

} // namespace chen


#endif // _C_SCOPED_PTR_H_
