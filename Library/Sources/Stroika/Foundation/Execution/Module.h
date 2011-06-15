/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Module_h_
#define	_Stroika_Foundation_Execution_Module_h_	1

#include	"../StroikaPreComp.h"

#include	"../Characters/StringUtils.h"
#include	"../Configuration/Basics.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Characters::tstring;

			tstring	GetEXEDir ();
			tstring	GetEXEPath ();

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
