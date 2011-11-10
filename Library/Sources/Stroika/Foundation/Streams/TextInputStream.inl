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


			inline	TextInputStream::~TextInputStream ()
				{
				}
			inline	size_t	TextInputStream::Read (Character* intoStart, Character* intoEnd)
				{
					RequireNotNull (intoStart);
					Require ((intoEnd - intoStart) >= 1);
					if (fPutBackCharValid_) {
						fPutBackCharValid_ = false;
						*intoStart = fPutBackCharacter_;
						return 1;
					}
					return _Read (intoStart, intoEnd);
				}
			inline	Character	TextInputStream::Read ()
				{
					if (fPutBackCharValid_) {
						fPutBackCharValid_ = false;
						return fPutBackCharacter_;
					}
					Character	c	=	'\0';
					size_t	n	=	_Read (&c, &c + 1);
					Assert (n == 0 or n == 1);
					return (n == 0)? '\0': c;
				}
		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextInputStream_inl_*/
