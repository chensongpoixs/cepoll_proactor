/***********************************************************************************************
		created: 		2019-04-06

		author:			chensong

		purpose:		select_proactor
************************************************************************************************/


#ifndef _C_SELECT_PROACTOR_H_
#define _C_SELECT_PROACTOR_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#if defined(_MSC_VER)
namespace chen {
	class cselect_proactor :public cnoncopyable
	{
	public:
		explicit cselect_proactor() {}
		~cselect_proactor() {}

	public:
		bool init(socket_type socket_fd);
		void destroy();

		void shutdown();
		uint32 select(socket_type socket_fd);
	private:

	};

} // namespace chen
#endif // #if defined(_MSC_VER)
#endif //_C_SELECT_PROACTOR_H_