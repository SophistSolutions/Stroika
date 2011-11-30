/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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
	}
}
#endif	/*_Stroika_Foundation_IO_FileFormatException_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"FileFormatException.inl"
