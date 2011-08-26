/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_	1

#include	"../../StroikaPreComp.h"

#include	<istream>

#include	"../../Configuration/Common.h"
#include	"../../Execution/ProgressMonitor.h"
#include	"../../Memory/VariantValue.h"

#include	"Common.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {

				// Override any of these methods you want to recieve notification of the given event. There is no need to call the
				// inherited version.
				class	SAXCallbackInterface {
					public:
						virtual	void	StartDocument ();
						virtual	void	EndDocument ();
					public:
						virtual	void	StartElement (const wstring& uri, const wstring& localname, const wstring& qname, const map<wstring,Memory::VariantValue>&	attrs);
						virtual	void	EndElement (const wstring& uri, const wstring& localname, const wstring& qname);
					public:
						virtual	void	CharactersInsideElement (const wstring& text);
				};

				#if		qHasLibrary_Xerces
					/*
					 * Parse will throw an exception if it encouters any errors parsing.
					 */
					void	SAXParse (istream& in, SAXCallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);

					//SCHEMA STUFF NYI - SEE RFLLIB XMLDB
					//void	SAXParse (istream& in, const Schema& schema, CallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);
				#endif

			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SAXReader.inl"
