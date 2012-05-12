/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"SharedPtrBase.h"






using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;





/*
 ********************************************************************************
 ****************************** SimpleSharedPtrBase *****************************
 ********************************************************************************
 */
void	SharedPtrNS::Private::SimpleSharedPtrBase::DO_DELETE_REF_CNT ()
{
	delete this;
}






/*
 ********************************************************************************
 ****************************** SharedPtrBase ***********************************
 ********************************************************************************
 */
void	SharedPtrBase::DO_DELETE_REF_CNT ()
{
}
