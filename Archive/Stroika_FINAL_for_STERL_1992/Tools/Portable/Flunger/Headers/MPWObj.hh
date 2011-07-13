/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MPWObj__
#define	__MPWObj__

/*
 * $Header: /fuji/lewis/RCS/MPWObj.hh,v 1.1 1992/09/08 18:24:17 lewis Exp $
 *
 * Description:
 *
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: MPWObj.hh,v $
# Revision 1.1  1992/09/08  18:24:17  lewis
# Initial revision
#
 *
 *
 */

#include	"Array.hh"
#include	"Sequence.hh"
#include	"Config-Foundation.hh"



#if		qMPW_MacroOverflowProblem && !qRealTemplatesAvailable
#define	MPWObjRecord	MPWORe
#endif

class	MPWObjRecordRep;

#if		!qRealTemplatesAvailable
Declare (ReferenceCounted, MPWObjRecordRep);

Declare (Iterator, UInt8);
Declare (Collection, UInt8);
Declare (Array, UInt8);
Declare (AbSequence, UInt8);
Declare (Sequence_Array, UInt8);
Declare (Sequence, UInt8);
#endif


// See Stroika-Foundation-Globals.hh for an explanation of this. It is copied
// from there...
#ifndef		_ContainersOfStringDeclared_
	#include	"Sequence.hh"
	#include	"Set.hh"

	#define		_ContainersOfStringDeclared_
	Declare (Iterator, String);
	Declare (Collection, String);
	Declare (AbSequence, String);
	Declare (AbSet, String);
	Declare (Array, String);
	Declare (Sequence_Array, String);
	Declare (Sequence, String);
	Declare (Set_Array, String);
	Declare (Set, String);
