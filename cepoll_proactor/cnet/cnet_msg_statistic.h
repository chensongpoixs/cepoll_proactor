/********************************************************************
created: 		2019-05-27

author:			chensong

purpose:		cnet_msg_statistic
*********************************************************************/


#ifndef _C_NET_MSG_STATISTIC_H_
#define _C_NET_MSG_STATISTIC_H_
#include "casync_log.h"

namespace chen {
	class cnet_msg_statistic : private cnoncopyable
	{
	public:
		explicit cnet_msg_statistic();
		~cnet_msg_statistic();
		
	public:
		bool	init();
		void	destroy();
	private:
	};
} //namespace chen

#endif // !#define _C_NET_MSG_STATISTIC_H_