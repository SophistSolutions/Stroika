/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MPWObj.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MPWObj.cc,v $
// Revision 1.1  1992/09/08  18:24:32  lewis
// Initial revision
//
 *
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/MPWObj.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $";
#endif

#include	<fstream.h>
#include	<strstream.h>

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"MPWObj.hh"




#if		!qRealTemplatesAvailable
Implement (Iterator, UInt8);
Implement (Collection, UInt8);
Implement (Array, UInt8);
Implement (AbSequence, UInt8);
Implement (Sequence_Array, UInt8);
Implement (Sequence, UInt8);
#endif







/*
 ********************************************************************************
 ******************************** MPWObjectFile *********************************
 ********************************************************************************
 */
MPWObjectFile::MPWObjectFile ():
	fRecords ()
{
}

MPWObjectFile::MPWObjectFile (const MPWObjectFile& from):
	fRecords ()
{
	fRecords = from.fRecords;
}

MPWObjectFile::~MPWObjectFile ()
{
}

const MPWObjectFile& MPWObjectFile::operator= (const MPWObjectFile& rhs)
{
	fRecords = rhs.fRecords;
	return (*this);
}

void	MPWObjectFile::Clear ()
{
	fRecords.RemoveAll ();
}

void	MPWObjectFile::ReadFrom (istream& from)
{
	while (from) {
		UInt8	recType	=	from.get ();
		if (!from) {
			break;		// EOF? Really should test not other sort of error!!! No - I guess do at different level...
		}
		if (recType == 255) {
			break;		// should have been handled by above???
		}

//fRecords.SetSlotsAlloced (RoundUpTo ((unsigned long)fRecords.GetLength (), 1000ul));	// keep from spending too much time in realloc
		fRecords.SetSlotsAlloced (RoundUpTo ((int)fRecords.GetLength (), 1000l));	// keep from spending too much time in realloc

		MPWObjRecord	a	=	ReadOneRecord (recType, from);
		if (a.GetRecordType () != ePad) {
			fRecords.Append (a);
		}
	}
//fRecords.Compact ();	// since we over set teh slots alloced earlier and we probably wont be extending this list...
}

void	MPWObjectFile::WriteTo (ostream& to)
{
	/*
	 * If the file offset is odd, then write a pad record. This probably shouldn't ever happen?
	 */
	if (to.tellp () & 1) {
		PadRecord ().Write (to);
	}
	ForEach (MPWObjRecord, it, fRecords) {
		if (it.Current ().GetRecordType () != ePad) {
			WriteOneRecord (it.Current (), to);
			/*
			 * If the file offset is odd, then write a pad record.
			 */
			if (to.tellp () & 1) {
				PadRecord ().Write (to);
			}
		}
	}
}

MPWObjRecord	MPWObjectFile::ReadOneRecord (RecordType type, istream& from)
{
	switch (type) {
		case	ePad:				return (PadRecord ());
		case	eFirst:				return (FirstRecord (from)); break;
		case	eLast:				return (LastRecord (from)); break;
		case	eComment:			return (CommentRecord (from)); break;
		case	eDictionary:		return (DictionaryRecord (from)); break;
		case	eModule:			return (ModuleRecord (from)); break;
		case	eEntryPoint:		return (EntryPointRecord (from)); break;
		case	eSize:				return (SizeRecord (from)); break;
		case	eContents:			return (ContentsRecord (from)); break;
		case	eReference:			return (ReferenceRecord (from)); break;
		case	eComputedReference:	return (ComputedReferenceRecord (from)); break;
		case	eFileName:			return (FileNameRecord (from)); break;
		case	eSourceStatement:	return (SourceStatementRecord (from)); break;
		case	eModuleBegin:		return (ModuleBeginRecord (from)); break;
		case	eModuleEnd:			return (ModuleEndRecord (from)); break;
		case	eBlockBegin:		return (BlockBeginRecord (from)); break;
		case	eBlockEnd:			return (BlockEndRecord (from)); break;
		case	eLocalIdentifier:	return (LocalIdentifierRecord (from)); break;
		case	eLocalLabel:		return (LocalLabelRecord (from)); break;
		case	eLocalType:			return (LocalTypeRecord (from)); break;
		default:					Exception().Raise ();
	}
	AssertNotReached ();
}

void	MPWObjectFile::WriteOneRecord (const MPWObjRecord& rec, ostream& to)
{
	rec.Write (to);
}





