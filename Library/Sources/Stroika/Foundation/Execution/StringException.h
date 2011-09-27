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

					// Only implemented for wstring
					template	<typename T>
						T	As () const;

					operator wstring () const;

				private:
					wstring	fError;
			};

			template	<>
				wstring	StringException::As () const;


			// Re-declare so we can specialize (real declaration is in Execution/Excpetions.h)
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const StringException& e2Throw);
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
