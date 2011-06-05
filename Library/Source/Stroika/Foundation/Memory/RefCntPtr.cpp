/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"RefCntPtr.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;


/*
 ********************************************************************************
 ****************************** SimpleRefCntPtrBase *****************************
 ********************************************************************************
 */
void	RefCntPtrNS::Private::SimpleRefCntPtrBase::DO_DELETE_REF_CNT ()
{
	delete this;
}


