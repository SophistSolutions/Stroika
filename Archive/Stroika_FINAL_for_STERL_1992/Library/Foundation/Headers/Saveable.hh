/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Saveable__
#define	__Saveable__

/*
 * $Header: /fuji/lewis/RCS/Saveable.hh,v 1.5 1992/10/10 20:19:00 lewis Exp $
 *
 * Description:
 *
 *		For classes that can be saved. Has virtual hooks to read and write, and maintains
 *		a version stamp. Also, keeps tabs on whether it has changed or not (for example, 
 *		if a out-of-date version is read in, it is always considered dirty. By default, will
 *		save files as the latest version, but can optionally pass a version into write, in 
 *		which case a file of the version should be written.
 *
 * TODO:
 *
 * Changes:
 *	$Log: Saveable.hh,v $
 *		Revision 1.5  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.4  1992/07/02  03:01:49  lewis
 *		Fixed syntax error with cfront version.
 *
 *		Revision 1.3  1992/06/28  03:58:25  lewis
 *		Fixed code for proper nested type behavior for inlines at end: GetVersion/GetMaxVersion.
 *
 *		Revision 1.2  1992/06/25  02:57:30  lewis
 *		Fix use of nested classes conditional on qCFront_NestedTypesHalfAssed flag since gcc requires
 *		we get it right, and CFront requires that we get it wrong.
 *
 *		Revision 1.8  1992/05/19  10:09:06  sterling
 *		made dowrite const, added ReadTag and WriteTag
 *
 *		Revision 1.7  92/02/18  03:52:46  03:52:46  lewis (Lewis Pringle)
 *		Had to change type name of Version to SaveableVersion to avoid name conflict.
 *		
 *		Revision 1.6  1992/02/15  04:50:37  sterling
 *		added static Read/Write version routines
 *
 *
 */

#include	"Exception.hh"

#include	"Changeable.hh"


class	istream;
class	ostream;

class	Saveable : public virtual Changeable {
	public:
#if		qCFront_NestedTypesHalfAssed
		typedef	Int16	SaveableVersion;		// change name cuz of conflict with Version.hh - scoping should fix it, but not with 2.1 compilers!!!
#else
		typedef	Int16	Version;
#endif

	protected:
#if		qCFront_NestedTypesHalfAssed
		Saveable (SaveableVersion maxVersion = 1);			// abstract class
#else
		Saveable (Version maxVersion = 1);					// abstract class
#endif
		
	public:
#if		qCFront_NestedTypesHalfAssed
		static	const SaveableVersion	kBadVersion;
#else
		static	const	Version	kBadVersion;
#endif
		typedef	Int32	Tag;

		virtual ~Saveable ();		

		nonvirtual	void	DoRead (istream& from);
#if		qCFront_NestedTypesHalfAssed
		nonvirtual	void	DoWrite (ostream& to, int tabCount = 0, SaveableVersion version = kBadVersion) const;
#else
		nonvirtual	void	DoWrite (ostream& to, int tabCount = 0, Version version = kBadVersion) const;
#endif

#if		qCFront_NestedTypesHalfAssed
		nonvirtual	SaveableVersion	GetVersion () const;
		nonvirtual	void			SetVersion (SaveableVersion version);

		nonvirtual	SaveableVersion	GetMaxVersion () const;
		nonvirtual	void			SetMaxVersion (SaveableVersion version);
#else
		nonvirtual	Version			GetVersion () const;
		nonvirtual	void			SetVersion (Version version);

		nonvirtual	Version			GetMaxVersion () const;
		nonvirtual	void			SetMaxVersion (Version version);
#endif

		nonvirtual	Boolean	VersionUpToDate ();
		
#if		qCFront_NestedTypesHalfAssed
		static	SaveableVersion	ReadVersion (SaveableVersion maxVersion, istream& from);
		static	void			WriteVersion (SaveableVersion version, ostream& to, int tabCount = 0);
#else
		static	Version			ReadVersion (Version maxVersion, istream& from);
		static	void			WriteVersion (Version version, ostream& to, int tabCount = 0);
#endif

		// can be used for file verification, writes out a unique tag (one unique tag per object)
		// usage: 
		//	in write code: WriteTag, write other stuff, WriteTag
		//  in read code: ReadTag, read other stuff, ReadTag, Assert (Tag1 == Tag2)
		nonvirtual	Tag		ReadTag (istream& from);
		nonvirtual	void	WriteTag (ostream& to, int tabCount = 0) const;

		static	Exception	sFileFormatTooNew;
		static	Exception	sBadVersion;

	protected:
		virtual	void	DoRead_ (istream& from);
		virtual	void	DoWrite_ (ostream& to, int tabCount) const;

#if		qCFront_NestedTypesHalfAssed
		virtual	void	SetVersion_ (SaveableVersion version);
		virtual	void	SetMaxVersion_ (SaveableVersion version);
#else
		virtual	void	SetVersion_ (Version version);
		virtual	void	SetMaxVersion_ (Version version);
#endif
		
	private:
#if		qCFront_NestedTypesHalfAssed
		SaveableVersion	fVersion;
		SaveableVersion	fMaxVersion;
#else
		Version	fVersion;
		Version	fMaxVersion;
#endif
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

#if		qCFront_NestedTypesHalfAssed
inline	SaveableVersion		Saveable::GetVersion () const		{ 	return (fVersion); 		}
inline	SaveableVersion		Saveable::GetMaxVersion () const	{ 	return (fMaxVersion); 	}
#else
inline	Saveable::Version	Saveable::GetVersion () const		{ 	return (fVersion); 		}
inline	Saveable::Version	Saveable::GetMaxVersion () const	{ 	return (fMaxVersion); 	}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif /*__Saveable__*/
