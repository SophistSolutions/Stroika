/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/SpelledIt/MungeDictionaries/MungeDictionaries.cpp,v 1.16 2003/12/11 15:18:09 lewis Exp $
 *
 * Changes:
 *	$Log: MungeDictionaries.cpp,v $
 *	Revision 1.16  2003/12/11 15:18:09  lewis
 *	fix -fh filtering to look for lower-case version of words for segWords
 *	
 *	Revision 1.15  2003/12/11 13:57:56  lewis
 *	emit cast around single-byte chars that are bigger than 128 so we don't get needless warning
 *	
 *	Revision 1.14  2003/12/11 02:26:37  lewis
 *	SPR#1588: Updated MungeDictionaries utilities with -fh flag to filter hypenated words
 *	(to trim unneeded entries from dict).
 *	
 *	Revision 1.13  2003/12/10 18:26:13  lewis
 *	hopefully fix so we don't get overflow compiler warnigns on single-byte dictioanries
 *	
 *	Revision 1.12  2003/12/10 00:51:44  lewis
 *	major reqork of compilation code - template based - so we can keep lists sorted in
 *	different order (for ansi vs. unicode) - to fix SPR#1584
 *	
 *	Revision 1.11  2003/12/09 21:00:26  lewis
 *	speed tweeks and cleanup verbose tracking messages
 *	
 *	Revision 1.10  2003/12/09 14:22:13  lewis
 *	fix PreFillWordFragTextMap () to fill in longer (25) words
 *	
 *	Revision 1.9  2003/12/09 02:08:08  lewis
 *	added debug checks for very long compiles
 *	
 *	Revision 1.8  2003/12/08 20:14:05  lewis
 *	SPR#1584: generate idefs in compiled dict output for singlebyte vs unicode. Also - did SPR#1541: code
 *	to compress char table (overlay common characters). VERY SLOW compilation. So - not really effectively
 *	tested yet.
 *	
 *	Revision 1.7  2003/06/27 13:31:01  lewis
 *	add option to EXPUNGE all disabled/munged out entries. We may NOT always want to
 *	carry these around in our dictionaries
 *	
 *	Revision 1.6  2003/06/20 16:10:27  lewis
 *	SPR#1538: get basic dictionary compilation working - producing initialzed C structs
 *	
 *	Revision 1.5  2003/06/19 23:20:58  lewis
 *	SPR#1537: added -d[e,d,m] flag to dump wordlists.
 *	
 *	Revision 1.4  2003/06/19 20:37:11  lewis
 *	SPR#1537: fixed code to merge contents of two (or more) passed in dictionaries
 *	
 *	Revision 1.3  2003/06/19 19:54:45  lewis
 *	SPR#1537: use new mergedout status instead of disabled for when we merge items out
 *	
 *	Revision 1.2  2003/06/19 19:25:00  lewis
 *	cleanup - support -- options - and cleanup Usage print text
 *	
 *	Revision 1.1  2003/06/19 19:20:08  lewis
 *	SPR#1537: first cut at MungeDictionaries utility. It does basic loading of 1 dict, and
 *	does the -m function - to merge entries, and also -s stats dump. More todo...
 *	
 *	
 *
 *
 *
 */
#include	<map>
#include	<vector>
#include	<string>
#include	<iostream>
#include	<fstream>
#include	<tchar.h>
#define		_ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include	<atlbase.h>

#include	"CodePage.h"
#include	"LedSupport.h"
#include	"SpellCheckEngine_Basic.h"
#include	"SpelledItDictionaryUtils.h"
#include	"TextBreaks.h"


#if		qLedUsesNamespaces
	using namespace	Led;
	using namespace	Led::SpelledIt;
#endif


namespace	{
	inline	bool	AsymetricCaseInsensativeCompare (Led_tChar wordChar, Led_tChar dictChar)
		{
			if (wordChar == dictChar) {
				return true;
			}
			if (isascii (wordChar) and isupper (wordChar)) {
				return tolower (wordChar) == dictChar;
			}
			return false;
		}
	inline	bool	AsymetricCaseInsensativeCompare (const Led_tChar* word, const Led_tChar* dictWord)
		{
			const Led_tChar*	wi	=	word;
			const Led_tChar*	di	=	dictWord;
			for (; ; ++wi, ++di) {
				if (not AsymetricCaseInsensativeCompare (*wi, *di)) {
					return false;
				}
				if (*wi == '\0' and *di == '\0') {
					return true;
				}
				if (*wi == '\0' or *di == '\0') {
					return false;
				}
			}
		}
	inline	wstring	SafeCSymbol (const wstring& s)
		{
			wstring	r	=	s;
			for (size_t i = 0; i < r.length (); ++i) {
				if (r[i] == ' ' or r[i] == '-') {
					r[i] = '_';
				}
			}
			return r;
		}
}



