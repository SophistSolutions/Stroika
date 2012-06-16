/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_    1

/*
 *
 * Description:
 *
 *
 *     ...
 */

#include    "../StroikaPreComp.h"

#include    "String.h"



/*
 * TODO:
 *
 *
 *		o	...
 *
 *
 */






namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

			/// VERY PRELIMINARY DRAFT
			class	RegularExpression {
			public:
				// not sure what these types mean - find out and document clearly
				enum SyntaxType { eECMAScript, eBasic };
				public:
					explicit RegularExpression (const String& re, SyntaxType syntaxType = eBasic)
						: fVal (re)
						, fSyntaxType (syntaxType)
					{
					}

			private:
					SyntaxType fSyntaxType;

			public:
				String	GetAsStr () const { return fVal; }
				SyntaxType	GetSyntaxType () const { return fSyntaxType; }
				private:
					String	fVal;
			};

        }
    }
}



#endif  /*_Stroika_Foundation_Characters_RegularExpression_h_*/


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "RegularExpression.inl"


