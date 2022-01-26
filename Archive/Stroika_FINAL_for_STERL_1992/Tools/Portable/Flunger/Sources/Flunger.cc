/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Flunger.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Flunger.cc,v $
// Revision 1.1  1992/09/08  18:24:32  lewis
// Initial revision
//
 *
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/Flunger.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $";
#endif

#include	<fstream.h>
#include	<strstream.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<CursorCtl.h>
#elif		qUnixOS
#include	<unistd.h>
#endif
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"File.hh"
#include	"Mapping_HashTable.hh"
#include	"StreamUtils.hh"

#include	"MPWObj.hh"

#include	"Flunger.hh"





#if		!qRealTemplatesAvailable
AbMappingDeclare (UInt16, String);
Mapping_HTDeclare(UInt16,String);

AbMappingImplement (UInt16, String);
Mapping_HTImplement1(UInt16,String);
Mapping_HTImplement2(UInt16,String);
Mapping_HTImplement3(UInt16,String);
#endif	/*!qRealTemplatesAvailable*/



class	MyMPWObjectFile : public MPWObjectFile {
	public:
		MyMPWObjectFile ();

		nonvirtual	AbSequence(MPWObjRecord)&	GetRecords ();

		override	MPWObjRecord	ReadOneRecord (RecordType type, istream& from);
		override	void			WriteOneRecord (const MPWObjRecord& rec, ostream& to);
		nonvirtual	void			PrintModules ();

	private:
		Mapping_HashTable (UInt16, String)	fIDToStringTable;

		nonvirtual	void	ProcessDictionaryRecord (const DictionaryRecord& dictRec);
};
static	CollectionSize	HashFunction (const UInt16& key)
{
	return (key);
}

MyMPWObjectFile::MyMPWObjectFile ():
	MPWObjectFile (),
	fIDToStringTable (&HashFunction, 1001)
{
}

AbSequence(MPWObjRecord)&	MyMPWObjectFile::GetRecords ()
{
	return (MPWObjectFile::GetRecords ());
}

MPWObjRecord	MyMPWObjectFile::ReadOneRecord (RecordType type, istream& from)
{
#if		qMacOS
	::SpinCursor(1);
#endif
	MPWObjRecord	r	=	MPWObjectFile::ReadOneRecord (type, from);
	if (r.GetRecordType () == eDictionary) {
		ProcessDictionaryRecord (DictionaryRecord (r));
	}
	return (r);
}

void	MyMPWObjectFile::WriteOneRecord (const MPWObjRecord& rec, ostream& to)
{
#if		qMacOS
	::SpinCursor(1);
#endif
	MPWObjectFile::WriteOneRecord (rec, to);
}

void	MyMPWObjectFile::PrintModules ()
{
	ForEach (MPWObjRecord, it, GetRecords ().operator Iterator(MPWObjRecord)* ()) {
		if (it.Current ().GetRecordType () == eModule) {
			UInt16	moduleID	=	ModuleRecord (it.Current ()).GetModuleID ();
			String	name;
			if (fIDToStringTable.Lookup (moduleID, &name)) {
				if (name == kEmptyString) {
					name = "<Anonymous>";
				}
			}
			else {
				cerr << "module ID not in dictionary!!! #" << moduleID << newline;
			}
			cout << "Module " << moduleID << " '" << name << "'" << newline;
#if		qMacOS
			::SpinCursor(1);
#endif
		}
	}
}

void	MyMPWObjectFile::ProcessDictionaryRecord (const DictionaryRecord& dictRec)
{
	if (dictRec.LinkIDs ()) {
		UInt16	i	=	0;
		ForEach (String, it, dictRec.GetStrings ().operator Iterator(String)* ()) {
			fIDToStringTable.Enter (dictRec.GetFirstID () + i, it.Current ());
			i++;
		}
	}
}







/*
 ********************************************************************************
 ******************************** FlungeFile ************************************
 ********************************************************************************
 */

#if		qPathNameBroke
void	FlungeFile (const String& fromFile, const String& toFile, ostream& diagnosticOut, Boolean verbose)
#else
void	FlungeFile (const PathName& fromFile, const PathName& toFile, ostream& diagnosticOut, Boolean verbose)
#endif
{
#if		qPathNameBroke
	char	buf1 [1024];
	char	buf2 [1024];
	int	fromFD		=	::open (fromFile.ToCStringTrunc (buf1, sizeof (buf1)), O_RDONLY);
	int	toFD		=	::open (toFile.ToCStringTrunc (buf2, sizeof (buf2)), O_RDWR | O_CREAT);
#else
	int	fromFD		=	FileSystem::Get ().Open (fromFile, O_RDONLY);
	int	toFD		=	FileSystem::Get ().Open (toFile, O_RDWR | O_CREAT);
#endif
	ifstream	in	=	fromFD;
	fstream		out	=	toFD;

	MyMPWObjectFile	objFile;
	if (verbose)	diagnosticOut << "Reading object file... '" << fromFile << "'" << newline;

#if		qMacOS
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
		::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
		::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
		::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
		::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
#endif

	objFile.ReadFrom (in);
	if (verbose)	diagnosticOut << "Read " << objFile.GetRecords ().GetLength () << " records (pad not included)." << newline;

	objFile.PrintModules ();

	if (verbose)	diagnosticOut << "Writing object file... '" << toFile << "'" << newline;
	objFile.WriteTo (out);
	if (verbose)	diagnosticOut << "Wrote " << objFile.GetRecords ().GetLength () << " records (pad not included)." << newline;

	::close (fromFD);
	out.flush ();
	::close (toFD);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

