/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
************************************************************************************************/
#include "cnet_session.h"
#include "clog.h"
#include "cscoped_ptr.h"
namespace chen  {
	
	
	
	bool 	cnet_session::init(socket_type	sockfd)
	{
		if (sockfd <= 0)
		{
			WARNING_EX_LOG(" sockfd <= 0\n ");
			return false;
		}
		
		/*if (m_sockfd == 0 )
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "][Warn]" <<  " sockfd =  " << m_sockfd << std::endl;
		}*/
		
		{
			cscoped_ptr<std::mutex> lock;
			m_sockfd = sockfd;
			//m_event = event;
		}
		
		return true;
	}
	void cnet_session::clearup()
	{
		m_status = ESS_INIT;
		if ((m_sockfd & 0XFFFF0000) == 0XFFFF0000)
		{
			m_sockfd += 0X00020000;
		}
		else
		{
			m_sockfd += 0X00010000;
		}
	}
	void cnet_session::set_event(int32 event)
	{
	}
	void 	cnet_session::destroy()
	{
		
	}
	
	
}  // chen