/*
 ********************************************************************************
 ********************************* MPWObjRecord *********************************
 ********************************************************************************
 */
 	Boolean	operator== (MPWObjRecord, MPWObjRecord) { AssertNotReached (); return (False); }
 	Boolean	operator!= (MPWObjRecord, MPWObjRecord) { AssertNotReached (); return (False); }
#if		!qRealTemplatesAvailable
	Implement (Iterator, MPWObjRecord);
	Implement (Collection, MPWObjRecord);
	Implement (AbSequence, MPWObjRecord);
	Implement (Array, MPWObjRecord);
	Implement (Sequence_Array, MPWObjRecord);
	Implement (Sequence, MPWObjRecord);
#endif

MPWObjRecord::MPWObjRecord (MPWObjRecordRep* rep):
	fSharedPart (rep)
{
}

MPWObjRecord::MPWObjRecord ():
	fSharedPart (NULL)
{
}


RecordType	MPWObjRecord::GetRecordType () const
{
	return (fSharedPart->GetRecordType ());
}

void	MPWObjRecord::Write (ostream& to) const
{
	fSharedPart->Write (to);
}




/*
 ********************************************************************************
 ****************************** MPWObjRecordRep *********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
Implement (ReferenceCounted, MPWObjRecordRep);
#endif

MPWObjRecordRep::MPWObjRecordRep ()
{
}

MPWObjRecordRep::~MPWObjRecordRep ()
{
}



/*
 ********************************************************************************
 ************************************ PadRecord *********************************
 ********************************************************************************
 */
PadRecord::PadRecord ():
	MPWObjRecord (new PadRecordRep ())
{
}



	
/*
 ********************************************************************************
 ********************************* PadRecordRep *********************************
 ********************************************************************************
 */
PadRecordRep::PadRecordRep ():
	MPWObjRecordRep ()
{
}
		
RecordType	PadRecordRep::GetRecordType () const
{
	return (ePad);
}

void	PadRecordRep::Write (ostream& to) const
{
	Assert (ePad == 0);
	to.put (ePad);
}




/*
 ********************************************************************************
 ********************************** FirstRecord *********************************
 ********************************************************************************
 */
FirstRecord::FirstRecord (istream& from):
	MPWObjRecord (new FirstRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ********************************* FirstRecordRep *******************************
 ********************************************************************************
 */
FirstRecordRep::FirstRecordRep (istream& from):
	MPWObjRecordRep ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	from.read ((char*)&fVersion, sizeof (fVersion));
}
		
RecordType	FirstRecordRep::GetRecordType () const
{
	return (eFirst);
}

void		FirstRecordRep::Write (ostream& to) const
{
	to.put (eFirst);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	to.write ((const char*)&fVersion, sizeof (fVersion));
}




/*
 ********************************************************************************
 *********************************** LastRecord *********************************
 ********************************************************************************
 */
