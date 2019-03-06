/***********************************************************************************************
	created: 		2019-02-26
	
	author:			chensong
					
	purpose:		queue
	
	Copyright boost
************************************************************************************************/

#ifndef _C_QUEUE_H_
#define _C_QUEUE_H_
#include "cnoncopyable.h"

namespace chen {

template <typename operation>
class cqueue;

class cqueue_access
{
public:
  template <typename Operation>
  static Operation* next(Operation* o)
  {
    return static_cast<Operation*>(o->next_);
  }

  template <typename Operation1, typename Operation2>
  static void next(Operation1*& o1, Operation2* o2)
  {
    o1->next_ = o2;
  }

  template <typename Operation>
  static void destroy(Operation* o)
  {
    o->destroy();
  }

  template <typename Operation>
  static Operation*& front(cqueue<Operation>& q)
  {
    return q.front_;
  }

  template <typename Operation>
  static Operation*& back(cqueue<Operation>& q)
  {
    return q.back_;
  }
};

template <typename Operation>
class cqueue
  : private cnoncopyable
{
public:
  // Constructor.
  cqueue()
    : front_(0),
      back_(0)
  {
  }

  // Destructor destroys all operations.
  ~cqueue()
  {
    while (Operation* op = front_)
    {
      pop();
      cqueue_access::destroy(op);
    }
  }

  // Get the operation at the front of the queue.
  Operation* front()
  {
    return front_;
  }

  // Pop an operation from the front of the queue.
  void pop()
  {
    if (front_)
    {
      Operation* tmp = front_;
      front_ = cqueue_access::next(front_);
      if (front_ == 0)
	  {
		   back_ = 0;
	  }
      cqueue_access::next(tmp, static_cast<Operation*>(0));
    }
  }

  // Push an operation on to the back of the queue.
  void push(Operation* h)
  {
    cqueue_access::next(h, static_cast<Operation*>(0)); 
    if (back_)
    {
      cqueue_access::next(back_, h);
      back_ = h;
    }
    else
    {
      front_ = back_ = h;
    }
  }

  // Push all operations from another queue on to the back of the queue. The
  // source queue may contain operations of a derived type.
  template <typename OtherOperation>
  void push(cqueue<OtherOperation>& q)
  {
    if (Operation* other_front = cqueue_access::front(q))
    {
      if (back_)
	  {
		   cqueue_access::next(back_, other_front);
	  }
      else
	  {
		   front_ = other_front;
	  }
      back_ = cqueue_access::back(q);
      cqueue_access::front(q) = 0;
      cqueue_access::back(q) = 0;
    }
  }

  // Whether the queue is empty.
  bool empty() const
  {
    return front_ == 0;
  }

private:
  friend class cqueue_access;

  // The front of the queue.
  Operation* front_;

  // The back of the queue.
  Operation* back_;
};

} // namespace chen



#endif // _C_QUEUE_H_
