/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Saveable.cc,v 1.5 1992/12/05 17:39:08 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Saveable.cc,v $
 *		Revision 1.5  1992/12/05  17:39:08  lewis
 *		Renamed Try->try, and Catch->catch - see exception.hh for explation.
 *
 *		Revision 1.4  1992/07/21  06:00:42  lewis
 *		Fix improper workarounds for qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
 *		bug.
 *
 *		Revision 1.3  1992/07/03  07:04:40  lewis
 *		Get rid of all references to #if qGCC - instead use proper defines for
 *		different bugs - qCFront_NestedTypesHalfAssed, qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport,
 *
 *		Revision 1.2  1992/06/28  03:59:20  lewis
 *		Lots of gross hacks around bad use of nested typing - clean these up better soon.
 *		Also, commented out code that used tab (int) - since we havent gotton our stream io manipulators
 *		to work with gcc yet.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/05/19  10:33:37  sterling
 *		made DoWrite const, added ReadTag/WriteTag
 *
 *		Revision 1.11  92/05/13  13:01:06  13:01:06  lewis (Lewis Pringle)
 *		STERL: modified routine to write version number to not write trailing newline.
 *		
 *		Revision 1.9  1992/02/15  05:35:26  sterling
 *		added read/write version utility routines
 *
 *
 *
 */


#include	"StreamUtils.hh"

#include	"Saveable.hh"





/*
 ********************************************************************************
 ************************************** Saveable ********************************
 ********************************************************************************
 */
Exception	Saveable::sFileFormatTooNew;
Exception	Saveable::sBadVersion;

const	Int16	Saveable::kBadVersion	=	-1;

#if		qCFront_NestedTypesHalfAssed
Saveable::Saveable (SaveableVersion maxVersion):
	fVersion (maxVersion),
	fMaxVersion (maxVersion)
{
}
#else
Saveable::Saveable (Saveable::Version maxVersion):
	fVersion (maxVersion),
	fMaxVersion (maxVersion)
{
}
#endif

Saveable::~Saveable ()
{
}

#if		qCFront_NestedTypesHalfAssed
SaveableVersion		Saveable::ReadVersion (SaveableVersion maxVersion, class istream& from)
#else
Saveable::Version	Saveable::ReadVersion (Saveable::Version maxVersion, class istream& from)
#endif
{
	char				c;
#if		qCFront_NestedTypesHalfAssed
	SaveableVersion		version;
#else
	Saveable::Version	version;
#endif

	from >> c;
	if (c == 'V') {
		from >> version;
	}
	else {
		version = kBadVersion;
		from.putback (c);
	}
	if (version == kBadVersion) {
		sBadVersion.Raise ();
	}
	if (version > maxVersion) {
		sFileFormatTooNew.Raise ();
	}
	
	return (version);
}

#if		qCFront_NestedTypesHalfAssed
void	Saveable::WriteVersion (SaveableVersion version, class ostream& to, int tabCount)
#else
void	Saveable::WriteVersion (Saveable::Version version, class ostream& to, int tabCount)
#endif
{
#if		qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
	tab (to, tabCount);
	to << 'V' << version << ' ';
#else	/*qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/
	to << tab (tabCount) << 'V' << version << ' ';
#endif	/*qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/
}

#if		qCFront_NestedTypesHalfAssed
Tag				Saveable::ReadTag (istream& from)
#else
Saveable::Tag	Saveable::ReadTag (istream& from)
#endif
{
#if		qCFront_NestedTypesHalfAssed
	Tag	tag;
#else
	Saveable::Tag	tag;
#endif
	from >> tag;
	return (tag);
}

void	Saveable::WriteTag (ostream& to, int tabCount) const
{
#if		qCFront_NestedTypesHalfAssed
	Tag	tag = (Tag)(void*)this;
	to << tab (tabCount) << tag << ' ';
#else	/*qCFront_NestedTypesHalfAssed*/
	Saveable::Tag	tag = (Saveable::Tag)(void*)this;

#if		qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
	tab (to, tabCount);
	to << tag << ' ';
#else	/*qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/
	to << tab (tabCount) << tag << ' ';
#endif	/*qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/

#endif	/*qCFront_NestedTypesHalfAssed*/
}

void	Saveable::DoRead (class istream& from)
{
	DoRead_ (from);
}

void	Saveable::DoRead_ (class istream& from)
{
	try {
		fVersion = ReadVersion (fMaxVersion, from);
	}
	catch1 (sBadVersion) {
		SetDirty ();
		_this_catch_->Raise ();		// propogate exception
	}
	
	if (fVersion < fMaxVersion) {
		PlusChangeCount ();
	}
}	

#if		qCFront_NestedTypesHalfAssed
void	Saveable::DoWrite (ostream& to, int tabCount, SaveableVersion version) const
#else
void	Saveable::DoWrite (ostream& to, int tabCount, Saveable::Version version) const
#endif
{
	Require (version <= GetMaxVersion ());
	
	Saveable* This = (Saveable*)this;
	This->SetVersion ((version == kBadVersion) ? GetMaxVersion () : version);
	DoWrite_ (to, tabCount);
	if (GetVersion () == GetMaxVersion ()) {
		This->ResetChangeCount ();
	}
}

void	Saveable::DoWrite_ (ostream& to, int tabCount) const
{
	WriteVersion (fVersion, to, tabCount);
}

void	Saveable::SetVersion (Int16 version)	
{
	Require (version <= fMaxVersion);
	
	if (fVersion != version) {
		SetVersion_ (version);
	}
	
	Ensure (fVersion == version);
}

void	Saveable::SetVersion_ (Int16 version)	
{
	fVersion = version; 
}

void	Saveable::SetMaxVersion (Int16 version)	
{
	Require (version >= GetVersion ());
	
	if (fMaxVersion != version) {
		SetMaxVersion_ (version);
	}
	
	Ensure (fMaxVersion == version);
}

void	Saveable::SetMaxVersion_ (Int16 version)	
{
	fMaxVersion = version; 
}

Boolean	Saveable::VersionUpToDate ()		
{	
	return (Boolean (fVersion == fMaxVersion));	
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

