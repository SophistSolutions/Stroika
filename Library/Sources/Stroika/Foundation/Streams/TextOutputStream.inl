/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_TextOutputStream_inl_
#define	_Stroika_Foundation_Streams_TextOutputStream_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			inline	TextOutputStream::TextOutputStream ()
				{
				}
			void	TextOutputStream::Write (const wchar_t* buffer, size_t bufSize)
				{
					RequireNotNil (buffer);
					Require (bufSize >= 1);
					Write_ (buffer, bufSize);
				}
		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextOutputStream_inl_*/