LastRecord::LastRecord (istream& from):
	MPWObjRecord (new LastRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ********************************* LastRecordRep ********************************
 ********************************************************************************
 */
LastRecordRep::LastRecordRep (istream& from):
	MPWObjRecordRep ()
{
	UInt8	extraByte	=	0;
	from.read ((char*)&extraByte, sizeof (extraByte));
	if (extraByte != 0) {
		Exception ().Raise ();
	}
}
		
RecordType	LastRecordRep::GetRecordType () const
{
	return (eLast);
}

void		LastRecordRep::Write (ostream& to) const
{
	to.put (eLast);
	to.put (0);			// extra byte so even size...
}




/*
 ********************************************************************************
 ******************************** CommentRecord *********************************
 ********************************************************************************
 */
CommentRecord::CommentRecord (istream& from):
	MPWObjRecord (new CommentRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** CommentRecordRep *********************************
 ********************************************************************************
 */
CommentRecordRep::CommentRecordRep (istream& from):
	MPWObjRecordRep (),
	fComment (kEmptyString)
{
	UInt16	nBytes	=	0;
	from.read ((char*)&nBytes, sizeof (nBytes));
	// then fill in the string by reading nBytes bytes into it...
	char* buf = new char [nBytes];
	from.read ((char*)buf, nBytes);
	fComment = buf;
	delete (buf);
}
		
RecordType	CommentRecordRep::GetRecordType () const
{
	return (eComment);
}

void	CommentRecordRep::Write (ostream& to) const
{
	to.put (eComment);
	UInt16	nBytes	=	fComment.GetLength ();
	to.write ((const char*)&nBytes, sizeof (nBytes));
	for (register CollectionSize i = 1; i <= nBytes; i++) {
		to.put (fComment [i].GetAsciiCode ());
	}
}




/*
 ********************************************************************************
 ******************************* DictionaryRecord *******************************
 ********************************************************************************
 */
DictionaryRecord::DictionaryRecord (istream& from):
	MPWObjRecord (new DictionaryRecordRep (from))
{
}

DictionaryRecord::DictionaryRecord (MPWObjRecord& from):
	MPWObjRecord (from)
{
	Require (GetRecordType () == eDictionary);
}

Boolean	DictionaryRecord::LinkIDs () const
{
	Assert (GetRecordType () == eDictionary);
	return not (((DictionaryRecordRep*)fSharedPart.operator-> ())->fFlags & (1<<7));
}

UInt16	DictionaryRecord::GetFirstID () const
{
	Assert (GetRecordType () == eDictionary);
	return (((DictionaryRecordRep*)fSharedPart.operator-> ())->fFirstID);
}

const AbSequence(String)&	DictionaryRecord::GetStrings () const
{
	Assert (GetRecordType () == eDictionary);
	return (((DictionaryRecordRep*)fSharedPart.operator-> ())->fStrings);
}


	
/*
 ********************************************************************************
 ***************************** DictionaryRecordRep ******************************
 ********************************************************************************
 */
DictionaryRecordRep::DictionaryRecordRep (istream& from):
	MPWObjRecordRep (),
	fFlags (0),
	fFirstID (0),
	fStrings ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	UInt16	recSize	=	0;
	from.read ((char*)&recSize, sizeof (recSize));
	recSize -= 6;			// subtract flags, and non-stringlist stuff
	from.read ((char*)&fFirstID, sizeof (fFirstID));
	char* buf = new char [recSize];
	from.read ((char*)buf, recSize);
	char*	bufPtr = buf;
	for (UInt16 totalSizeLeft = recSize; totalSizeLeft > 0; ) {
//fStrings.SetSlotsAlloced (RoundUpTo ((unsigned long)fStrings.GetLength (), 1000ul));	// keep from spending too much time in realloc
fStrings.SetSlotsAlloced (RoundUpTo ((int)fStrings.GetLength (), 1000ul));	// keep from spending too much time in realloc
		UInt16	strLen	=	*(UInt8*)bufPtr;
		String	s;
		s.SetLength (strLen);
		for (UInt16 i = 1; i <= strLen; i++) {
			s.SetCharAt (bufPtr [i], i);
		}
		bufPtr += (strLen+1);
		totalSizeLeft -= (strLen+1);
		fStrings.Append (s);
	}
	delete (buf);
fStrings.SetSlotsAlloced (fStrings.GetLength ());	// COMPACT CALL DOES NOT EXIST YET
}
		
RecordType	DictionaryRecordRep::GetRecordType () const
{
	return (eDictionary);
}

void	DictionaryRecordRep::Write (ostream& to) const
{
	to.put (eDictionary);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	/*
	 * Write a bad size, and come back later and patch it.
	 */
	streampos	sizePos	=	to.tellp (); 
	UInt16		recSize	=	6;		// add flags, and non-stringlist stuff
	to.write ((const char*)&recSize, sizeof (recSize));
	to.write ((const char*)&fFirstID, sizeof (fFirstID));
	ForEach (String, it, fStrings) {
		UInt8	len	=	it.Current ().GetLength ();
		to.write ((const char*)&len, sizeof (len));
		to << it.Current ();						// I assume this does no formatting and just writes chars???
		recSize += (len+1);
	}

	/*
	 * Now patch the size, and go back to the end.
	 */
	streampos	endPos	=	to.tellp (); 
	to.seekp (sizePos);
	to.write ((const char*)&recSize, sizeof (recSize));
	to.seekp (endPos);
}




/*
 ********************************************************************************
 ********************************* ModuleRecord *********************************
 ********************************************************************************
 */
ModuleRecord::ModuleRecord (istream& from):
	MPWObjRecord (new ModuleRecordRep (from))
{
}

ModuleRecord::ModuleRecord (MPWObjRecord& from):
	MPWObjRecord (from)
{
	Require (GetRecordType () == eModule);
}

UInt16	ModuleRecord::GetModuleID () const
{
	Assert (GetRecordType () == eModule);
	return (((ModuleRecordRep*)fSharedPart.operator-> ())->fModuleID);
}



	
/*
 ********************************************************************************
 ***************************** ModuleRecordRep **********************************
 ********************************************************************************
 */
ModuleRecordRep::ModuleRecordRep (istream& from):
	MPWObjRecordRep (),
	fFlags (0),
	fModuleID (0),
	fSegmentIDOrSize (0)
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	from.read ((char*)&fModuleID, sizeof (fModuleID));
	from.read ((char*)&fSegmentIDOrSize, sizeof (fSegmentIDOrSize));
}
		
