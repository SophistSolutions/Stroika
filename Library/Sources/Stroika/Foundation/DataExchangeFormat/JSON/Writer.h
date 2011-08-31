/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_
#define	_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_	1

#include	"../../StroikaPreComp.h"

#include	<ostream>

#include	"../../Configuration/Common.h"
#include	"../../Memory/VariantValue.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	JSON {

// MUST RE-IMPLEMENT USING STROIKA TEXT_STREAM OUTPUT!!! BUT THATS NOT IMPLEMENTED/SHAKEN OUT ENUF!!! and needs tie-in to 
// stdio streams so can work effectively...

				void	PrettyPrint (const Memory::VariantValue& v, ostream& out);

			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Writer.inl"
