/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_TextInputStream_inl_
#define	_Stroika_Foundation_Streams_TextInputStream_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			inline	TextInputStream::TextInputStream ()
				{
				}
			inline	size_t	TextInputStream::Read (wchar_t* buffer, size_t bufSize)
				{
					RequireNotNull (buffer);
					Require (bufSize >= 1);
					return _Read (buffer, bufSize);
				}
			inline	wchar_t	TextInputStream::Read ()
				{
					wchar_t	c	=	'\0';
					size_t	n	=	_Read (&c, 1);
					Assert (n == 0 or n == 1);
					return (n == 0)? '\0': c;
				}
		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextInputStream_inl_*/
