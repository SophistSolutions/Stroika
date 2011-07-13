/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Version__
#define	__Version__

/*
 * $Header: /fuji/lewis/RCS/Version.hh,v 1.4 1992/10/10 03:16:25 lewis Exp $
 *
 * Description:
 *
 *		Define application/software version information. Portable
 *	represenation we can use across machines. Really only well thgouht out
 *	for the mac - we may need to update to reflect versioning considerations
 *	on other systems.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Version.hh,v $
 *		Revision 1.4  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.3  1992/09/01  15:20:18  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.2  1992/02/19  01:09:02  lewis
 *		Ported to mac and added bugfix level.
 *
 *
 */

#include	"String.hh"


class	Version {
	public:
		typedef	UInt8	MajorVersion;
		typedef	UInt8	MinorVersion;
		typedef	UInt8	BugFixVersion;
		enum Stage {
			eDevelopment,
			eAlpha,
			eBeta,
			eRelease,
		};
		typedef	UInt8	StageVersion;

		Version (MajorVersion majorVersion, MinorVersion minorVersion, BugFixVersion bugFixVersion, Stage stage,
				 StageVersion stageVersion, const String& shortVersionString,
				 const String& longVersionString);

#if		qMacOS
		// windows also like this! But X NO!!! Their resource mechanism inappropriate!!!
		Version (int resourceID);
#endif

		nonvirtual	MajorVersion	GetMajorVersion () const;
		nonvirtual	void			SetMajorVersion (MajorVersion majorVersion);
		nonvirtual	MinorVersion	GetMinorVersion () const;
		nonvirtual	void			SetMinorVersion (MinorVersion minorVersion);
		nonvirtual	BugFixVersion	GetBugFixVersion () const;
		nonvirtual	void			SetBugFixVersion (BugFixVersion bugFixVersion);
		nonvirtual	Stage			GetStage () const;
		nonvirtual	void			SetStage (Stage stage);
		nonvirtual	StageVersion	GetStageVersion () const;
		nonvirtual	void			SetStageVersion (StageVersion stageVersion);
		nonvirtual	String			GetShortVersionString () const;
		nonvirtual	void			SetShortVersionString (const String& shortVersionString);
		nonvirtual	String			GetLongVersionString () const;
		nonvirtual	void			SetLongVersionString (const String& longVersionString);


	private:
		MajorVersion	fMajorVersion;
		MinorVersion	fMinorVersion;
		BugFixVersion	fBugFixVersion;
		Stage			fStage;
		StageVersion	fStageVersion;
		String			fShortVersionString;
		String			fLongVersionString;
};


extern	const	Version	kStroikaVersion;				// we define this in Stroika
extern	const	Version	kApplicationVersion;			// each App must define this (value undefined til start of main ())






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

#endif	/*__Version__*/
