/***********************************************************************************************
			created: 		2019-04-06

			author:			chensong

			purpose:		select_proactor
************************************************************************************************/

#include "cselect_proactor.h"

#if defined(_MSC_VER)
namespace chen {
	bool cselect_proactor::init(socket_type socket_fd)
	{
		return false;
	}
	void cselect_proactor::destroy()
	{
	}
	void cselect_proactor::shutdown()
	{
	}
	uint32 cselect_proactor::select(socket_type socket_fd)
	{
		return uint32();
	}
} // namespace chen 

#endif // #if defined(_MSC_VER)