/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<sstream>

#include	"../../Characters/StringUtils.h"
#include	"../BadFormatException.h"

#include	"SAXReader.h"



#if		qHasLibrary_Xerces
	// avoid namespace conflcit with some Xerces code	
	#undef Assert

	#include	<xercesc/util/PlatformUtils.hpp>
	#include	<xercesc/util/XMLString.hpp>
	#include	<xercesc/util/BinInputStream.hpp>
	#include	<xercesc/dom/DOM.hpp>
	#include	<xercesc/framework/MemBufInputSource.hpp>
	#include	<xercesc/framework/MemBufFormatTarget.hpp>
	#include	<xercesc/util/XMLUni.hpp>
	#include	<xercesc/util/OutOfMemoryException.hpp>
	#include	<xercesc/sax2/XMLReaderFactory.hpp>
	#include	<xercesc/sax2/SAX2XMLReader.hpp>
	#include	<xercesc/sax2/DefaultHandler.hpp>	
	#include	<xercesc/validators/common/Grammar.hpp>
	#include	<xercesc/parsers/XercesDOMParser.hpp>
	#include	<xercesc/framework/LocalFileFormatTarget.hpp>
	#include	<xercesc/framework/XMLGrammarPoolImpl.hpp>
	#include	<xercesc/util/XMLEntityResolver.hpp>
	#include	<xercesc/sax/InputSource.hpp>
	#include	<xercesc/parsers/SAX2XMLReaderImpl.hpp>
	#if		defined (_DEBUG)
		#define	Assert(c)			{if (!(c)) { Stroika::Foundation::Debug::_Debug_Trap_ (__FILE__, __LINE__); }}
	#else
		#define	Assert(c)
	#endif
#endif



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::DataExchangeFormat;
using	namespace	Stroika::Foundation::DataExchangeFormat::XML;


using	Memory::VariantValue;








/*
 ********************************************************************************
 ******************************* SAXCallbackInterface ***************************
 ********************************************************************************
 */
void	SAXCallbackInterface::StartDocument ()
{
}

void	SAXCallbackInterface::EndDocument ()
{
}

void	SAXCallbackInterface::StartElement (const wstring& uri, const wstring& localname, const wstring& qname, const map<wstring,Memory::VariantValue>& attrs)
{
}

void	SAXCallbackInterface::EndElement (const wstring& uri, const wstring& localname, const wstring& qname)
{
}

void	SAXCallbackInterface::CharactersInsideElement (const wstring& text)
{
}







/*
 ********************************************************************************
 ************************************* SAXParse *********************************
 ********************************************************************************
 */

#if		qHasLibrary_Xerces
namespace	{
	class SAX2PrintHandlers_
			: public DefaultHandler
	{
		private:
			SAXCallbackInterface&	fCallback;

		public:
			SAX2PrintHandlers_ (SAXCallbackInterface& callback)
				: fCallback (callback)
				{
				}

		public:
			virtual		void startDocument () override
				{
					fCallback.StartDocument ();
				}
			virtual		void	endDocument () override
				{
					fCallback.EndDocument ();
				}

		public:
			virtual		void startElement (const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attributes) override
				{
					Require (uri != NULL);
					Require (localname != NULL);
					Require (qname != NULL);
					map<wstring,Value>	attrs;
					for (XMLSize_t i = 0; i < attributes.getLength(); i++) {
						const XMLCh* localAttrName = attributes.getLocalName (i);
						const XMLCh* val = attributes.getValue (i);
						attrs.insert (map<wstring,VariantValue>::value_type (localAttrName, val));
					}
					fCallback.StartElement (uri, localname, qname, attrs);
				}
			virtual		void	endElement (const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname) override
				{
					Require (uri != NULL);
					Require (localname != NULL);
					Require (qname != NULL);
					fCallback.EndElement (uri, localname, qname);
				}
			virtual		void	characters (const XMLCh* const chars, const XMLSize_t length) override
				{
					Require (chars != NULL);
					Require (length != 0);
					fCallback.CharactersInsideElement (wstring (chars, chars + length));
				}
	};
}

void	XML::SAXParse (istream& in, SAXCallbackInterface& callback, Execution::ProgressMontior* progres)
{
	SAX2PrintHandlers_	handler (callback);
	RefCntPtr<SAX2XMLReader>	parser	=	RefCntPtr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager));
	SetupCommonParserFeatures_ (*parser, false);
    parser->setContentHandler (&handler);
	parser->setErrorHandler (&sMyErrorReproter);
    parser->parse (StdIStream_InputSourceWithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.9f), L"XMLDB::SAX::Parse"));
}

#if 0
//SCHEMA NOT YET SUPPROTED
void	XML::SAXParse (istream& in, const Schema& schema, SAXCallbackInterface& callback, Execution::ProgressMontior* progres)
{
	if (schema.HasSchema ()) {
		SAX2PrintHandlers	handler (callback);
		Schema::AccessCompiledXSD	accessSchema (schema);// REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
		RefCntPtr<SAX2XMLReader>	parser	=	RefCntPtr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
		SetupCommonParserFeatures_ (*parser, true);
        parser->setContentHandler (&handler);
		parser->setErrorHandler (&sMyErrorReproter);
		parser->parse (StdIStream_InputSourceWithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.9f), L"XMLDB::SAX::Parse"));
	}
	else {
		Parse (in, callback, progressCallback);
	}
}
#endif
#endif

