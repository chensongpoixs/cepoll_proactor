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
class cop_queue;

class cop_queue_access
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
  static Operation*& front(cop_queue<Operation>& q)
  {
    return q.front_;
  }

  template <typename Operation>
  static Operation*& back(cop_queue<Operation>& q)
  {
    return q.back_;
  }
};

template <typename Operation>
class cop_queue
  : private cnoncopyable
{
public:
  // Constructor.
  cop_queue()
    : front_(0),
      back_(0)
  {
  }

  // Destructor destroys all operations.
  ~cop_queue()
  {
    while (Operation* op = front_)
    {
      pop();
      cop_queue_access::destroy(op);
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
      front_ = cop_queue_access::next(front_);
      if (front_ == 0)
	  {
		   back_ = 0;
	  }
      cop_queue_access::next(tmp, static_cast<Operation*>(0));
    }
  }

  // Push an operation on to the back of the queue.
  void push(Operation* h)
  {
    cop_queue_access::next(h, static_cast<Operation*>(0)); 
    if (back_)
    {
      cop_queue_access::next(back_, h);
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
  void push(cop_queue<OtherOperation>& q)
  {
    if (Operation* other_front = cop_queue_access::front(q))
    {
      if (back_)
	  {
		   cop_queue_access::next(back_, other_front);
	  }
      else
	  {
		   front_ = other_front;
	  }
      back_ = cop_queue_access::back(q);
      cop_queue_access::front(q) = 0;
      cop_queue_access::back(q) = 0;
    }
  }

  // Whether the queue is empty.
  bool empty() const
  {
    return front_ == 0;
  }

private:
  friend class cop_queue_access;

  // The front of the queue.
  Operation* front_;

  // The back of the queue.
  Operation* back_;
};

} // namespace chen



#endif // _C_QUEUE_H_