namespace	{

	string	cvt2ANSI (string s)		{ return s; }
	string	cvt2ANSI (wstring s)	{ return Wide2ACPString (s); }


	template <class CHAR>
		struct	WordFragTextMap {
			private:
				vector<CHAR>	fBLOB;

			public:
				WordFragTextMap ():
					fBLOB ()
					{
					}

			public:
				static	const	size_t	kBadIdx	=	static_cast<size_t> (-1);


			public:
				nonvirtual	void	AccessBLOB (typename vector<CHAR>::const_iterator* start, typename vector<CHAR>::const_iterator* end)
					{
						*start = fBLOB.begin ();
						*end = fBLOB.end ();
					}

			public:
				nonvirtual	basic_string<CHAR>	GetStr (size_t start, size_t end)
					{
						Led_Assert (start < end);
						return basic_string<CHAR> (fBLOB.begin () + start, fBLOB.begin () + end);
					}

			public:
				// return kBadIdx if not found
				nonvirtual	size_t	Lookup (const CHAR* t, size_t len)
					{
						vector<CHAR>::const_iterator	start	=	fBLOB.begin ();
						vector<CHAR>::const_iterator	end		=	fBLOB.end ();

						// Make sure we don't compare past end of array with memcmp ()
						if (end - start > len) {
							end = end - len + 1;
						}
						else if (end - start == len) {
							end = end - len;
						}

						for (typename vector<CHAR>::const_iterator i = start; i != end; ++i) {
							if (::memcmp (&*i, t, len * sizeof (CHAR)) == 0) {
								return i - start;
							}
						}
						return kBadIdx;
					}
			public:
				nonvirtual	size_t	AppendTextFrag (const CHAR* t, size_t len)
					{
						size_t	oldLen	=	fBLOB.size ();
						size_t	newLen	=	oldLen + len;
						if (newLen > fBLOB.capacity ()) {
							fBLOB.reserve (max (fBLOB.capacity () * 2, newLen));
						}
						fBLOB.resize (newLen);
						copy (t, t + len, &fBLOB[oldLen]);
						return oldLen;
					}
		};
}



class	MyApp {
	public:
		MyApp ():
			fFilesToRead (),
			fOutputFile (),
			fCompileOut (),
			fFilterHyphenated (false),
			fDisplayedUsage (false),
			fVerbose (false),
			fDumpEnabled (),
			fDumpMerged (),
			fDumpDisabled (),
			fDumpStats (false),
			fExpunge (false),
			fMergeOutItems (false),
			fDictsRead (0)
			{
				(void)::CoInitialize (NULL);
			}

		~MyApp ()
			{
				(void)::CoUninitialize ();
			}

	public:
		nonvirtual	void	Run (int argc, _TCHAR* argv[])
			{
				ParseArgs (argc, argv);
				if (fDisplayedUsage) {
					return;
				}
				ReadFiles ();
				if (fDumpStats) {
					DumpStats (fMergeOutItems? " before merge": "");
				}
				if (fMergeOutItems) {
					MergeOutItems ();
					if (fDumpStats) {
						DumpStats (" after merge");
					}
				}
				if (fFilterHyphenated) {
					FilterHyphenated ();
					if (fDumpStats) {
						DumpStats (" after FilterHypenated");
					}
				}
				if (not fDumpDisabled.empty ()) {
					OutputDumpedTextFile (WordInfo::eDisabled, fDumpDisabled);
				}
				if (not fDumpEnabled.empty ()) {
					OutputDumpedTextFile (WordInfo::eEnabled, fDumpEnabled);
				}
				if (not fDumpMerged.empty ()) {
					OutputDumpedTextFile (WordInfo::eMergedOut, fDumpMerged);
				}
				if (fExpunge) {
					Expunge ();
				}
				if (not fCompileOut.empty ()) {
					OutputCompiledDictionary ();
				}
				if (not fOutputFile.empty ()) {
					OutputDictionary ();
				}
			}

