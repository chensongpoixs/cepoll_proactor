/***********************************************************************************************
					created: 		2019-02-24

					author:			chensong

					purpose:		epoll_reactor

					later tasks:	null
************************************************************************************************/

#include "cepoll_reactor.h"
#include "cobject_pool.h"
#include "cscoped_lock.h"
#include "csocket_ops.h"
//#include ""
#include "clog.h"

#if defined(__GNUC__)
namespace chen
{
	bool cepoll_reactor::init(socket_type listenfds, uint32 active_num)
	{
		if (active_num == 0 || active_num > 0xFFFF)
		{
			ERROR_EX_LOG(" max fd you not active_num =%lu", active_num);
			return false;
		}

		if (m_active == NULL)
		{
			m_active = new epoll_event[active_num];
			if (!m_active)
			{
				ERROR_EX_LOG(" alloc epoll active eror active_num =%lu", active_num);
				return false;
			}
		}

		if (m_epoll_fd_ == -1)
		{
			m_epoll_fd_ = ::epoll_create(active_num);
			if (m_epoll_fd_ == -1)
			{
				ERROR_EX_LOG(" epoll_create alloc ");
				return false;
			}
		}

		m_curfd_count = 0;
		m_maxfd_count = active_num;
	//	m_maxfd = listenfds;
		// add epoll 
		{
//csocket_ops::set_nonblocking(listenfds);
//		register_read_descriptor(listenfds, NULL);
			
//			register_readwrite_descriptor(m_epoll_fd_, NULL);

			//epoll_event ev = { 0, { 0 } };
			//ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			
	//		::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, listenfds, &ev);
			//csocket_ops::set_nonblocking(listenfds);

			//epoll_event ev = { 0, { 0 } };
			//ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			////ev.data.ptr = listenfds;
			//::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, listenfds, &ev);
		}

		return true;
	}

	void cepoll_reactor::destroy()
	{
		if (m_epoll_fd_ != -1)
		{
			csocket_ops::close_socket(m_epoll_fd_);
		}
		/*
		if (m_timer_fd_ != -1)
		{
			::close(m_timer_fd_);
		}
		*/
	}

	int32 cepoll_reactor::select(uint32 ms)
	{
		if (m_curfd_count == 0)
		{
			return 0;
		}
		int32 num_events = epoll_wait(m_epoll_fd_, m_active, m_maxfd_count, ms);
		//ERROR_EX_LOG(" num_events =%d", num_events);
		if (num_events > m_maxfd_count )
		{
			if (errno == EINTR)
			{
				return 0;
			}
		//	ERROR_EX_LOG("select failed, num_events=%d, errno=%d", num_events, errno);
			return 0;
		}
		//epoll_event events[128];
		//m_active.clear();
		//int64 num_events = epoll_wait(epoll_fd_, events, 128, ms);
		//for (int64 i = 0; i < num_events; ++i)
		{
			//m_active.emplace_back(&events[i]);  // core addr 
		}
		return num_events;
	}
	bool cepoll_reactor::is_read(uint32 index) const
	{
		return  (m_active[index].events & EPOLLIN);
	}
	bool cepoll_reactor::is_write(uint32 index) const
	{
		return  (m_active[index].events & EPOLLOUT);
	}
	bool cepoll_reactor::is_exception(uint32 index) const
	{
		return (m_active[index].events &(EPOLLERR | EPOLLHUP));
	}
	/*	void cepoll_reactor::showdown()
		{

		}
		*/
	//int32  cepoll_reactor::register_descriptor(socket_type descriptor, cnet_session *& session)
	//{



	//	epoll_event ev = { 0, { 0 } };
	//	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
	//	session->set_event(ev.events);
	//	ev.data.ptr = session;
	//	int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
	//	if (result != 0)
	//	{
	//		return errno;
	//	}
	//	return 0;
	//}
	void cepoll_reactor::deregister_descriptor(socket_type& descriptor)
	{
		if (descriptor == -1)
		{
			ERROR_EX_LOG("ERROR deregister_descriptor fd (socket_type=%d)", descriptor);
			return;
		}

		struct epoll_event ev;
		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev))
		{
			ERROR_EX_LOG("deregister_descriptor...epoll_ctl...DEL...");
			return;
		}
		--m_curfd_count;
	}

	int32 cepoll_reactor::_register_descriptor(socket_type& descriptor, uint32 st, void * session)
	{
		if (descriptor == -1)
		{
			WARNING_EX_LOG(" register descriptor failed");
			return 0;
		}
		struct epoll_event ev;

		ev.data.ptr = session;
		ev.events = EPOLLET;
		if ((st & EPOLLIN) != 0)
		{
			ev.events |= EPOLLIN;
		}
		if ((st & EPOLLOUT) != 0)
		{
			ev.events |= EPOLLOUT;
		}

		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev))
		{
			ERROR_EX_LOG("epoll_ctl failed error =%d", errno);
			return -1;
		}
		++m_curfd_count;
		return 0;
	}

	/*socket_type cepoll_reactor::do_epoll_create()
	{

		socket_type fd = ::epoll_create(epoll_size);
		if (fd != -1)
		{
			::fcntl(fd, F_SETFD, FD_CLOEXEC);
		}
		if (fd == -1)
		{
			ERRROR_EX_LOG(" epoll_create alloc" );
			return -1;
		}
		return fd;
	}*/



} //namespace chen

#endif //#if defined(__GNUC__)
