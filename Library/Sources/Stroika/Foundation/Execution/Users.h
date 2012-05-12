/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Users_h_
#define	_Stroika_Foundation_Execution_Users_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Characters/String.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			enum	UserNameFormat {
				ePrettyName_UNF,
				eDEFAULT_UNF = ePrettyName_UNF,
			};
			Characters::String	GetCurrentUserName (UserNameFormat format = eDEFAULT_UNF);

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Users_h_*/