	private:
		nonvirtual	void	Usage ()
			{
				cerr << "Usage: MungeDictionaries [options]? [srcDictList]*\n";
				cerr << "\twhere [options]can be:\n";
				cerr << "\t\t-help to print out this message\n";
				cerr << "\t\t-c [FILENAME] to produce a compiled (C++ file) form of the loaded dictionary(s).\n";
				cerr << "\t\t-d[e,d,n] [FILENAME] to to dump word list of enabled/disabled/merged words to FILENAME\n";
				cerr << "\t\t-e to expunge merged-out or disabled items\n";
				cerr << "\t\t-fh to filter out (mark as deleted) all words which could be found by hypenated lookup rules\n";
				cerr << "\t\t-m merge out redundant words (like capitalized versions of a word appearing in lower case)\n";
				cerr << "\t\t-o <FILENAME> dump the resulting dictionary data to the given file\n";
				cerr << "\t\t-stats to dump out dictionary statistics\n";
				cerr << "\t\t-verbose to print extra info\n";
				fDisplayedUsage = true;
			}

	private:
		nonvirtual	void	ParseArgs (int argc, _TCHAR* argv[])
			{
				for (int i = 1; i < argc; ++i) {
					const _TCHAR*	argi	=	argv[i];
					if (argi != NULL) {
						if (argi[0] == '-' or argi[0] == '/') {
							size_t	optCharIdx	=	1;
							_TCHAR	optChar		=	argi[optCharIdx];
							if (optChar == '-' and argi[0] == '-') {
								optCharIdx++;
								optChar = argi[optCharIdx];
							}
							switch (optChar) {
								case	'd': {
									switch (argi[optCharIdx+1]) {
										case	'd':
										case	'e':
										case	'm': {
											++i;
											if (i < argc) {
												switch (argi[optCharIdx+1]) {
													case	'd':	fDumpDisabled = argv[i]; break;
													case	'e':	fDumpEnabled = argv[i]; break;
													case	'm':	fDumpMerged = argv[i]; break;
												}
											}
											else {
												Usage ();
												return;
											}
										}
										break;
										default: {
											Usage ();
											return;
										}
										break;
									}
								}
								break;
								case	'f': {
									switch (argi[optCharIdx+1]) {
										case	'h': {
											fFilterHyphenated = true;
										}
										break;
										default: {
											Usage ();
											return;
										}
										break;
									}
								}
								break;
								case	'h':
								case	'?': {
									Usage ();
								}
								break;
								case	'c': {
									++i;
									if (i < argc) {
										argi = argv[i];
										fCompileOut = argi;
									}
									else {
										Usage ();
										return;
									}
								}
								break;
								case	'e': {
									fExpunge = true;
								}
								break;
								case	'm': {
									fMergeOutItems = true;
								}
								break;
								case	'o': {
									++i;
									if (i < argc) {
										argi = argv[i];
										fOutputFile = argi;
									}
									else {
										Usage ();
										return;
									}
								}
								break;
								case	's': {
									fDumpStats = true;
								}
								break;
								case	'v': {
									fVerbose = true;
								}
								break;
								default: {
									cerr << "Unknown argument '" << argi << "'\n";
									Usage ();
								}
								break;
							}
						}
						else {
							fFilesToRead.push_back (argi);
						}
					}
				}
			}

