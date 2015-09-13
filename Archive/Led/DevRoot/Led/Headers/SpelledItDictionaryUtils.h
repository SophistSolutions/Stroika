/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SpelledItDictionaryUtils_h__
#define	__SpelledItDictionaryUtils_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SpelledItDictionaryUtils.h,v 2.2 2003/06/19 23:20:12 lewis Exp $
 */

/*
@MODULE:	SpelledItDictionaryUtils
@DESCRIPTION:
		<p>This module is designed to provide support for building and maintaining the SpelledIt
	dictionaries.</p>
 */


/*
 * Changes:
 *	$Log: SpelledItDictionaryUtils.h,v $
 *	Revision 2.2  2003/06/19 23:20:12  lewis
 *	for SPR#1537- added eMergedOut status, and be more careful about error conditions reading XML files without all attributes specified
 *	
 *	Revision 2.1  2003/06/17 23:56:50  lewis
 *	SPR#1536: added SpelledItDictionaryUtils utilities to read/write XML-format dict word files
 *	
 *	
 *	
 *
 *
 *
 */

#include	<map>
#include	<string>

#include	"LedSupport.h"



#ifndef	qXMLDictionaryFormatSupported
#define	qXMLDictionaryFormatSupported	qWindows
#endif


#if		qXMLDictionaryFormatSupported
	#include	<atlbase.h>
#endif


#if		qLedUsesNamespaces
namespace	Led {
	namespace	SpelledIt {
#endif



struct	WordInfo {
	WordInfo ():
		fDateFound (),
		fStatus (eEnabled),
		fOccuranceCount (0)
		{
		}
	wstring	fDateFound;
	enum Status { eEnabled, eDisabled, eMergedOut };	// could add others...
	Status	fStatus;
	size_t	fOccuranceCount;
};


class	DictionaryIO {
	public:
		typedef	map<wstring,WordInfo>	WordMap;

		wstring	fDictionaryName;
		WordMap	fWordMap;

		typedef	vector<wstring>	URLList;
		URLList	fURLList;

	public:
		static	const	wchar_t	kTag_DictionarySource[];
		static	const	wchar_t	kTag_WordList[];
		static	const	wchar_t	kTag_URLList[];
		static	const	wchar_t	kTag_Word[];
		static	const	wchar_t	kTag_URL[];
		static	const	wchar_t	kAttribute_DictionaryName[];
		static	const	wchar_t	kAttribute_DateFound[];
		static	const	wchar_t	kAttribute_Status[];
		static	const	wchar_t	kAttribute_OccuranceCount[];
#if		qWindows
	public:
		static	const	CComBSTR	kBSTRTag_DictionarySource;
		static	const	CComBSTR	kBSTRTag_WordList;
		static	const	CComBSTR	kBSTRTag_URLList;
		static	const	CComBSTR	kBSTRTag_Word;
		static	const	CComBSTR	kBSTRTag_URL;
		static	const	CComBSTR	kBSTRAttribute_DictionaryName;
		static	const	CComBSTR	kBSTRAttribute_DateFound;
		static	const	CComBSTR	kBSTRAttribute_Status;
		static	const	CComBSTR	kBSTRAttribute_OccuranceCount;
#endif
};

#if		qXMLDictionaryFormatSupported
	class	XMLDictionaryReader : public DictionaryIO {
		private:
			typedef	DictionaryIO	inherited;
		public:
			XMLDictionaryReader ();

		public:
			nonvirtual	void	Load (const TCHAR* fileName);

		private:
			nonvirtual	void	DoRead (CComQIPtr<IXMLDOMElement> topLevelElt);
			nonvirtual	void	Read_WordList (CComQIPtr<IXMLDOMElement> wordList);
			nonvirtual	void	Read_URLList (CComQIPtr<IXMLDOMElement> urlList);
			nonvirtual	void	Read_Word (CComQIPtr<IXMLDOMElement> wordElt);
			nonvirtual	void	Read_URL (CComQIPtr<IXMLDOMElement> urlElt);
	};
	class	XMLDictionaryWriter : public DictionaryIO {
		private:
			typedef	DictionaryIO	inherited;
		public:
			XMLDictionaryWriter ();

		public:
			nonvirtual	void	Save (const TCHAR* fileName);

		private:
			nonvirtual	void	DoSave (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt);
			nonvirtual	void	Save_WordList (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt);
			nonvirtual	void	Save_Word (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> parentElt, WordMap::const_iterator it);
			nonvirtual	void	Save_URLList (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> dictionarySourceElt);
			nonvirtual	void	Save_URL (CComQIPtr<IXMLDOMDocument> doc, CComQIPtr<IXMLDOMElement> parentElt, URLList::const_iterator it);
	};
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */




#if		qLedUsesNamespaces
	}
}
#endif


#endif	/*__CodePage_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

