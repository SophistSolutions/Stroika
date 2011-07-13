/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__InfoDB__
#define	__InfoDB__

/*
 * $Header: /fuji/lewis/RCS/InfoDB.hh,v 1.8 1992/11/11 04:03:30 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: InfoDB.hh,v $
 *		Revision 1.8  1992/11/11  04:03:30  lewis
 *		Cleanup and use bug defines for workarounds like qGCC_NoAutoGenOfDefaultCTORBug
 *		and qGCC_OperatorNewAndStartPlusPlusBug.
 *
 *		Revision 1.7  1992/10/16  20:01:48  lewis
 *		Get rid of qGlarf crap, and do slightly more correct
 *		versions of op= for various classes defined here.
 *
 *		Revision 1.6  1992/10/09  14:17:12  lewis
 *		Include SequenceOfString_hh for psedo-templates.
 *
 *		Revision 1.5  1992/10/08  02:07:39  sterling
 *		Conditional support for using GenClass to boostrap.
 *
 *		Revision 1.4  1992/10/02  23:49:10  lewis
 *		Lots of hacks - got most of the various classes generating properly-
 *		most of the hacks were required to get Mapping to work - some of them
 *		turned out to be unneeded, but I left them in anyhow.
 *
 *		Revision 1.3  1992/09/30  03:25:01  lewis
 *		(LGP checkin for STERL)Brokup declare macro to get to compile
 *		on mac.
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"File.hh"
#include	"Mapping_HashTable.hh"
#include	"Sequence.hh"
#include	"String.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SequenceOfString_hh
#endif


// InfoDependency SB scoped but that tends to break compilers.... LGP Saturday, September 26, 1992 11:37:03 PM
	struct	InfoDependency {
#if		qGCC_NoAutoGenOfDefaultCTORBug
		InfoDependency ();
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration
		// Also GCC bug workaround...
		InfoDependency (const InfoDependency& from):fBangDep(from.fBangDep),fBangBang(from.fBangBang),fName(from.fName),fArguments(from.fArguments)    {}
		InfoDependency& operator=(const InfoDependency& rhs)
		{
			fBangDep = rhs.fBangDep;
			fBangBang = rhs.fBangBang;
			fName = rhs.fName;
			fArguments = rhs.fArguments;
			return *this;
		}
#endif
		Boolean				fBangDep;
		Boolean				fBangBang;
		String				fName;
		Sequence(String)	fArguments;						// formal parameter names to this template
	};
// really SB more elaborate...
inline Boolean operator == (const InfoDependency& lhs, const InfoDependency& rhs) { return Boolean ((lhs.fName == rhs.fName) and (lhs.fArguments == rhs.fArguments)); }

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SequenceOfInfoDependency_hh
#endif
class	ostream;
extern	ostream&	operator << (ostream& out, const InfoDependency& infoDependency);


struct	RenameRecord {
#if		qGCC_NoAutoGenOfDefaultCTORBug
		RenameRecord ();
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration
		RenameRecord (const RenameRecord& from):fTemplateName(from.fTemplateName),fShortName(from.fShortName),fArguments(from.fArguments)   {}
		RenameRecord& operator=(const RenameRecord& rhs)
		{
			fTemplateName = rhs.fTemplateName;
			fShortName = rhs.fShortName;
			fArguments = rhs.fArguments;
			return *this;
		}
#endif
	String				fTemplateName;
	String				fShortName;
	Sequence(String)	fArguments;
};
ostream&	operator << (ostream& out, const RenameRecord& renameRecord);
inline Boolean operator == (const RenameRecord& lhs, const RenameRecord& rhs) { return Boolean ((lhs.fTemplateName == rhs.fTemplateName) and (lhs.fShortName == rhs.fShortName)  and (lhs.fArguments == rhs.fArguments)); }
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include SequenceOfRenameRecord_hh
#endif
struct	InfoRecord {
#if		qGCC_NoAutoGenOfDefaultCTORBug
		InfoRecord ();
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStartPlusPlusBug
		InfoRecord (const InfoRecord& from);
		InfoRecord& operator=(const InfoRecord& rhs);
#endif
	String						fTemplateName;			// name of file/category of templates (eg Array also
														// declares ArrayNode, ArrayIterator, etc...)
	Sequence(String)			fArguments;				// formal parameter names to templates in this file
	String						fRawText;				// stuff between ##'s
	Sequence(InfoDependency)	fHeaderDependencies;
	Sequence(InfoDependency)	fSourceDependencies;
	Sequence(RenameRecord)		fRenames;
};
Boolean operator == (const InfoRecord& lhs, const InfoRecord& rhs);

extern	ostream&	operator << (ostream& out, const InfoRecord& infoRecord);

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include Mapping_HashTableOfString_InfoRecord_hh
#endif


extern	Mapping(String,InfoRecord)	ReadInfoRecords (const PathName& pathName);



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

#endif	/*__InfoDB__*/