	private:
		nonvirtual	void	ReadFiles ()
			{
				for (vector<wstring>::const_iterator i = fFilesToRead.begin (); i != fFilesToRead.end (); ++i) {
					ReadFile (*i);
				}
			}
		nonvirtual	void	ReadFile (const wstring& fileName)
			{
				if (fVerbose) {
					cerr << "Progress: Reading file: '" << Wide2ACPString (fileName) << "'...";
				}
				try {
					XMLDictionaryReader	reader;
					reader.Load (fileName.c_str ());
					if (fDictsRead == 0) {
						fWordMap = reader.fWordMap;
						fURLList = reader.fURLList;
						fDictionaryName = reader.fDictionaryName;
					}
					else {
						// merge word lists
						for (DictionaryIO::WordMap::const_iterator i = reader.fWordMap.begin (); i != reader.fWordMap.end (); ++i) {
							const wstring& w	=	(*i).first;
							DictionaryIO::WordMap::iterator i2 = fWordMap.find (w);
							if (i2 == fWordMap.end ()) {
								fWordMap.insert (*i);
							}
							else {
								// just update count on word in map and presevere STATUS from first occurance
								i2->second.fOccuranceCount += i->second.fOccuranceCount;
								if (fVerbose and i2->second.fStatus != i->second.fStatus) {
									cerr << "\r\nWarning: word '" << Wide2ACPString (w) << "' has a status mismatch (keeping first).\r\n";
								}
							}
						}
						// Merge URL lists
						for (DictionaryIO::URLList::const_iterator i = reader.fURLList.begin (); i != reader.fURLList.end (); ++i) {
							const wstring& urlToAdd	=	(*i);
							DictionaryIO::URLList::iterator i2 = std::find (fURLList.begin (), fURLList.end (), urlToAdd);
							if (i2 == fURLList.end ()) {
								fURLList.push_back (urlToAdd);
							}
						}
						// merge dictionary names
						if (fDictionaryName != reader.fDictionaryName) {
							cerr << "Warning: dictionaries with different names - using the first encountered\n";
						}
					}
					fDictsRead++;
				}
				catch (...) {
					cerr << "\n\tError reading file: '" << Wide2ACPString (fileName) << "'\n";
					return;
				}
				if (fVerbose) {
					cerr << " done\n";
				}
			}

	private:
		nonvirtual	void	DumpStats (const char* extraStuff = NULL)
			{

				cerr << "Stats";
				if (extraStuff!=NULL) {
					cerr << extraStuff;
				}
				cerr << ":\n";
				cerr << "\tWord Count:\t" << fWordMap.size () << "\n";
				{
					unsigned int	enabledItems	=	0;
					unsigned int	disabledItems	=	0;
					unsigned int	mergedItems		=	0;
					unsigned int	totalChars		=	0;
					for (DictionaryIO::WordMap::const_iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
						if ((*i).second.fStatus == WordInfo::eDisabled) {
							disabledItems++;
						}
						if ((*i).second.fStatus == WordInfo::eEnabled) {
							enabledItems++;
						}
						if ((*i).second.fStatus == WordInfo::eMergedOut) {
							mergedItems++;
						}
						if ((*i).second.fStatus == WordInfo::eEnabled) {
							totalChars += (*i).first.length ();
						}
					}
					cerr << "\t\tEnabled:\t" << enabledItems << "\t\tDisabled:\t" << disabledItems << "\t\tMerged-out:\t" << mergedItems << "\n";
					cerr << "\ttotalChars (enabled):\t" << totalChars << "\n";
				}
				cerr << "\tURL Count:\t" << fURLList.size () << "\n";
			}

	private:
		template	<typename	CHAR>
			nonvirtual	void	PreFillWordFragTextMap (const map<basic_string<CHAR>,WordInfo>& wordMap, WordFragTextMap<CHAR>* wftm)
				{
					if (fVerbose) {
						cerr << "\n\tStarting PreFillWordFragTextMap (slow)";
					}
					// Pre-fill the map with longer words first (cuz shorter ones more likely to be found as substrings of
					// longer ones
					const	size_t	kBigStart	=	25;
					for (size_t	wordLen = kBigStart; wordLen >= 1; --wordLen) {
						if (fVerbose) {
							cerr << ".";
						}
						for (map<basic_string<CHAR>,WordInfo>::const_iterator i = wordMap.begin (); i != wordMap.end (); ++i) {
							if (((*i).second.fStatus == WordInfo::eEnabled) and 
								((wordLen == kBigStart and (*i).first.length () >= wordLen) or
								((*i).first.length () == wordLen)
								)
								) {
								const basic_string<CHAR>&	x	=	(*i).first;
								size_t	atOffset	=	wftm->Lookup (x.c_str (), x.length ());
								if (atOffset == WordFragTextMap<CHAR>::kBadIdx) {
									wftm->AppendTextFrag (x.c_str (), x.length ());
								}
							}
						}
					}
					if (fVerbose) {
						cerr << "done\n";
					}
				}

