/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_
#define	_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_	1

#include	"../../StroikaPreComp.h"

#include	<istream>

#include	"../../Configuration/Common.h"
#include	"../../Memory/VariantValue.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	JSON {

// MUST RE-IMPLEMENT USING STROIKA TEXT_STREAM OUTPUT!!! BUT THATS NOT IMPLEMENTED/SHAKEN OUT ENUF!!! and needs tie-in to 
// stdio streams so can work effectively...

				// Reads one complete JSON value. Doesn't read past that - so not considered an error if extra stuff afterwards. MAY leave 'in' state ANYWHERE - don't
				// count on where its set after calling reader.
				Memory::VariantValue	Reader (istream& in);
				Memory::VariantValue	Reader (wistream& in);

			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