RecordType	ModuleRecordRep::GetRecordType () const
{
	return (eModule);
}

void	ModuleRecordRep::Write (ostream& to) const
{
	to.put (eModule);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	to.write ((const char*)&fModuleID, sizeof (fModuleID));
	to.write ((const char*)&fSegmentIDOrSize, sizeof (fSegmentIDOrSize));
}

#if		!qRealTemplatesAvailable
	BlockAllocatedDeclare (ModuleRecordRep);
	BlockAllocatedImplement (ModuleRecordRep);
#endif

void*	ModuleRecordRep::operator new (size_t n)
{
	#if		qRealTemplatesAvailable
		return (BlockAllocated<ModuleRecordRep>::operator new (n));
	#else
		return (BlockAllocated(ModuleRecordRep)::operator new (n));
	#endif
}

void	ModuleRecordRep::operator delete (void* p)
{
	#if		qRealTemplatesAvailable
		BlockAllocated<ModuleRecordRep>::operator delete (p);
	#else
		BlockAllocated(ModuleRecordRep)::operator delete (p);
	#endif
}




/*
 ********************************************************************************
 ***************************** EntryPointRecord *********************************
 ********************************************************************************
 */
EntryPointRecord::EntryPointRecord (istream& from):
	MPWObjRecord (new EntryPointRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** EntryPointRecordRep ******************************
 ********************************************************************************
 */
EntryPointRecordRep::EntryPointRecordRep (istream& from):
	MPWObjRecordRep ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	from.read ((char*)&fEntryID, sizeof (fEntryID));
	from.read ((char*)&fOffset, sizeof (fOffset));
}
		
RecordType	EntryPointRecordRep::GetRecordType () const
{
	return (eEntryPoint);
}

void	EntryPointRecordRep::Write (ostream& to) const
{
	to.put (eEntryPoint);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	to.write ((const char*)&fEntryID, sizeof (fEntryID));
	to.write ((const char*)&fOffset, sizeof (fOffset));
}




/*
 ********************************************************************************
 *********************************** SizeRecord *********************************
 ********************************************************************************
 */
SizeRecord::SizeRecord (istream& from):
	MPWObjRecord (new SizeRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ******************************** SizeRecordRep *********************************
 ********************************************************************************
 */
SizeRecordRep::SizeRecordRep (istream& from):
	MPWObjRecordRep ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	from.read ((char*)&fModuleSize, sizeof (fModuleSize));
}
		
RecordType	SizeRecordRep::GetRecordType () const
{
	return (eSize);
}

void	SizeRecordRep::Write (ostream& to) const
{
	to.put (eSize);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	to.write ((const char*)&fModuleSize, sizeof (fModuleSize));
}




/*
 ********************************************************************************
 ******************************* ContentsRecord *********************************
 ********************************************************************************
 */
ContentsRecord::ContentsRecord (istream& from):
	MPWObjRecord (new ContentsRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** ContentsRecordRep ********************************
 ********************************************************************************
 */
ContentsRecordRep::ContentsRecordRep (istream& from):
	MPWObjRecordRep (),
	fFlags (0),
	fOffset (0),
	fRepeat (1),
	fContents ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	UInt16	recSize	=	0;
	from.read ((char*)&recSize, sizeof (recSize));
	recSize -= 4;		// for recSize+ flags + tag
	if (not (fFlags & (1<<3))) {
		// test the complete flag, and if not complete, read in repeat count - else leave as 1...
		from.read ((char*)&fOffset, sizeof (fOffset));
		recSize -= 4;		// for offset
	}
	if (fFlags & (1<<4)) {
		// test the repeat flag, and if on, read in offset...
		from.read ((char*)&fRepeat, sizeof (fRepeat));
		recSize -= 2;		// for repeat count
	}

	unsigned char*	contents	=	new unsigned char [recSize];
	from.read ((char*)contents, recSize);
fContents.SetSlotsAlloced (recSize);
	for (UInt16 i = 0; i < recSize; i++) {
		fContents.Append (contents [i]);
	}
	delete (contents);
}
		
