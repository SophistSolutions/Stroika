/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BinaryOutputStream_inl_
#define	_Stroika_Foundation_Streams_BinaryOutputStream_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			inline	BinaryOutputStream::BinaryOutputStream ()
				{
				}
			inline	void	BinaryOutputStream::Write (const Byte* buffer, size_t bufSize)
				{
					RequireNotNil (buffer);
					Require (bufSize >= 1);
					Write_ (buffer, bufSize);
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
