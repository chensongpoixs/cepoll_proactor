/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
************************************************************************************************/
#include "cnet_session.h"


namespace chen  {
	
	
	
	bool 	cnet_session::init(socket_type	sockfd, int32 event)
	{
		if (sockfd <= 0)
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "][Warn]" <<  " sockfd <= 0 " << std::endl;
			return false;
		}
		
		if (m_sockfd == 0 )
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "][Warn]" <<  " sockfd =  " << m_sockfd << std::endl;
		}
		
		{
			cscoped_lock<std::mutex> lock;
			m_sockfd = sockfd;
			m_event = event;
		}
		
		return true;
	}
	void 	cnet_session::destroy()
	{
		
	}
	
	
}  // chen
