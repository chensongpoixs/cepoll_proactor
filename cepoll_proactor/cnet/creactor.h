/***********************************************************************************************
			created: 		2019-04-06

			author:			chensong

			purpose:		reactor
************************************************************************************************/
#ifndef _C_REACTOR_H_
#define _C_REACTOR_H_
#if defined(_MSC_VER)
#include "cselect_reactor.h"
#elif defined(__GNUC__)
#include "cepoll_reactor.h"

#else
#pragma error "unknow platform!!!"

#endif

namespace chen {
#if defined(_MSC_VER)
	typedef cselect_reactor		creactor;
#elif defined(__GNUC__)
	typedef cepoll_reactor			creactor;
#else
#pragma error "unknow platform!!!"

#endif
} // namespace chen

#endif // !_C_REACTOR_H_