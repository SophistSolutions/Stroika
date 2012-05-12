/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_
#define	_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_	1

#include	"../../StroikaPreComp.h"

#include	<ostream>

#include	"../../Configuration/Common.h"
#include	"../../Memory/VariantValue.h"


/*
 * TODO:
 *		o	MUST RE-IMPLEMENT USING STROIKA TEXT_STREAM OUTPUT!!! BUT THATS NOT IMPLEMENTED/SHAKEN OUT ENUF!!! and needs tie-in to 
 *			stdio streams so can work effectively...
 *
 *		o	When we have BINPARYOUTPUTSTREAM_IOSTREAM - then we can use that to impem,ent the ostrean& overload and just use
 *			regular BINARYOUTPUTSTREAM version for basic impl.
 */



namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	JSON {


				/*
				 *
				 * The arguemnt VariantValue must be composed of any combination of these types:
				 *			o	Memory::VariantValue::eBoolean
				 *			o	Memory::VariantValue::eInteger
				 *			o	Memory::VariantValue::eFloat
				 *			o	Memory::VariantValue::eString
				 *			o	Memory::VariantValue::eMap
				 *			o	Memory::VariantValue::eArray
				 *	or it can be the type:
				 *			o	Memory::VariantValue::eNull
				 *
				 *	Other types are illegal an JSON and will trigger a 'Require' failure.
				 *
				 * Note that PrettyPrint () writes in UTF-8 format to the output stream.
				 */
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
