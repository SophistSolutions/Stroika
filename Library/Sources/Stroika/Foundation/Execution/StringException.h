/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_StringException_h_
#define	_Stroia_Foundation_Execution_StringException_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			class	StringException {
				public:
					StringException (const wstring& reasonForError);

					operator wstring () const;

				private:
					wstring	fError;
			};

		}
	}
}
#endif	/*_Stroia_Foundation_Execution_StringException_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"StringException.inl"