	private:
		template	<typename	CHAR>
			nonvirtual	void	DoCompOut (const map<basic_string<CHAR>,WordInfo>& wordMap, const string& dataName, const string& idxName, fstream& out)
				{
					typedef	map<basic_string<CHAR>,WordInfo>	WM;
					typedef	basic_string<CHAR>					STR;

					WordFragTextMap<CHAR>	wftm;
					PreFillWordFragTextMap (wordMap, &wftm);

					// Write Dictionary Data (text)
					out << "static	Led_tChar	" << dataName << "[] = {\n";
					out << hex;

					{
						vector<CHAR>::const_iterator	start;
						vector<CHAR>::const_iterator	end;
						wftm.AccessBLOB (&start, &end);
						size_t	offsetSoFar	=	0;
						const size_t	kWidth	=	16;
						for (vector<CHAR>::const_iterator i = start; i != end; ++i) {
							if (offsetSoFar % kWidth == 0) {
								out << "\t";
							}
							CHAR	prettyPrintCC	=	*i;
							if (('a' <= prettyPrintCC and prettyPrintCC <= 'z') or
								('A' <= prettyPrintCC and prettyPrintCC <= 'Z') or
								('0' <= prettyPrintCC and prettyPrintCC <= '9') or
								('-' == prettyPrintCC)
								) {
								out << "\'" << static_cast<char> (prettyPrintCC) << "\', ";
							}
							else {
								if (sizeof (CHAR) == 1) {
									unsigned char	uc	=	static_cast<unsigned char> (*i);
									if (uc >= 128) {
										out << "(Led_tChar)0x" << static_cast<int> (uc) << ", ";
									}
									else {
										out << "0x" << static_cast<int> (uc) << ", ";
									}
								}
								else {
									out << "0x" << static_cast<unsigned int> (*i) << ", ";
								}
							}
							offsetSoFar++;
							if (offsetSoFar % kWidth == 0) {
								out << "\n";
							}
						}
						out << "\n";
						if (fDumpStats) {
							cerr << "\tCompiled shared " << (sizeof(CHAR)==sizeof(wchar_t)?"UNICODE": "single-byte") << " char table size: " << dec << end-start << "\n";
						}
					}
					out << "};\n";

					typedef	SpellCheckEngine_Basic::InfoBlock InfoBlock;
					vector<InfoBlock>	ibs;
					ibs.reserve (wordMap.size ());
					if (fVerbose) {
						cerr << "\tStarting look to construct IBS vectors (long)";
					}
					int loopCnt = 0;
					for (WM::const_iterator i = wordMap.begin (); i != wordMap.end (); ++i) {
						if ((*i).second.fStatus == WordInfo::eEnabled) {
							InfoBlock	iB;
							(void)::memset (&iB, 0, sizeof (iB));
							++loopCnt;

							size_t		atOffset	 = wftm.Lookup ((*i).first.c_str (), (*i).first.length ());
							iB.fIndex = atOffset;
							if (iB.fIndex != atOffset) {
								cerr << "Error: overflow on word '" << cvt2ANSI ((*i).first) << "' with offset =" << atOffset <<  "\n";
							}
							size_t	wordLen	=	(*i).first.length ();
							iB.fWordLen = wordLen;
							if (iB.fWordLen != wordLen) {
								cerr << "Error: overflow(2) on word '" << cvt2ANSI ((*i).first) << "' with offset =" << atOffset <<  "\n";
							}
							iB.fXXX = 0;//tmphack
							Led_Assert (iB.fWordLen > 0);
							ibs.push_back (iB);

							if (fVerbose and (loopCnt % 100 == 0)) {
								cerr << ".";
							}
						}
					}

					if (fVerbose) {
						cerr << "done\n";
					}

					// Write Dictionary Index
					out << "static	SpellCheckEngine_Basic::InfoBlock	" << idxName << "[0x" << ibs.size () << "] = {\n";
					out << hex;
						for (vector<InfoBlock>::const_iterator i = ibs.begin (); i != ibs.end (); ++i) {
							string	printName	=	cvt2ANSI (STR (wftm.GetStr ((*i).fIndex, (*i).fIndex + (*i).fWordLen)));
							out << "\t{\t0x" << (*i).fIndex << ", 0x" << (*i).fWordLen << ", 0x" << (*i).fXXX << "\t},\t\t\t//\t'" << printName << "'\n";
						}
					out << "};\n";
				}
		nonvirtual	void	OutputCompiledDictionary ()
			{
				if (fVerbose) {
					cerr << "Progress: Exporting compiled dictionary to: '" << Wide2ACPString (fCompileOut) << "'...";
				}
				try {
					fstream	out;
					out.open (Wide2ACPString (fCompileOut).c_str (), ios_base::out);

					string	dictObjectName	=	"Dictionary_" + Wide2ACPString (SafeCSymbol (fDictionaryName));
					string	dataName		=	dictObjectName + "_Data";
					string	idxName			=	dictObjectName + "_Index";

					out << "#if		qWideCharacters\n";
						DoCompOut (fWordMap, dataName, idxName, out);
					out << "#else\n";
						{
							map<string,WordInfo>	ansiWM;
							for (DictionaryIO::WordMap::iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
								ansiWM.insert (map<string, WordInfo>::value_type (Wide2ACPString ((*i).first), (*i).second));
							}
							DoCompOut (ansiWM, dataName, idxName, out);
						}
					out << "#endif\n";

					// Write Dictionary object (pulling idx and data together)
					out << "static	SpellCheckEngine_Basic::CompiledDictionary::CompiledDictionaryData	" << dictObjectName << " = {\n";
					out << "\t" << dataName << ", " << dataName << " + Led_NEltsOf (" << dataName << "),\n";
					out << "\t" << idxName << ", " << idxName << " + Led_NEltsOf (" << idxName << "),\n";
					out << "};\n\n";
				}
				catch (...) {
					cerr << "\n\tError writing file: '" << Wide2ACPString (fCompileOut) << "'\n";
					return;
				}
				if (fVerbose) {
					cerr << " done\n";
				}
			}

