/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Character_h_
#define	_Stroika_Foundation_Characters_Character_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"






//tmphack to workaround gcc compilation issue - not sure if my bug or theirs yet...
#if qPlatform_Windows
#define qENABLE_SPECIALIZTION_STRHACK 1
#else
#define qENABLE_SPECIALIZTION_STRHACK 0
#endif


/*
@CONFIGVAR:		qNBytesPerWCharT
@DESCRIPTION:	<p>2 or 4</p>
*/
#if		!defined (qNBytesPerWCharT)
	#error "qNBytesPerWCharT should normally be defined indirectly by StroikaConfig.h"
#endif
#if		qNBytesPerWCharT != 2 && qNBytesPerWCharT != 4
	#error "qNBytesPerWCharT invalid value"
#endif



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


					template	<typename T>
						T	As () const;

// SHOULD do specialization for typedef unsigned short char16_t and char32_t as well!

#if qENABLE_SPECIALIZTION_STRHACK
					template<>
						wchar_t	As () const;
#endif

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
