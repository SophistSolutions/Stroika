
#error "Obsolete"

/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DirectoryTreeTranslater__
#define	__DirectoryTreeTranslater__

#include	"File.hh"

class DirectoryTreeTranslater {
	public:
		DirectoryTreeTranslater (Boolean include, Boolean symbols,
								 IncludeTranslater& theIncludeTranslater,
								 GlobalSymbolTranslater& theGlobalSymbolTranslater,
								 Mapping(String,String)& theIncludeDictionary,
								 PathName& temporary) :
			fMapIncludes (include),
			fMapGlobalSymbols (symbols),
			fIncludeTranslater (theIncludeTranslater),
			fGlobalSymbolTranslater (theGlobalSymbolTranslater),
			fIncludeDictionary (theIncludeDictionary),
			fTemporaryFile (temporary)
		{
		}

		nonvirtual void DoIt (const PathName& fromDirectory, const PathName& toDirectory);

	private:
		Boolean					fMapIncludes;
		Boolean 				fMapGlobalSymbols;

		IncludeTranslater		fIncludeTranslater;
		GlobalSymbolTranslater	fGlobalSymbolTranslater;

		Mapping(String,String)&	fIncludeDictionary;
		PathName				fTemporaryFile;
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__DirectoryTreeTranslater__*/