	private:
		nonvirtual	void	OutputDictionary ()
			{
				if (fVerbose) {
					cerr << "Progress: Saving output file: '" << Wide2ACPString (fOutputFile) << "'...";
				}
				try {
					XMLDictionaryWriter	writer;
					writer.fWordMap = fWordMap;
					writer.fURLList = fURLList;
					writer.fDictionaryName = fDictionaryName;
					writer.Save (fOutputFile.c_str ());
				}
				catch (...) {
					cerr << "\n\tError writing file: '" << Wide2ACPString (fOutputFile) << "'\n";
					return;
				}
				if (fVerbose) {
					cerr << " done\n";
				}
			}

	private:
		nonvirtual	void	Expunge ()
			{
				if (fVerbose) {
					cerr << "Progress: starting expunge'...";
				}
				DictionaryIO::WordMap	tmpWordMap;
				for (DictionaryIO::WordMap::iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
					if ((*i).second.fStatus == WordInfo::eEnabled) {
						tmpWordMap.insert (*i);
					}
				}
				if (fVerbose) {
					cerr << "...\n";
				}
				fWordMap = tmpWordMap;
				if (fVerbose) {
					cerr << "done\n";
				}
			}

	private:
		nonvirtual	void	OutputDumpedTextFile (WordInfo::Status s, const wstring& outputFile)
			{
				if (fVerbose) {
					cerr << "Progress: Exporting wordlist to: '" << Wide2ACPString (outputFile) << "'...";
				}
				try {
					fstream	out;
					out.open (Wide2ACPString (outputFile).c_str (), ios_base::out);
					for (DictionaryIO::WordMap::iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
						if ((*i).second.fStatus == s) {
							out << Wide2ACPString ((*i).first) << "\n";
						}
					}
				}
				catch (...) {
					cerr << "\n\tError writing file: '" << Wide2ACPString (outputFile) << "'\n";
					return;
				}
				if (fVerbose) {
					cerr << " done\n";
				}
			}

	private:
		bool	LookupWordNE (const Led_tString& checkWord, DictionaryIO::WordMap::iterator* matchedWordResult)
			{
				Led_tString	w2	=	checkWord;
				w2[0] = tolower (w2[0]);
				DictionaryIO::WordMap::iterator	i	=	fWordMap.find (w2);
				if (i != fWordMap.end ()) {
					if (matchedWordResult != NULL) {
						*matchedWordResult = i;
					}
					return true;
				}
				return false;
			}	