#endif		/*_ContainersOfStringDeclared_*/


	//
	// It would be nice to make all these enums and classes nested, but for
	// lingering CFront 2.1 problems....
	// qCFront_NestedTypesHalfAssed (and maybe other troubles with no-templates)
	//
		// Note that these enums have values equal to the first byte of the record
		enum	RecordType {
			ePad				=	0,
			eFirst				=	1,
			eLast				=	2,
			eComment			=	3,
			eDictionary			=	4,
			eModule				=	5,
			eEntryPoint			=	6,
			eSize				=	7,
			eContents			=	8,
			eReference			=	9,
			eComputedReference	=	10,
			eFileName			=	11,
			eSourceStatement	=	12,
			eModuleBegin		=	13,
			eModuleEnd			=	14,
			eBlockBegin			=	15,
			eBlockEnd			=	16,
			eLocalIdentifier	=	17,
			eLocalLabel			=	18,
			eLocalType			=	19,
		
			eFirstRecordType	=	ePad,
			eLastRecordType		=	eLocalType,
		};

		
		class	MPWObjRecord {
			protected:
				MPWObjRecord (MPWObjRecordRep* rep);

public:
MPWObjRecord ();	// Needed cuz we use array

			public:
				nonvirtual	RecordType	GetRecordType () const;
				nonvirtual	void		Write (ostream& to) const;
	
			protected:
				#if		qRealTemplatesAvailable
					ReferenceCounted<MPWObjRecordRep>	fSharedPart;
				#else
					ReferenceCounted(MPWObjRecordRep)	fSharedPart;
				#endif
		};


		class	MPWObjRecordRep {
			protected:
				MPWObjRecordRep ();
			public:
				virtual ~MPWObjRecordRep ();
			public:
				virtual	RecordType	GetRecordType () const		=	NULL;
				virtual	void		Write (ostream& to) const	=	NULL;
		};
		
		class	PadRecord : public MPWObjRecord {
			public:
				PadRecord ();
		};

		class	PadRecordRep : public MPWObjRecordRep {
			public:
				PadRecordRep ();
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;
		};

		class	FirstRecord : public MPWObjRecord {
			public:
				FirstRecord (istream& from);
		};

		class	FirstRecordRep : public MPWObjRecordRep {
			public:
				FirstRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8	fFlags;
				UInt16	fVersion;
		};
		
		class	LastRecord : public MPWObjRecord {
			public:
				LastRecord (istream& from);
		};
		
		class	LastRecordRep : public MPWObjRecordRep {
			public:
				LastRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;
		};
		
		class	CommentRecord : public MPWObjRecord {
			public:
				CommentRecord (istream& from);
		};
		
		class	CommentRecordRep : public MPWObjRecordRep {
			public:
				CommentRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				String	fComment;
		};
		
		class	DictionaryRecord : public MPWObjRecord {
			public:
				DictionaryRecord (istream& from);
				DictionaryRecord (MPWObjRecord& from);	// Assertion error if not of proper type.. (check GetRecType)

				nonvirtual	Boolean	LinkIDs () const;		// linker ID names or SYM file symbolic names.
															// we only care about link names.
				nonvirtual	UInt16						GetFirstID () const;
				nonvirtual	const AbSequence(String)&	GetStrings () const;
		};
		
		class	DictionaryRecordRep : public MPWObjRecordRep {
			public:
				DictionaryRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				UInt16					fFirstID;
				Sequence_Array(String)	fStrings;
		};
		
		class	ModuleRecord : public MPWObjRecord {
			public:
				ModuleRecord (istream& from);
				ModuleRecord (MPWObjRecord& from);	// Assertion error if not of proper type.. (check GetRecType)

				nonvirtual	UInt16	GetModuleID () const;
		};
		
		class	ModuleRecordRep : public MPWObjRecordRep {
			public:
				ModuleRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8	fFlags;
				UInt16	fModuleID;
				UInt16	fSegmentIDOrSize;

			public:	// do block allocation for faster performance....
				nonvirtual	void*	operator new (size_t n);
				nonvirtual	void	operator delete (void* p);
		};
		
		class	EntryPointRecord : public MPWObjRecord {
			public:
				EntryPointRecord (istream& from);
		};
		
		class	EntryPointRecordRep : public MPWObjRecordRep {
			public:
				EntryPointRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8	fFlags;
				UInt16	fEntryID;
				UInt32	fOffset;
		};
		
		class	SizeRecord : public MPWObjRecord {
			public:
				SizeRecord (istream& from);
		};
		
		class	SizeRecordRep : public MPWObjRecordRep {
			public:
				SizeRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8	fFlags;
				UInt32	fModuleSize;
		};
		
		class	ContentsRecord : public MPWObjRecord {
			public:
				ContentsRecord (istream& from);
		};
		
		class	ContentsRecordRep : public MPWObjRecordRep {
			public:
				ContentsRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				UInt32					fOffset;		// zero if complete bit is set- otherwise offset in module of where this begins
				UInt16					fRepeat;		// 1 if repeat bit not set - otherwise the repeat count
				Sequence_Array(UInt8)	fContents;		//	actual code or data contents...

			public:	// do block allocation for faster performance....
				nonvirtual	void*	operator new (size_t n);
				nonvirtual	void	operator delete (void* p);
		};
		
		class	ReferenceRecord : public MPWObjRecord {
			public:
				ReferenceRecord (istream& from);
		};
		
		class	ReferenceRecordRep : public MPWObjRecordRep {
			public:
				ReferenceRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				UInt16					fID;
				Sequence_Array(UInt8)	fOffsetsInfo;

			public:	// do block allocation for faster performance....
				nonvirtual	void*	operator new (size_t n);
				nonvirtual	void	operator delete (void* p);
		};
		
		class	ComputedReferenceRecord : public MPWObjRecord {
			public:
				ComputedReferenceRecord (istream& from);
		};
		
		class	ComputedReferenceRecordRep : public MPWObjRecordRep {
			public:
				ComputedReferenceRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				UInt16					fID1;
				UInt16					fID2;
				Sequence_Array(UInt8)	fOffsetsInfo;
		};
		
		class	FileNameRecord : public MPWObjRecord {
			public:
				FileNameRecord (istream& from);
		};
		
		class	FileNameRecordRep : public MPWObjRecordRep {
			public:
				FileNameRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8	fFlags;
				UInt16	fFileID;
				UInt32	fModificationDate;
		};
		
		class	SourceStatementRecord : public MPWObjRecord {
			public:
				SourceStatementRecord (istream& from);
		};
		
		class	SourceStatementRecordRep : public MPWObjRecordRep {
			public:
				SourceStatementRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				UInt16					fParentID;
				UInt16					fFileID;
				Sequence_Array(UInt8)	fInfo;
		};

		class	ModuleBeginRecord : public MPWObjRecord {
			public:
				ModuleBeginRecord (istream& from);
		};
		
		class	ModuleBeginRecordRep : public MPWObjRecordRep {
			public:
				ModuleBeginRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8			fFlags;
				UInt16			fModuleID;
				UInt16			fParentID;
				UInt16			fFileID;
				UInt32			fFileOffset;		// could be 16bit according to flag in fFlags
				UInt8			fModuleKind;
		};

		class	ModuleEndRecord : public MPWObjRecord {
			public:
				ModuleEndRecord (istream& from);
		};
		
		class	ModuleEndRecordRep : public MPWObjRecordRep {
			public:
				ModuleEndRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8			fFlags;
				UInt16			fModuleID;
				UInt32			fFileOffset;
		};
		
		class	BlockBeginRecord : public MPWObjRecord {
			public:
				BlockBeginRecord (istream& from);
		};
		
		class	BlockBeginRecordRep : public MPWObjRecordRep {
			public:
				BlockBeginRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8			fFlags;
				UInt16			fBlockID;
				UInt16			fParentID;
				UInt16			fFileID;
				UInt32			fFileOffset;		// could be 16bit according to flag in fFlags
				UInt32			fCodeOffset;		// could be 16bit according to flag in fFlags
		};
		
		class	BlockEndRecord : public MPWObjRecord {
			public:
				BlockEndRecord (istream& from);
		};
		
		class	BlockEndRecordRep : public MPWObjRecordRep {
			public:
				BlockEndRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8			fFlags;
				UInt16			fBlockID;
				UInt32			fFileOffset;
				UInt32			fCodeOffset;
		};
		
		class	LocalIdentifierRecord : public MPWObjRecord {
			public:
				LocalIdentifierRecord (istream& from);
		};
		
		class	LocalIdentifierRecordRep : public MPWObjRecordRep {
			public:
				LocalIdentifierRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				Sequence_Array(UInt8)	fInfo;
		};

		class	LocalLabelRecord : public MPWObjRecord {
			public:
				LocalLabelRecord (istream& from);
		};
		
		class	LocalLabelRecordRep : public MPWObjRecordRep {
			public:
				LocalLabelRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				Sequence_Array(UInt8)	fInfo;
		};

		class	LocalTypeRecord : public MPWObjRecord {
			public:
				LocalTypeRecord (istream& from);
		};
		
		class	LocalTypeRecordRep : public MPWObjRecordRep {
			public:
				LocalTypeRecordRep (istream& from);
		
				override	RecordType	GetRecordType () const;
				override	void		Write (ostream& to) const;

			public:	// for now at least - just allow public access to these fields...
				UInt8					fFlags;
				Sequence_Array(UInt8)	fInfo;
		};

