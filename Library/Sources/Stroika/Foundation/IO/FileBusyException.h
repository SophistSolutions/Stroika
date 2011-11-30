/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileBusyException_h_
#define	_Stroika_Foundation_IO_FileBusyException_h_	1

#include	"../StroikaPreComp.h"

#include	"../Characters/TString.h"
#include	"../Execution/StringException.h"

namespace	Stroika {	
	namespace	Foundation {

		namespace	IO {

			using	Characters::TString;

			class	FileBusyException : public Execution::StringException {
				public:
					FileBusyException (const TString& fileName = TString ());

				public:
					nonvirtual	TString			GetFileName () const;

				private:
					TString	fFileName_;
			};

		}

		namespace	Execution {
			// Re-declare so we can specialize (real declaration is in Execution/Excpetions.h)
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<>
				inline	void	_NoReturn_	DoThrow (const IO::FileBusyException& e2Throw);
		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileBusyException_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"FileBusyException.inl"
