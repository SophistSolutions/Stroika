/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__osw32Memory_h__
#define	__osw32Memory_h__	1

#include	"../../StroikaPreComp.h"

#if		defined(_WIN32)
	#include	<Windows.h>
#endif

#include	<memory>

#include	"../../Configuration/Basics.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {
			namespace	OS_Win32 {

				class	StackBasedHandleLocker {
					public:
						StackBasedHandleLocker (HANDLE h);
						~StackBasedHandleLocker ();
					private:
						StackBasedHandleLocker (const StackBasedHandleLocker&);
						const StackBasedHandleLocker& operator= (const StackBasedHandleLocker&);

					public:
						nonvirtual	Byte*	GetPointer () const;
						nonvirtual	size_t	GetSize () const;

					private:
						HANDLE	fHandle;
						Byte*	fPointer;
				};

			}
		}
	}
}
#endif	/*__osw32Memory_h__*/


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Handle.inl"
