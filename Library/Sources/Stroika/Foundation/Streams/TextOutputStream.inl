/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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

			//	class	TextOutputStream
			inline	TextOutputStream::TextOutputStream ()
				{
				}
			inline	TextOutputStream::~TextOutputStream ()
				{
				}
			void	TextOutputStream::Write (const Character* start, const Character* end)
				{
					RequireNotNull (start);
					Require (end - start >= 1);
					_Write (start, end);
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextOutputStream_inl_*/
