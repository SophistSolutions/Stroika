/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Mos32emory_inl
#define	_Mos32emory_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Debug/Assertions.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {
			namespace	OS_Win32 {


			//	class	StackBasedHandleLocker
				inline	StackBasedHandleLocker::StackBasedHandleLocker (HANDLE h):
					fHandle (h)
					{
						RequireNotNil (h);
						fPointer = reinterpret_cast<Byte*> (::GlobalLock (h));
					}
				inline	StackBasedHandleLocker::~StackBasedHandleLocker ()
					{
						::GlobalUnlock (fHandle);
					}
				inline	Byte*	StackBasedHandleLocker::GetPointer () const
					{
						return fPointer;
					}
				inline	size_t	StackBasedHandleLocker::GetSize () const
					{
						return ::GlobalSize (fHandle);
					}


			}
		}
	}
}
#endif	/*_Mos32emory_inl*/
