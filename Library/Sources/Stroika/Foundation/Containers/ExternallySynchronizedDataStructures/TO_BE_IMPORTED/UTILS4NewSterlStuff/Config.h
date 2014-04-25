#pragma once


#include <stddef.h>


#if     !defined (qDebug)
#if     defined(_DEBUG)
#define qDebug  1
#elif   defined (NDEBUG)
#define qDebug  0
#else
#define qDebug  1
#endif
#endif



#include <assert.h>



#define nonvirtual

#if qDebug

#define Assert(v)           assert ((v))
#define AssertNotReached()  assert (false)
#define AssertNotNull(v)    assert ((v) != nullptr)
#define RequireNotNull(v)   assert ((v) != nullptr)
#define Require(v)          assert ((v))
#define Ensure(v)           assert ((v))
#define Verify(c)           assert (c)

#else

#define Assert(v)
#define AssertNotReached()
#define AssertNotNull(v)
#define RequireNotNull(v)
#define Require(v)
#define Ensure(v)
#define Verify(v)           (v)

#endif  /* qDebug */



//#define NOMINMAX  1
