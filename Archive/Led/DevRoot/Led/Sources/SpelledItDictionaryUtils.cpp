/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SpelledItDictionaryUtils.cpp,v 2.4 2003/06/27 13:29:12 lewis Exp $
 *
 * Changes:
 *	$Log: SpelledItDictionaryUtils.cpp,v $
 *	Revision 2.4  2003/06/27 13:29:12  lewis
 *	comment out debugging call
 *	
 *	Revision 2.3  2003/06/19 23:20:14  lewis
 *	for SPR#1537- added eMergedOut status, and be more careful about error conditions reading XML files without all attributes specified
 *	
 *	Revision 2.2  2003/06/19 16:06:35  lewis
 *	fix writing code to emit good whitespace so that I can do nice diffs on dictionary files when I store in CVS (and so I can see nesting better)
 *	
 *	Revision 2.1  2003/06/17 23:56:54  lewis
 *	SPR#1536: added SpelledItDictionaryUtils utilities to read/write XML-format dict word files
 *	
 *	
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedConfig.h"
#include	"SpelledItDictionaryUtils.h"


#if		qXMLDictionaryFormatSupported && qWindows
	#include	<msxml2.h>
#endif

#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
		namespace	SpelledIt {
#endif


namespace {
	inline	void	ThrowIfError (HRESULT hr)
		{
			Led_ThrowIfErrorHRESULT (hr);
		}

	inline	void	AppendWhitespace (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> parentElt, const char* text)
		{
			// Add white space.     
			//currNode=doc.DocumentElement;
			//XmlWhitespace ws = doc.CreateWhitespace("\r\n");
			//currNode.InsertAfter(ws, currNode.FirstChild);
			CComQIPtr<IXMLDOMNode> whitespaceElt;
			ThrowIfError (doc->createNode (CComVariant (NODE_TEXT), CComBSTR (""), CComBSTR (""), &whitespaceElt));
			ThrowIfError (whitespaceElt->put_text (CComBSTR (text)));
			ThrowIfError (parentElt->appendChild (whitespaceElt, NULL));
		}

	inline	wstring	Status2String (WordInfo::Status s)
		{
			switch (s) {
				case	WordInfo::eEnabled:		return L"enabled";
				case	WordInfo::eDisabled:	return L"disabled";
				case	WordInfo::eMergedOut:	return L"merged-out";
				default: Led_Assert (false);	return L"disabled";
			}
		}

	inline	WordInfo::Status	String2Status (const wstring& s)
		{
			if (s == L"enabled")	{	return WordInfo::eEnabled;	}
			if (s == L"disabled")	{	return WordInfo::eDisabled;	}
			if (s == L"merged-out")	{	return WordInfo::eMergedOut;}
			return WordInfo::eEnabled;	// interpret any other strings as just meaning enabled...
		}
}







/*
 ********************************************************************************
 ******************************** DictionaryIO **********************************
 ********************************************************************************
 */
const	wchar_t	DictionaryIO::kTag_DictionarySource[]					=	L"DictionarySource";
const	wchar_t	DictionaryIO::kTag_WordList[]							=	L"WordList";
const	wchar_t	DictionaryIO::kTag_URLList[]							=	L"URLList";
const	wchar_t	DictionaryIO::kTag_Word[]								=	L"Word";
const	wchar_t	DictionaryIO::kTag_URL[]								=	L"URL";
const	wchar_t	DictionaryIO::kAttribute_DictionaryName[]				=	L"Name";
const	wchar_t	DictionaryIO::kAttribute_DateFound[]					=	L"Date-Found";
const	wchar_t	DictionaryIO::kAttribute_Status[]						=	L"Status";
const	wchar_t	DictionaryIO::kAttribute_OccuranceCount[]				=	L"OccuranceCount";
#if		qWindows
	const	CComBSTR	DictionaryIO::kBSTRTag_DictionarySource			=	DictionaryIO::kTag_DictionarySource;
	const	CComBSTR	DictionaryIO::kBSTRTag_WordList					=	DictionaryIO::kTag_WordList;
	const	CComBSTR	DictionaryIO::kBSTRTag_URLList					=	DictionaryIO::kTag_URLList;
	const	CComBSTR	DictionaryIO::kBSTRTag_Word						=	DictionaryIO::kTag_Word;
	const	CComBSTR	DictionaryIO::kBSTRTag_URL						=	DictionaryIO::kTag_URL;
	const	CComBSTR	DictionaryIO::kBSTRAttribute_DictionaryName		=	DictionaryIO::kAttribute_DictionaryName;
	const	CComBSTR	DictionaryIO::kBSTRAttribute_DateFound			=	DictionaryIO::kAttribute_DateFound;
	const	CComBSTR	DictionaryIO::kBSTRAttribute_Status				=	DictionaryIO::kAttribute_Status;
	const	CComBSTR	DictionaryIO::kBSTRAttribute_OccuranceCount		=	DictionaryIO::kAttribute_OccuranceCount;
#endif





#if		qXMLDictionaryFormatSupported
/*
 ********************************************************************************
 ***************************** XMLDictionaryReader ******************************
 ********************************************************************************
 */
XMLDictionaryReader::XMLDictionaryReader ():
	inherited ()
{
}

void	XMLDictionaryReader::Load (const TCHAR* fileName)
{
	CComQIPtr<IXMLDOMDocument>	pDoc;
	ThrowIfError (::CoCreateInstance (__uuidof (DOMDocument), NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDoc));
	ThrowIfError (pDoc->put_validateOnParse (false));
	VARIANT_BOOL	success;
	ThrowIfError (pDoc->load (CComVariant (fileName), &success));
	if (success) {
		CComQIPtr<IXMLDOMElement> topLevelElt;
		ThrowIfError (pDoc->get_documentElement (&topLevelElt));
		DoRead (topLevelElt);
	}
}

void	XMLDictionaryReader::DoRead (CComQIPtr<IXMLDOMElement> topLevelElt)
{
	CComBSTR	nodeName;
	ThrowIfError (topLevelElt->get_nodeName (&nodeName));
	if (nodeName != kBSTRTag_DictionarySource) {
		throw E_FAIL;	// better err handling!
	}
	CComVariant	comDictName;
	if (SUCCEEDED (topLevelElt->getAttribute (kBSTRAttribute_DictionaryName, &comDictName)) and
		comDictName.vt == VT_BSTR and
		comDictName.bstrVal != NULL
		) {
		fDictionaryName = comDictName.bstrVal;
	}
	else {
		fDictionaryName = L"";
	}
	CComQIPtr<IXMLDOMNode>	subList;
	ThrowIfError (topLevelElt->get_firstChild (&subList));
	while (subList.p != NULL) {
		CComBSTR	listName;
		ThrowIfError (subList->get_nodeName (&listName));
		if (listName == kBSTRTag_WordList) {
			Read_WordList (CComQIPtr<IXMLDOMElement> (subList));
		}
		else if (listName == kBSTRTag_URLList) {
			Read_URLList (CComQIPtr<IXMLDOMElement> (subList));
		}
		CComQIPtr<IXMLDOMNode>	next;
		ThrowIfError (subList->get_nextSibling (&next));
		subList = next;
	}
}

void	XMLDictionaryReader::Read_WordList (CComQIPtr<IXMLDOMElement> wordList)
{
	// now iterate over all elements
	CComBSTR	nodeName;
	ThrowIfError (wordList->get_nodeName (&nodeName));
	if (nodeName != kBSTRTag_WordList) {
		throw E_FAIL;	// better err handling!
	}
	CComQIPtr<IXMLDOMNode>	curWord;
	ThrowIfError (wordList->get_firstChild (&curWord));
	while (curWord.p != NULL) {
		CComBSTR	wordTagName;
		ThrowIfError (curWord->get_nodeName (&wordTagName));
		if (wordTagName != kBSTRTag_Word) {
			throw E_FAIL;	// better err handling!
		}
		Read_Word (CComQIPtr<IXMLDOMElement> (curWord));
		CComQIPtr<IXMLDOMNode>	next;
		ThrowIfError (curWord->get_nextSibling (&next));
		curWord = next;
	}
}

void	XMLDictionaryReader::Read_URLList (CComQIPtr<IXMLDOMElement> urlList)
{
	// now iterate over all elements
	CComBSTR	nodeName;
	ThrowIfError (urlList->get_nodeName (&nodeName));
	if (nodeName != kBSTRTag_URLList) {
		throw E_FAIL;	// better err handling!
	}
	CComQIPtr<IXMLDOMNode>	curURL;
	ThrowIfError (urlList->get_firstChild (&curURL));
	while (curURL.p != NULL) {
		CComBSTR	wordTagName;
		ThrowIfError (curURL->get_nodeName (&wordTagName));
		if (wordTagName != kBSTRTag_URL) {
			throw E_FAIL;	// better err handling!
		}
		Read_URL (CComQIPtr<IXMLDOMElement> (curURL));
		CComQIPtr<IXMLDOMNode>	next;
		ThrowIfError (curURL->get_nextSibling (&next));
		curURL = next;
	}
}

void	XMLDictionaryReader::Read_Word (CComQIPtr<IXMLDOMElement> wordElt)
{
	// handle errors inside here - and just stick in default values for things - but other errors should be propagated...
	#if		0
	{
		CComBSTR	xmlText;
		ThrowIfError (wordElt->get_xml (&xmlText));
	}
	#endif
	CComBSTR	nodeText;
	ThrowIfError (wordElt->get_text (&nodeText));
	CComVariant	comDateFound;
	ThrowIfError (wordElt->getAttribute (kBSTRAttribute_DateFound, &comDateFound));
	CComVariant	comStatus;
	ThrowIfError (wordElt->getAttribute (kBSTRAttribute_Status, &comStatus));
	CComVariant	comOccuranceCount;
	ThrowIfError (wordElt->getAttribute (kBSTRAttribute_OccuranceCount, &comOccuranceCount));
	(void)comOccuranceCount.ChangeType (VT_UI4);	// ignore error here - treat absense or bad val as zero - default...

	WordInfo	wi;
	wi.fDateFound = comDateFound.bstrVal==NULL? wstring (): wstring (comDateFound.bstrVal);
	wi.fOccuranceCount = comOccuranceCount.uintVal;
	wi.fStatus = String2Status (comStatus.bstrVal == NULL? wstring (): wstring (comStatus.bstrVal));
	fWordMap.insert (WordMap::value_type ((nodeText.m_str==NULL? wstring (): wstring (nodeText.m_str)), wi));
}

void	XMLDictionaryReader::Read_URL (CComQIPtr<IXMLDOMElement> urlElt)
{
	CComBSTR	nodeText;
	ThrowIfError (urlElt->get_text (&nodeText));
	fURLList.push_back (nodeText.m_str==NULL? wstring (): nodeText.m_str);
}
#endif







#if		qXMLDictionaryFormatSupported
/*
 ********************************************************************************
 ***************************** XMLDictionaryWriter ******************************
 ********************************************************************************
 */
XMLDictionaryWriter::XMLDictionaryWriter ():
	inherited ()
{
}

void	XMLDictionaryWriter::Save (const TCHAR* fileName)
{
	CComQIPtr<IXMLDOMDocument>	pDoc;
	ThrowIfError (::CoCreateInstance (__uuidof (DOMDocument), NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDoc));
	CComQIPtr<IXMLDOMElement> topLevelElt;
	ThrowIfError (pDoc->createElement (kBSTRTag_DictionarySource, &topLevelElt));
	ThrowIfError (topLevelElt->setAttribute (kBSTRAttribute_DictionaryName, CComVariant (fDictionaryName.c_str ())));
	ThrowIfError (pDoc->putref_documentElement (topLevelElt));
	{
		//   pi = xmlDoc.createProcessingInstruction("xml", "version=\"1.0\"");
		//   xmlDoc.insertBefore(pi, xmlDoc.childNodes.item(0));
		CComPtr<IXMLDOMProcessingInstruction>	xmlHeaderTag;
		ThrowIfError (pDoc->createProcessingInstruction (CComBSTR ("xml"), CComBSTR ("version=\"1.0\""), &xmlHeaderTag));
 		CComPtr<IXMLDOMNodeList>	childNodes;
		ThrowIfError (pDoc->get_childNodes (&childNodes));
 		CComPtr<IXMLDOMNode>	item0;
		ThrowIfError (childNodes->get_item (0, &item0));
		CComPtr<IXMLDOMNode>	outNewChild;
		ThrowIfError (pDoc->insertBefore (xmlHeaderTag, CComVariant (item0), &outNewChild));
	}
	ThrowIfError (pDoc->put_preserveWhiteSpace (VARIANT_TRUE));
	DoSave (pDoc, topLevelElt);
	ThrowIfError (pDoc->save (CComVariant (fileName)));
}

void	XMLDictionaryWriter::DoSave (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt)
{
	Save_WordList (doc, dictionarySourceElt);
	Save_URLList (doc, dictionarySourceElt);
	AppendWhitespace (doc, dictionarySourceElt, "\r\n");
}

void	XMLDictionaryWriter::Save_WordList (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt)
{
	CComQIPtr<IXMLDOMElement> wordList;
	AppendWhitespace (doc, dictionarySourceElt, "\r\n\t");
	ThrowIfError (doc->createElement (kBSTRTag_WordList, &wordList));
	ThrowIfError (dictionarySourceElt->appendChild (wordList, NULL));
	AppendWhitespace (doc, wordList, "\r\n\t");
	for (WordMap::const_iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
		Save_Word (doc, wordList, i);
	}
}

void	XMLDictionaryWriter::Save_Word (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> parentElt, WordMap::const_iterator it)
{
	CComQIPtr<IXMLDOMElement> wordElt;
	AppendWhitespace (doc, parentElt, "\t");
	ThrowIfError (doc->createElement (kBSTRTag_Word, &wordElt));
	ThrowIfError (wordElt->put_text (CComBSTR (it->first.c_str ())));
	ThrowIfError (wordElt->setAttribute (kBSTRAttribute_DateFound, CComVariant (it->second.fDateFound.c_str ())));
	//ThrowIfError (wordElt->setAttribute (kBSTRAttribute_Status, CComVariant (it->second.fStatus==WordInfo::eDisabled? "disabled": "enabled")));
	ThrowIfError (wordElt->setAttribute (kBSTRAttribute_Status, CComVariant (Status2String (it->second.fStatus).c_str ())));
	ThrowIfError (wordElt->setAttribute (kBSTRAttribute_OccuranceCount, CComVariant (static_cast<unsigned int> (it->second.fOccuranceCount))));
	ThrowIfError (parentElt->appendChild (wordElt, NULL));
	AppendWhitespace (doc, parentElt, "\r\n\t");
}

void	XMLDictionaryWriter::Save_URLList (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt)
{
	CComQIPtr<IXMLDOMElement> urlList;
	AppendWhitespace (doc, dictionarySourceElt, "\r\n\t");
	ThrowIfError (doc->createElement (kBSTRTag_URLList, &urlList));
	ThrowIfError (dictionarySourceElt->appendChild (urlList, NULL));
	AppendWhitespace (doc, urlList, "\r\n\t");
	for (URLList::const_iterator i = fURLList.begin (); i != fURLList.end (); ++i) {
		Save_URL (doc, urlList, i);
	}
}

void	XMLDictionaryWriter::Save_URL (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> parentElt, URLList::const_iterator it)
{
	CComQIPtr<IXMLDOMElement> urlElt;
	AppendWhitespace (doc, parentElt, "\t");
	ThrowIfError (doc->createElement (kBSTRTag_URL, &urlElt));
	ThrowIfError (urlElt->put_text (CComBSTR (it->c_str ())));
	ThrowIfError (parentElt->appendChild (urlElt, NULL));
	AppendWhitespace (doc, parentElt, "\r\n\t");
}
#endif


#if		qLedUsesNamespaces
	}
}
#endif








// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

