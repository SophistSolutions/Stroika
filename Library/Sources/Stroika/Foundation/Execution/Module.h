/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Module_h_
#define	_Stroika_Foundation_Execution_Module_h_	1

#include	"../StroikaPreComp.h"

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Characters::TString;

			TString	GetEXEDir ();
			TString	GetEXEPath ();

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Module_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Module.inl"
