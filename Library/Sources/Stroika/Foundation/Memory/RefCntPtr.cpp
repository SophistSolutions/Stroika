/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"RefCntPtr.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;


/*
 ********************************************************************************
 ****************************** SimpleRefCntPtrBase *****************************
 ********************************************************************************
 */
void	RefCntPtrNS::Private::SimpleRefCntPtrBase::DO_DELETE_REF_CNT ()
{
	delete this;
}


