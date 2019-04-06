/***********************************************************************************************
	created: 		2019-02-24
	
	author:			chensong
					
	purpose:		epoll_reactor
	
	later tasks:	null
************************************************************************************************/

#include "cepoll_reactor.h"
#include "cobject_pool.h"
#include "cscoped_lock.h"
//#include ""
#include "clog.h"

#if defined(__GNUC__)

namespace chen 
{
	bool cepoll_reactor::init(socket_type sockfd)
	{
		if (sockfd <= 0)
		{
			ERROR_EX_LOG("listend fd = %u\n", sockfd);
			return false;
		}
		m_epoll_fd_ = do_epoll_create();
		if (m_epoll_fd_ <= 0)
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "]" <<  " epoll_create alloc" << std::endl;
			return false;
		}
		
		// add epoll 
		{
			csocket_ops::set_nonblocking(m_epoll_fd_);
			
			epoll_event ev = { 0, { 0 } };
			ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			ev.data.ptr = m_epoll_fd_;
			::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, sockfd, &ev);
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
	
	int64 select(uint32 csleep)
	{
		epoll_event events[128];
		m_active.clear();
		int64 num_events = epoll_wait(epoll_fd_, events, 128, csleep);
		for (int64 i = 0; i < num_events; ++i)
		{
			m_active.emplace_back(&events[i]);  // core addr 
		}
		return num_events;
	}
/*	void cepoll_reactor::showdown()
	{
		 
	}
	*/
	int32  cepoll_reactor::register_descriptor(socket_type descriptor, cnet_session *& session)
	{
		
		
		
	epoll_event ev = { 0, { 0 } };
	  ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
	  session->set_event ( ev.events);
	  ev.data.ptr = session;
	  int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
	  if (result != 0)
	  {
		  return errno;
	  }
		return 0;
	}
	void cepoll_reactor::deregister_descriptor(socket_type descriptor)
	{
		if (!session)
		{
			return;
		}
		 epoll_event ev = { 0, { 0 } };
		::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev);
	}
	
	socket_type cepoll_reactor::do_epoll_create()
	{
		
		socket_type fd = ::epoll_create(epoll_size);
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
	
	
	
} // chen

#endif /// linux epoll
