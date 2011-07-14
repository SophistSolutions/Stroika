/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Character_h_
#define	_Stroika_Foundation_Characters_Character_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"


//// KEY ISSUES TO DECIDE:
//
//		o	Can we use a class with no loss of performacne (or must we use typdef wchar_t Character)
//
//		o	How do we handle char16_t versus char32_t - Windows uses 16bit, UNIX 32-bit. UNCLEAR how to handle here.
//			(LEANING TOWARDS FORCING USE OF 16bit char??) - maybe irrlevelnt if our STRING class internally stores stuff as
//			utf8
//

namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			class	Character	{
				public:
					Character ();
					Character (char c);
					Character (wchar_t wc);

					// Asserts the given characer is ascii
					nonvirtual	char	GetAsciiCode () const;


					nonvirtual	wchar_t	GetCharacterCode () const;

					operator wchar_t () const;

				private:
					wchar_t		fCharacterCode;
			};

			bool	operator== (Character lhs, Character rhs);
			bool	operator!= (Character lhs, Character rhs);
			bool	operator>= (Character lhs, Character rhs);
			bool	operator> (Character lhs, Character rhs);
			bool	operator<= (Character lhs, Character rhs);
			bool	operator< (Character lhs, Character rhs);
		}
	}
}
#endif	/*__Character__*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Character.inl"
