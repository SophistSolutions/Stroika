/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Character_h_
#define	_Stroika_Foundation_Characters_Character_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"



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
