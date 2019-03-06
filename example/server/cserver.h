/***********************************************************************************************
	created: 		2019-03-07
	
	author:			chensong
					
	purpose:		server
************************************************************************************************/
#ifndef _C_NET_MGR_H_
#define _C_NET_MGR_H_

namespace chen
	
	class cserver :public cnoncopyable
	{
	public:
		explicit cserver():m_net_mgr(NULL){}
		virtual	~cserver() {}
	public:
		bool 	init(int32 port);
		bool 	Loop();
		void 	destroy();
		void 	set_stoping();
	private:
		cnet_mgr*		m_net_mgr;
	};
} // chen 

#endif // _C_NET_MGR_H_