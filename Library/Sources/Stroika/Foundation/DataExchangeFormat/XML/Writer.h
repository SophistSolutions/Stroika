/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Characters/String.h"
#include	"../../Memory/Optional.h"

#include	"Common.h"


/*
* TODO:
*
*		(1)		Must add SOME kind of better writing support, but not clear how todo significantly better than
*				just streamed output. Maybe soemthign more structured todo indents automatically?
*
*		(o)		Short term- convert to using only Stroika string class (but make easy to still leverage from HF - think this out carefully befor change)
*
*/



namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {

				using	Characters::String;

				/*
				 */
				string	QuoteForXMLAttribute (const string& s);
				string	QuoteForXMLAttribute (const wstring& s);		// encode non-ascii characters as entity-references
				string	QuoteForXMLAttribute (const String& s);
				string	QuoteForXMLAttribute (const Memory::Optional<String>& s);

				/*
				 */
				wstring	QuoteForXMLAttributeW (const wstring& s);
				
				/*
				 * This function only emits ascii characters (so makes no assumptions about the codepage used for writing xml). It
				 * emits non-ascii characters as entity references.
				 */
				string	QuoteForXML (const string& s);
				string	QuoteForXML (const wstring& s);		// encode non-ascii characters as entity-references
				string	QuoteForXML (const String& s);
				string	QuoteForXML (const Memory::Optional<String>& s);

				/*
				 */
				wstring	QuoteForXMLW (const wstring& s);

			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Writer.inl"
