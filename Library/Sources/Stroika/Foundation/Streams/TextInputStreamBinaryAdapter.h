/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define	_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"BinaryInputStream.h"
#include	"TextInputStream.h"


/*
 * TODO:
 *			o	Must use either stdc++ multibyte code or Stroika CodePage code - currnetly a hack assuming input is ascii.
 *
 *			o	CONSIDER REDOING USE SharedPtr<> - to make lifetime stuff workout better...
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			using	Characters::Character;

			/*
			 */
			class	TextInputStreamBinaryAdapter : public TextInputStream {
				public:
					TextInputStreamBinaryAdapter (BinaryInputStream& src);

				protected:
					virtual	size_t	_Read (Character* intoStart, Character* intoEnd) override;

				private:
					BinaryInputStream&	fSource_;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
