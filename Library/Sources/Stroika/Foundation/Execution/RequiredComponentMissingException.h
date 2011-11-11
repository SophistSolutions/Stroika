/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_RequiredComponentMissingException_h_
#define	_Stroika_Foundation_Execution_RequiredComponentMissingException_h_	1

#include	"../StroikaPreComp.h"

#include	"../Characters/String.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Characters::String;

			class	RequiredComponentMissingException : public StringException {
				public:
					static	const	wchar_t	kJava[];
					static	const	wchar_t	kPDFViewer[];
					static	const	wchar_t	kPDFOCXViewer[];
				public:
					RequiredComponentMissingException (const wstring& component);

				public:
					wstring	fComponent;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_RequiredComponentMissingException_h_*/
