/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PathNameSelectors__
#define	__PathNameSelectors__

/*
 * $Header: /fuji/lewis/RCS/PathNameSelecters.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PathNameSelecters.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/07/14  19:48:35  lewis
 *		Lots of changes here - needless repetition of =Nils removed. template
 *		friendly usage of Set_BitString<FileType>. Also, use Set<T> whenever possible
 *		instead of Set_Bitstring () in public interface.
 *		Have pose not return Boolean any longer - raise exception. (NOT WELL DONE WHICH
 *		ONE - referenced extern in Document.cc - must come up with our own!!!).
 *		Changes for FileType changes in File.hh.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/03/09  23:55:03  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *
 *
 */

#include	"File.hh"

#include	"Config-Framework.hh"




class	AbstractNewPathNameGetter {
	protected:
		AbstractNewPathNameGetter ();

	public:
#if		qRealTemplatesAvailable
		virtual	const AbSet<FileSystem::FileType>&	GetFileTypes () const						=	Nil;
		virtual	void								SetFileTypes (const AbSet<FileSystem::FileType>&
														fileTypes)								=	Nil;
#else
		virtual	const AbSet(FileTypeX)&				GetFileTypes () const						=	Nil;
		virtual	void								SetFileTypes (const AbSet(FileTypeX)&
														fileTypes)								=	Nil;
#endif
	public:
		virtual	PathName				GetPathName () const									=	Nil;
		virtual	void					SetPathName (const PathName& pathName)					=	Nil;

	/*
	 * Raise an exception on user cancels.
	 */
	public:
		virtual	void	Pose ()																	=	Nil;
};



class	AbstractOldPathNameGetter {
	protected:
		AbstractOldPathNameGetter ();

	public:
#if		qRealTemplatesAvailable
		virtual	const AbSet<FileSystem::FileType>&	GetFileTypes () const						=	Nil;
		virtual	void								SetFileTypes (const AbSet<FileSystem::FileType>&
														fileTypes)								=	Nil;
#else
		virtual	const AbSet(FileTypeX)&				GetFileTypes () const						=	Nil;
		virtual	void								SetFileTypes (const AbSet(FileTypeX)&
														fileTypes)								=	Nil;
#endif

	public:
		virtual	PathName				GetPathName () const									=	Nil;
		virtual	void					SetPathName (const PathName& pathName)					=	Nil;

	/*
	 * Used to filter out files you don't want to see. Typically one might override this
	 * to elimitate all files but text files, or something like that.
	 */
	public:
		virtual	Boolean	ShowFile (const PathName& pathName)										=	Nil;

	/*
	 * Raise an exception on user cancels.
	 */
	public:
		virtual	void	Pose ()																	=	Nil;
};



class	NewPathNameGetter_MacUI : public AbstractNewPathNameGetter {
	protected:
		NewPathNameGetter_MacUI ();
};



class	OldPathNameGetter_MacUI : public AbstractOldPathNameGetter {
	protected:
		OldPathNameGetter_MacUI ();
};



#if		qMacToolkit
// need support for getting at message name, and sftypelist fields...
class	NewPathNameGetter_MacUI_Native : public NewPathNameGetter_MacUI {
	public:
#if		qRealTemplatesAvailable
		NewPathNameGetter_MacUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName);
#else
		NewPathNameGetter_MacUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName);
#endif

	public:
#if		qRealTemplatesAvailable
		override	const AbSet<FileSystem::FileType>&	GetFileTypes () const;
		override	void								SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes);
#else
		override	const AbSet(FileTypeX)&				GetFileTypes () const;
		override	void								SetFileTypes (const AbSet(FileTypeX)& fileTypes);
#endif

	public:
		override	PathName	GetPathName () const;
		override	void		SetPathName (const PathName& pathName);

	public:
		override	void		Pose ();

	public:
		nonvirtual	String		GetMessage () const;
		nonvirtual	void		SetMessage (const String& message);

	private:
#if		qRealTemplatesAvailable
		Set_BitString<FileSystem::FileType	fFileTypes;
#else
		Set_BitString(FileTypeX)			fFileTypes;
#endif
		PathName							fPathName;
		String								fMessage;
};


class	OldPathNameGetter_MacUI_Native : public OldPathNameGetter_MacUI {
	public:
#if		qRealTemplatesAvailable
		OldPathNameGetter_MacUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName);
#else
		OldPathNameGetter_MacUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName);
#endif

#if		qRealTemplatesAvailable
		override	const AbSet<FileSystem::FileType>&	GetFileTypes () const;
		override	void								SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes);
