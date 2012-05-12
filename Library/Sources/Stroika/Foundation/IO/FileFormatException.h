/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileFormatException_h_
#define	_Stroika_Foundation_IO_FileFormatException_h_	1

#include	"../StroikaPreComp.h"

#include	"../Characters/TString.h"
#include	"../Execution/StringException.h"

namespace	Stroika {	
	namespace	Foundation {

		namespace	IO {

			using	Characters::TString;

			class	FileFormatException : public Execution::StringException {
				public:
					FileFormatException (const TString& fileName);

				public:
					nonvirtual	TString			GetFileName () const;

				private:
					TString	fFileName_;
			};

		}

		namespace	Execution {
			// Just pre-declare DoThrow><> template here so we can specailize
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const IO::FileFormatException& e2Throw);
		}

	}
}
#endif	/*_Stroika_Foundation_IO_FileFormatException_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"FileFormatException.inl"