RecordType	ContentsRecordRep::GetRecordType () const
{
	return (eContents);
}

void	ContentsRecordRep::Write (ostream& to) const
{
	to.put (eContents);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	UInt16	recSize	=	4;
	if (not (fFlags & (1<<3))) {
		recSize += 4;		// for offset
	}
	if (fFlags & (1<<4)) {
		recSize += 2;		// for repeat count
	}
	recSize += fContents.GetLength ();
	to.write ((const char*)&recSize, sizeof (recSize));


	if (not (fFlags & (1<<3))) {
		// test the complete flag, and if not complete, read in repeat count - else leave as 1...
		to.write ((char*)&fOffset, sizeof (fOffset));
	}
	if (fFlags & (1<<4)) {
		// test the repeat flag, and if on, read in offset...
		to.write ((char*)&fRepeat, sizeof (fRepeat));
	}

	ForEach (UInt8, it, fContents) {
		to.put (it.Current ());
	}
}

#if		!qRealTemplatesAvailable
	BlockAllocatedDeclare (ContentsRecordRep);
	BlockAllocatedImplement (ContentsRecordRep);
#endif

void*	ContentsRecordRep::operator new (size_t n)
{
	#if		qRealTemplatesAvailable
		return (BlockAllocated<ContentsRecordRep>::operator new (n));
	#else
		return (BlockAllocated(ContentsRecordRep)::operator new (n));
	#endif
}

void	ContentsRecordRep::operator delete (void* p)
{
	#if		qRealTemplatesAvailable
		BlockAllocated<ContentsRecordRep>::operator delete (p);
	#else
		BlockAllocated(ContentsRecordRep)::operator delete (p);
	#endif
}




/*
 ********************************************************************************
 ****************************** ReferenceRecord *********************************
 ********************************************************************************
 */