	private:
		nonvirtual	void	MergeOutItems ()
			{
				if (fVerbose) {
					cerr << "Progress: Doing MergeOutItems...";
				}
				{
// There are lots of checks we COULD do here (like "word-word") - but for now - just throw away
// words which could be found by 
					for (DictionaryIO::WordMap::iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
						if ((*i).second.fStatus != WordInfo::eDisabled) {
							const wstring& w	=	(*i).first;
							if (w.length () == 0) {
								(*i).second.fStatus = WordInfo::eDisabled;
							}
							else {
								if (isascii (w[0]) and isupper (w[0])) {
									// See if we have a word which is all lower case and matches...
									DictionaryIO::WordMap::iterator	matchedWordResult	=	fWordMap.end ();
									if (LookupWordNE (w, &matchedWordResult)) {
										(*i).second.fStatus = WordInfo::eMergedOut;
										Led_AssertNotNil (matchedWordResult);
										(*matchedWordResult).second.fOccuranceCount += (*i).second.fOccuranceCount;
										(*i).second.fOccuranceCount = 0;
									}
								}
							}
						}
					}
				}
				if (fVerbose) {
					cerr << "done.\n";
				}
			}

	private:
		nonvirtual	void	FilterHyphenated ()
			{
				if (fVerbose) {
					cerr << "Progress: Doing FilterHyphenated...";
				}
				{
// There are lots of checks we COULD do here (like "word-word") - but for now - just throw away
// words which could be found by 
					for (DictionaryIO::WordMap::iterator i = fWordMap.begin (); i != fWordMap.end (); ++i) {
						if ((*i).second.fStatus != WordInfo::eDisabled) {
							const wstring& w	=	(*i).first;
							bool	hadHypen			=	false;
							bool	wordSBStripped		=	false;
							size_t	lastFoundAt			=	0;
							bool	mustCheckLastSeg	=	false;
							for (size_t j = w.find ('-'); j != Led_tString::npos or mustCheckLastSeg; j = w.find ('-', j + 1)) {
								hadHypen = true;
								if (j == Led_tString::npos) {
									Led_Assert (mustCheckLastSeg);
									mustCheckLastSeg = false;
									j = w.length ();
								}
								else {
									mustCheckLastSeg = true;	// if we find a dash - we must check afer it as well
								}
								Led_tString	segWord		=	Led_tString (w.substr (lastFoundAt, j-lastFoundAt));
								Led_tString	segLCWord	=	segWord;
								if (segLCWord[0] < 128 and isupper (segLCWord[0])) {
									segLCWord[0] = tolower (segLCWord[0]);
								}
								if (fWordMap.find (segWord) != fWordMap.end ()) {
									wordSBStripped = true;
								}
								else if (fWordMap.find (segLCWord) != fWordMap.end ()) {
									wordSBStripped = true;
								}
								else if (j != w.length () and fWordMap.find (segWord + LED_TCHAR_OF ("-")) != fWordMap.end ()) {
									// OK
									wordSBStripped = true;
								}
								else {
									// We need this word. Dont' check anymore (other segments)
									wordSBStripped = false;
									break;
								}
								lastFoundAt = j + 1;
							}

							if (hadHypen and fVerbose) {
								cerr << "\tWord '" << Wide2ACPString (w) << "'";
								if (wordSBStripped) {
									cerr << " is STRIPPED (disabled) because it can be found indirectly\n";
								}
								else {
									cerr << " is PRESERVED because it cannot be found indirectly\n";
								}
							}
							if (wordSBStripped) {
								(*i).second.fStatus = WordInfo::eDisabled;
							}
						}
					}
				}
				if (fVerbose) {
					cerr << "done.\n";
				}
			}

	private:
		vector<wstring>			fFilesToRead;
		wstring					fOutputFile;
		wstring					fCompileOut;
		bool					fFilterHyphenated;
		bool					fDisplayedUsage;
		bool					fVerbose;
		wstring					fDumpEnabled;
		wstring					fDumpMerged;
		wstring					fDumpDisabled;
		bool					fDumpStats;
		bool					fExpunge;
		bool					fMergeOutItems;
		unsigned int			fDictsRead;
		wstring					fDictionaryName;
		DictionaryIO::WordMap	fWordMap;
		DictionaryIO::URLList	fURLList;
};

int		_tmain (int argc, _TCHAR* argv[])
{
	MyApp	app;
	app.Run (argc, argv);
	return 0;
}

