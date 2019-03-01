/***********************************************************************************************
	created: 		2019-02-24
	
	author:			chensong
					
	purpose:		epoll_reactor
	
	later tasks:	null
************************************************************************************************/

#include "cepoll_reactor.h"
#include "cobject_pool.h"
#include "cscoped_lock.h"

namespace chen 
{
	bool cepoll_reactor::init()
	{
		m_epoll_fd_ = do_epoll_create();
		if (m_epoll_fd_ <= 0)
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "]" <<  " epoll_create alloc" << std::endl;
			return false;
		}
		
		// add epoll 
		{
			//csocket_ops::set_nonblocking(m_epoll_fd_);
			
			//epoll_event ev = { 0, { 0 } };
			//ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			//ev.data.ptr = m_epoll_fd_;
			//::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, m_epoll_fd_, &ev);
		}
		
		return true;
	}
	
	void cepoll_reactor::Destroy()
	{
		if (m_epoll_fd_ != -1)
		{
			::close(m_epoll_fd_);
		}
		/*
		if (m_timer_fd_ != -1)
		{
			::close(m_timer_fd_);
		}	
		*/
	}
	
	
	void cepoll_reactor::showdown()
	{
		
	}
	
	int32  cepoll_reactor::register_descriptor(socket_type descriptor, cdescriptor_state_queue *& descriptor_state)
	{
		descriptor_data = _allocate_descriptor_state();

		{
		scoped_lock descriptor_lock(descriptor_data->m_mutex_);

		descriptor_data->m_reactor_ = this;
		descriptor_data->m_descriptor_ = descriptor;
		descriptor_data->m_shutdown_ = false;
		}

	  epoll_event ev = { 0, { 0 } };
	  ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
	  descriptor_data->m_registered_events_ = ev.events;
	  ev.data.ptr = descriptor_data;
	  int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
	  if (result != 0)
	  {
		  return errno;
	  }

		return 0;
	}
	void cepoll_reactor::deregister_descriptor(socket_type descriptor, cdescriptor_state_queue *& descriptor_state, bool closing)
	{
		 if (!descriptor_state)
		 {
			return;
		 }

		  mutex::scoped_lock descriptor_lock(descriptor_data->m_mutex_);

		  if (!descriptor_data->m_shutdown_)
		  {
			if (closing)
			{
			  // The descriptor will be automatically removed from the epoll set when
			  // it is closed.
			}
			else
			{
			  epoll_event ev = { 0, { 0 } };
			  ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev);
			}

			op_queue<operation> ops;
			for (int i = 0; i < max_ops; ++i)
			{
			  while (reactor_op* op = descriptor_data->op_queue_[i].front())
			  {
				op->ec_ = boost::asio::error::operation_aborted;
				descriptor_data->op_queue_[i].pop();
				ops.push(op);
			  }
			}

			descriptor_data->descriptor_ = -1;
			descriptor_data->shutdown_ = true;

			descriptor_lock.unlock();

			free_descriptor_state(descriptor_data);
			descriptor_data = 0;

   
	}
	
	socket_type cepoll_reactor::do_epoll_create()
	{
		
		int fd = ::epoll_create(epoll_size);
		if (fd != -1)
		{
			::fcntl(fd, F_SETFD, FD_CLOEXEC);
		}
		if (fd == -1)
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "]" <<  " epoll_create alloc" << std::endl;
			return -1;
		}
		return fd;
	}
	
	cdescriptor_state_queue* cepoll_reactor::_allocate_descriptor_state()
	{
		// scoped_lock descriptors_lock(m_registered_descriptors_mutex_);
		//return m_registered_descriptors_.alloc();
		//return 
	}
	
	void cepoll_reactor::_free_descriptor_state(cdescriptor_state_queue* s)
	{
	 // scoped_lock descriptors_lock(m_registered_descriptors_mutex_);
	  //m_registered_descriptors_.free(s);
	}
	
} // chen