ReferenceRecord::ReferenceRecord (istream& from):
	MPWObjRecord (new ReferenceRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** ReferenceRecordRep *******************************
 ********************************************************************************
 */
ReferenceRecordRep::ReferenceRecordRep (istream& from):
	MPWObjRecordRep (),
	fFlags (0),
	fID (0),
	fOffsetsInfo ()
{
	from.read ((char*)&fFlags, sizeof (fFlags));
	UInt16	recSize	=	0;
	from.read ((char*)&recSize, sizeof (recSize));
	recSize -= 4;		// for recSize+ flags + tag
	from.read ((char*)&fID, sizeof (fID));
	recSize -= sizeof (fID);

	unsigned char*	contents	=	new unsigned char [recSize];
	from.read ((char*)contents, recSize);
fOffsetsInfo.SetSlotsAlloced (recSize);
	for (UInt16 i = 0; i < recSize; i++) {
		fOffsetsInfo.Append (contents [i]);
	}
	delete (contents);
}

RecordType	ReferenceRecordRep::GetRecordType () const
{
	return (eReference);
}

void	ReferenceRecordRep::Write (ostream& to) const
{
	to.put (eReference);
	to.write ((const char*)&fFlags, sizeof (fFlags));
	UInt16	recSize	=	fOffsetsInfo.GetLength () + 6;
	to.write ((const char*)&recSize, sizeof (recSize));
	to.write ((const char*)&fID, sizeof (fID));

	ForEach (UInt8, it, fOffsetsInfo) {
		to.put (it.Current ());
	}
}

#if		!qRealTemplatesAvailable
	BlockAllocatedDeclare (ReferenceRecordRep);
	BlockAllocatedImplement (ReferenceRecordRep);
#endif

void*	ReferenceRecordRep::operator new (size_t n)
{
	#if		qRealTemplatesAvailable
		return (BlockAllocated<ReferenceRecordRep>::operator new (n));
	#else
		return (BlockAllocated(ReferenceRecordRep)::operator new (n));
	#endif
}

void	ReferenceRecordRep::operator delete (void* p)
{
	#if		qRealTemplatesAvailable
		BlockAllocated<ReferenceRecordRep>::operator delete (p);
	#else
		BlockAllocated(ReferenceRecordRep)::operator delete (p);
	#endif
}




/*
 ********************************************************************************
 *************************** ComputedReferenceRecord ****************************
 ********************************************************************************
 */
ComputedReferenceRecord::ComputedReferenceRecord (istream& from):
	MPWObjRecord (new ComputedReferenceRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** ComputedReferenceRecordRep ***********************
 ********************************************************************************
 */
ComputedReferenceRecordRep::ComputedReferenceRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	ComputedReferenceRecordRep::GetRecordType () const
{
	return (eComputedReference);
}

void		ComputedReferenceRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ******************************* FileNameRecord *********************************
 ********************************************************************************
 */
FileNameRecord::FileNameRecord (istream& from):
	MPWObjRecord (new FileNameRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** FileNameRecordRep ********************************
 ********************************************************************************
 */
FileNameRecordRep::FileNameRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	FileNameRecordRep::GetRecordType () const
{
	return (eFileName);
}

void		FileNameRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ****************************** SourceStatementRecord ***************************
 ********************************************************************************
 */
SourceStatementRecord::SourceStatementRecord (istream& from):
	MPWObjRecord (new SourceStatementRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** SourceStatementRecordRep *************************
 ********************************************************************************
 */
SourceStatementRecordRep::SourceStatementRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	SourceStatementRecordRep::GetRecordType () const
{
	return (eSourceStatement);
}

void		SourceStatementRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 **************************** ModuleBeginRecord *********************************
 ********************************************************************************
 */
ModuleBeginRecord::ModuleBeginRecord (istream& from):
	MPWObjRecord (new ModuleBeginRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** ModuleBeginRecordRep *****************************
 ********************************************************************************
 */
ModuleBeginRecordRep::ModuleBeginRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	ModuleBeginRecordRep::GetRecordType () const
{
	return (eModuleBegin);
}

void		ModuleBeginRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ****************************** ModuleEndRecord *********************************
 ********************************************************************************
 */
ModuleEndRecord::ModuleEndRecord (istream& from):
	MPWObjRecord (new ModuleEndRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** ModuleEndRecordRep *******************************
 ********************************************************************************
 */
ModuleEndRecordRep::ModuleEndRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	ModuleEndRecordRep::GetRecordType () const
{
	return (eModuleEnd);
}

void		ModuleEndRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ***************************** BlockBeginRecord *********************************
 ********************************************************************************
 */
BlockBeginRecord::BlockBeginRecord (istream& from):
	MPWObjRecord (new BlockBeginRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** BlockBeginRecordRep ******************************
 ********************************************************************************
 */
BlockBeginRecordRep::BlockBeginRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	BlockBeginRecordRep::GetRecordType () const
{
	return (eBlockBegin);
}

void		BlockBeginRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ******************************* BlockEndRecord *********************************
 ********************************************************************************
 */
BlockEndRecord::BlockEndRecord (istream& from):
	MPWObjRecord (new BlockEndRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ***************************** BlockEndRecordRep ********************************
 ********************************************************************************
 */
BlockEndRecordRep::BlockEndRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	BlockEndRecordRep::GetRecordType () const
{
	return (eBlockEnd);
}

void		BlockEndRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 **************************** LocalIdentifierRecord *****************************
 ********************************************************************************
 */
LocalIdentifierRecord::LocalIdentifierRecord (istream& from):
	MPWObjRecord (new LocalIdentifierRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ************************* LocalIdentifierRecordRep *****************************
 ********************************************************************************
 */
LocalIdentifierRecordRep::LocalIdentifierRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	LocalIdentifierRecordRep::GetRecordType () const
{
	return (eLocalIdentifier);
}

void		LocalIdentifierRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ***************************** LocalLabelRecord *********************************
 ********************************************************************************
 */
LocalLabelRecord::LocalLabelRecord (istream& from):
	MPWObjRecord (new LocalLabelRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ****************************** LocalLabelRecordRep *****************************
 ********************************************************************************
 */
LocalLabelRecordRep::LocalLabelRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	LocalLabelRecordRep::GetRecordType () const
{
	return (eLocalLabel);
}

void		LocalLabelRecordRep::Write (ostream& to) const
{
}




/*
 ********************************************************************************
 ****************************** LocalTypeRecord *********************************
 ********************************************************************************
 */
LocalTypeRecord::LocalTypeRecord (istream& from):
	MPWObjRecord (new LocalTypeRecordRep (from))
{
}



	
/*
 ********************************************************************************
 ******************************* LocalTypeRecordRep *****************************
 ********************************************************************************
 */
LocalTypeRecordRep::LocalTypeRecordRep (istream& from):
	MPWObjRecordRep ()
{
AssertNotImplemented ();
}
		
RecordType	LocalTypeRecordRep::GetRecordType () const
{
	return (eLocalType);
}

void		LocalTypeRecordRep::Write (ostream& to) const
{
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