#else
		override	const AbSet(FileTypeX)&				GetFileTypes () const;
		override	void								SetFileTypes (const AbSet(FileTypeX)& fileTypes);
#endif

	public:
		override	PathName				GetPathName () const;
		override	void					SetPathName (const PathName& pathName);

	public:
		override	void	Pose ();

	public:
		nonvirtual	String					GetMessage () const;
		nonvirtual	void					SetMessage (const String& message);

	/*
	 * Used to filter out files you don't want to see. Typically one might override this
	 * to elimitate all files but text files, or something like that.
	 */
	public:
		override	Boolean	ShowFile (const PathName& pathName);

	private:
#if		qRealTemplatesAvailable
		Set_BitString<FileSystem::FileType	fFileTypes;
#else
		Set_BitString(FileTypeX)			fFileTypes;
#endif
		PathName							fPathName;
		String								fMessage;


		static	pascal	unsigned char	FileFilterProc (struct ParamBlockRec* paramBlockPtr);
		static	OldPathNameGetter_MacUI_Native*	sCurrenGetter;

		// code for directory selector
		static	pascal	short		FoldersOnly_GetDirHook (short item, struct osGrafPort* dialogPtr);
};

#endif	/*qMacToolkit*/



class	NewPathNameGetter_MotifUI : public AbstractNewPathNameGetter {
	protected:
		NewPathNameGetter_MotifUI ();
};



class	OldPathNameGetter_MotifUI : public AbstractOldPathNameGetter {
	protected:
		OldPathNameGetter_MotifUI ();
};


#if		qXmToolkit
// need support for getting at message name, and sftypelist fields...
class	NewPathNameGetter_MotifUI_Native : public NewPathNameGetter_MotifUI {
	public:
#if		qRealTemplatesAvailable
		NewPathNameGetter_MotifUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName);
#else
		NewPathNameGetter_MotifUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName);
#endif

	public:
#if		qRealTemplatesAvailable
		override	const AbSet<FileSystem::FileType>&	GetFileTypes () const;
		override	void								SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes);
#else
		override	const AbSet(FileTypeX)&				GetFileTypes () const;
		override	void								SetFileTypes (const AbSet(FileTypeX)& fileTypes);
#endif

	public:
		override	PathName	GetPathName () const;
		override	void		SetPathName (const PathName& pathName);

	public:
		override	void		Pose ();

	public:
		nonvirtual	String		GetMessage () const;
		nonvirtual	void		SetMessage (const String& message);

	private:
#if		qRealTemplatesAvailable
		Set_BitString<FileSystem::FileType	fFileTypes;
#else
		Set_BitString(FileTypeX)			fFileTypes;
#endif
		PathName							fPathName;
		String								fMessage;
};


class	OldPathNameGetter_MotifUI_Native : public OldPathNameGetter_MotifUI {
	public:
#if		qRealTemplatesAvailable
		OldPathNameGetter_MotifUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName);
#else
		OldPathNameGetter_MotifUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName);
#endif

	public:
#if		qRealTemplatesAvailable
		override	const AbSet<FileSystem::FileType>&	GetFileTypes () const;
		override	void								SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes);
#else
		override	const AbSet(FileTypeX)&				GetFileTypes () const;
		override	void								SetFileTypes (const AbSet(FileTypeX)& fileTypes);
#endif

		override	PathName				GetPathName () const;
		override	void					SetPathName (const PathName& pathName);

	public:
		override	void	Pose ();

	public:
		nonvirtual	String					GetMessage () const;
		nonvirtual	void					SetMessage (const String& message);

	public:
		/*
		 * Used to filter out files you don't want to see. Typically one might override this
		 * to elimitate all files but text files, or something like that.
		 */
		override	Boolean	ShowFile (const PathName& pathName);

	private:
#if		qRealTemplatesAvailable
		Set_BitString<FileSystem::FileType	fFileTypes;
#else
		Set_BitString(FileTypeX)			fFileTypes;
#endif
		PathName							fPathName;
		String								fMessage;

};

#endif	/*qXmToolkit*/





#if		qMacToolkit
typedef	NewPathNameGetter_MacUI_Native		NewPathNameGetter;
typedef	OldPathNameGetter_MacUI_Native		OldPathNameGetter;
#endif

#if		qXmToolkit
typedef	NewPathNameGetter_MotifUI_Native		NewPathNameGetter;
typedef	OldPathNameGetter_MotifUI_Native		OldPathNameGetter;
#endif	/*qXmToolkit*/



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__PathNameSelectors__*/

