/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Characters/String.h"

#include	"Common.h"


/*
* TODO:
*
*		(1)		Must add SOME kind of better writing support, but not clear how todo significantly better than
*				just streamed output. Maybe soemthign more structured todo indents automatically?
*
*/



namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {

				string	QuoteForXMLAttribute (const string& s);
				string	QuoteForXMLAttribute (const wstring& s);		// encode non-ascii characters as entity-references
				wstring	QuoteForXMLAttributeW (const wstring& s);
				string	QuoteForXML (const string& s);
				string	QuoteForXML (const wstring& s);		// encode non-ascii characters as entity-references
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