#if		!qRealTemplatesAvailable
	Declare (Iterator, MPWObjRecord);
	Declare (Collection, MPWObjRecord);
	Declare (AbSequence, MPWObjRecord);
	Declare (Array, MPWObjRecord);
	Declare (Sequence_Array, MPWObjRecord);
	Declare (Sequence, MPWObjRecord);
#endif





/*
 * In-core representation of an Object file. This can be created empty, and then
 * either read-from, or written to a real disk file (really iostreams). Copy of one of
 * these guys is done by value, and since they can be quite large, watchit!!!!
 */
class	MPWObjectFile {
	public:
		MPWObjectFile ();
		MPWObjectFile (const MPWObjectFile& from);
		~MPWObjectFile ();
		nonvirtual	const MPWObjectFile& operator= (const MPWObjectFile& rhs);

	public:
		nonvirtual	void	Clear ();					// remove all records
		nonvirtual	void	ReadFrom (istream& from);	// read all records from the given stream into this
		nonvirtual	void	WriteTo (ostream& to);		// write all records to the given stream from this

	// Note that we allow NON-CONST access to the record list!!!
	protected:
		nonvirtual	AbSequence(MPWObjRecord)&	GetRecords () { return (fRecords); }

		virtual	MPWObjRecord	ReadOneRecord (RecordType type, istream& from);
		virtual	void			WriteOneRecord (const MPWObjRecord& rec, ostream& to);

	private:
		Sequence_Array(MPWObjRecord)	fRecords;
};







/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__MPWObj__*/

