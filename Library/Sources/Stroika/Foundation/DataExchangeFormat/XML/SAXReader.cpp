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






#if		qHasLibrary_Xerces
	class	MyErrorReproter : public XMLErrorReporter, public ErrorHandler {
		// XMLErrorReporter
		public:
			virtual		void	error
				(
					const   unsigned int        errCode
					, const XMLCh* const        errDomain
					, const ErrTypes            type
					, const XMLCh* const        errorText
					, const XMLCh* const        systemId
					, const XMLCh* const        publicId
					, const XMLFileLoc          lineNum
					, const XMLFileLoc          colNum
				) override
				{
					Execution::DoThrow (ValidationFailed (errorText, static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0));
				}
			virtual	 void resetErrors () override
				{
				}

		// ErrorHandler
		public:
			virtual	 void warning (const SAXParseException& exc) override
				{
					// ignore
				}
			virtual	 void error (const SAXParseException& exc) override
				{
					AssertNotYetImplemented ();
					//Execution::DoThrow (ValidationFailed (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()), static_cast<unsigned int> (exc.getColumnNumber ()), 0));
				}
			virtual	 void fatalError (const SAXParseException& exc) override
				{
					AssertNotYetImplemented ();
					//Execution::DoThrow (ValidationFailed (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()), static_cast<unsigned int> (exc.getColumnNumber ()), 0));
				}
	};
	static	MyErrorReproter	sMyErrorReproter;
#endif





namespace	{
	#if		qHasLibrary_Xerces
		inline	void	SetupCommonParserFeatures_ (SAX2XMLReader& reader)
			{
				reader.setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
				reader.setFeature (XMLUni::fgXercesDynamic, false);
				reader.setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false);			// false:  * *Do not report attributes used for Namespace declarations, and optionally do not report original prefixed names
			}
		inline	void	SetupCommonParserFeatures_ (SAX2XMLReader& reader, bool validatingWithSchema)
			{
				reader.setFeature (XMLUni::fgXercesSchema, validatingWithSchema);
				reader.setFeature (XMLUni::fgSAX2CoreValidation, validatingWithSchema);

				// The purpose of this maybe to find errors with the schema itself, in which case,
				// we shouldn't bother (esp for release builds)
				//	(leave for now til we performance test)
				//		-- LGP 2007-06-21
				reader.setFeature (XMLUni::fgXercesSchemaFullChecking, validatingWithSchema);
				reader.setFeature (XMLUni::fgXercesUseCachedGrammarInParse, validatingWithSchema);
				reader.setFeature (XMLUni::fgXercesIdentityConstraintChecking, validatingWithSchema);

				// we only want to use loaded schemas - don't save any more into the grammar cache, since that
				// is global/shared.
				reader.setFeature (XMLUni::fgXercesCacheGrammarFromParse, false);
			}
	#endif
}




#if		qHasLibrary_Xerces
namespace	{
	// These SHOULD be part of xerces! Perhaps someday post them?
	class	StdIStream_InputSource : public InputSource {
		protected:
			class StdIStream_InputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
				public :
					StdIStream_InputStream (istream& in):
						fSource (in)
						{
							#if		defined (_DEBUG)
								::InterlockedIncrement (&sStdIStream_InputStream_COUNT);
							#endif
						}
					~StdIStream_InputStream ()
						{
							#if		defined (_DEBUG)
								::InterlockedDecrement (&sStdIStream_InputStream_COUNT);
							#endif
						}
				public:
					virtual	 XMLFilePos curPos () const override
						{
							return fSource.tellg ();
						}
					virtual		XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
						{
							fSource.read (reinterpret_cast<char*> (toFill), maxToRead);
							return static_cast<XMLSize_t> (fSource.gcount ());	// cast safe cuz amount asked to read was also XMLSize_t
						}
					virtual		const XMLCh* getContentType () const override
						{
							return NULL;
						}
				protected:
					istream&	fSource;
			};

		public :
			StdIStream_InputSource (istream& in, const XMLCh* const bufId = NULL):
				InputSource (bufId),
				fSource (in)
			{
			}
		virtual		BinInputStream* makeStream () const override
			{
				return new (getMemoryManager ()) StdIStream_InputStream (fSource);
			}
		protected:
			istream& fSource;
	};


	// my variations of StdIInputSrc with progresstracker callback
	class	StdIStream_InputSourceWithProgress : public StdIStream_InputSource {
		protected:
			class ISWithProg : public StdIStream_InputSource::StdIStream_InputStream {
				public :
					ISWithProg (istream& in, ProgressStatusCallback* progressCallback):
						StdIStream_InputStream (in),
						fProgress (progressCallback, 0.0f, 1.0f),
						fTotalSize (0.0f)
					{
						streamoff	start	=	in.tellg ();
						in.seekg (0, ios_base::end);
						streamoff totalSize	=	in.tellg ();
						Assert (start <= totalSize);
						in.seekg (start, ios_base::beg);
						fTotalSize = static_cast<float> (totalSize);
					}
				public :
					virtual		XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
						{
							float	curOffset			=	0.0;
							bool	doProgressBefore	=	(maxToRead > 10*1024);	// only bother calling both before & after if large read
							if (fTotalSize > 0.0f and doProgressBefore) {
								curOffset = fSource? static_cast<float> (fSource.tellg ()):  fTotalSize;
								fProgress.SetCurrentProgress (curOffset / fTotalSize);
							}
							fSource.read (reinterpret_cast<char*> (toFill), maxToRead);
							XMLSize_t	result	=	static_cast<XMLSize_t> (fSource.gcount ());	// safe cast cuz read maxToRead bytes
							if (fTotalSize > 0) {
								curOffset = fSource? static_cast<float> (fSource.tellg ()):  fTotalSize;
								fProgress.SetCurrentProgress (curOffset / fTotalSize);
							}
							return result;
						}
				private:
					float			fTotalSize;
					ProgressSubTask	fProgress;
			};

		public :
			StdIStream_InputSourceWithProgress (istream& in, ProgressStatusCallback* progressCallback, const XMLCh* const bufId = NULL):
				StdIStream_InputSource (in, bufId),
				fProgressCallback (progressCallback)
			{
			}
		virtual		BinInputStream* makeStream () const override
			{
				return new (getMemoryManager ()) ISWithProg (fSource, fProgressCallback);
			}
		private:
			ProgressStatusCallback* fProgressCallback;
	};

}
#endif




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

