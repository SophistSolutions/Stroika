/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__File__
#define	__File__

/*
 * $Header: /fuji/lewis/RCS/File.hh,v 1.32 1992/12/08 21:15:02 lewis Exp $
 *
 * Description:
 *		A PathComponent is a portion of a path name to a given file, in a
 *	file system. It could be, a simple file name. A sequence of PathComponents
 *	specify a PathName. A PathName, can be used to uniquely specify a File,
 *	in a FileSystem. Stroika allows for multiple FileSystems, but mainly you
 *	are expected to access the current one with FileSystem::Get ().
 *
 *		PathNames are by default intereptted as ROOT RELATIVE. This can be
 *	overriden by simply making the first pathname component a
 *	CurrentDirectoryPathComponent. This is different from UNIX, and perhaps
 *	slightly differnent from the mac (mac is weird), and different from DOS.
 *	So, the decision deserves an explanation.
 *
 *		Clearly when interpretting a PathName we need some cue to know that
 *	it is to be inpretted as root-relative. In UNIX/DOS they count on
 *	starting with the character '\' or '/' to do this. But alas, there is
 *	no such character for the mac where you can have multiple roots (sort of
 *	true with DOS too). So the alternative is to introduce some new
 *	class/token kRootStart as the portable way to start an absolute path.
 *	But we already have such a token for current-directory-relative
 *	(PathName::kDot). Not sure if that was convincing, but thats how it is...
 *
 *
 *
 * TODO:
 *		+	File permisions is currently a crock
 *
 *		+	No real support for locking, of files - flock().
 *
 *		+	Tom wants some support for once the file is opened - some abstraction for
 *			open file access path???/ easier read/write and maybe binary read write support?
 *
 *		+	Add to AbstractFileSystem:
 *			-		parsestringtopathnae
 *			-		get separetor
 *			-		get maxfilenamelen
 *			-		get maxpathlen
 *
 *		+	Note change in GetSize/GetDataForkSize ():
 *			be careful to search for code that depended on this?? At least warning in release notes that it changed.
 *			GetSize (const PathName& pathName) const;
 *
 *		+	For UNIX filesystem, add owner/group getter/setter. Be careful to first add uid_t, or whatever
 *			posix says is right name. Also, review stat struct is Posix docs, and see what else we are
 *			missing of value.
 *
 *		+	Study giving better support for NTFS (Windows NT file system).
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: File.hh,v $
 *		Revision 1.32  1992/12/08  21:15:02  lewis
 *		Add preliminary support for qWinNT - enuf to compile.
 *
 *		Revision 1.31  1992/12/05  17:33:19  lewis
 *		Add  || qWinOS in a few references to qDOS.
 *
 *		Revision 1.30  1992/11/26  02:45:47  lewis
 *		Moved CurrentDirectoryPathComponent and ParentDirectoryPathComponent
 *		to .hh file so they can be used to initialize globals - not sure
 *		that was the best choice?
 *		
 *
 *		Revision 1.29  1992/11/23  21:22:43  lewis
 *		Return Iterator<T> instead of IteratorRep<T>*.
 *
 *		Revision 1.28  1992/11/16  04:42:58  lewis
 *		Get rid of qDontNestEnumUsedInTemplate, and the workarounds for
 *		this bug.
 *
 *		Revision 1.27  1992/11/12  08:04:10  lewis
 *		Disable ever setting qDontNestEnumUsedInTemplate. May no longer
 *		be needed???
 *		Also, fixup template version of constant sets. Still not working
 *		with templates.
 *
 *		Revision 1.26  1992/10/16  22:48:00  lewis
 *		Get rid of externs in declarations. Added op==/!= for PathComponents
 *		since otherwise PathComp a,b; a==b built a pathname, which then
 *		iterated over its components comparing them, yielding an infinite loop.
 *
 *		Revision 1.25  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.24  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.23  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.22  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.21  1992/10/07  22:08:37  sterling
 *		Conditionally use new GenClass stuff instead of declare marcos.
 *
 *		Revision 1.20  1992/09/26  22:25:41  lewis
 *		Added PathName        operator+ (const PathName& lhs, const PathName& rhs).
 *
 *		Revision 1.19  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.18  1992/09/15  17:02:04  lewis
 *		Got rid of AbSet, and made changes here to be compatable with container
 *		class changes. This evoked a GCC 2.2.1 bug - still not fully tracked
 *		down - and now just labeled here wtih qDontNestEnumUsedInTemplate.
 *
 *		Revision 1.17  1992/09/11  15:46:04  sterling
 *		used new Shared implementation
 *
 *		Revision 1.16  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.15  1992/09/03  19:33:50  lewis
 *		Added += operators for PathName for appending PathComponents,
 *		and added iterator operators for easy iteration over components.
 *
 *		Revision 1.13  1992/07/21  06:28:11  lewis
 *		Got rid of Sequence/DoubleLinkList stuff for PathName - unused.
 *		Make SequenceIterator return valuies from MakeMountedVolumeIterator and
 *		MakeDirectoryIterator all const.
 *
 *		Revision 1.12  1992/07/16  13:29:15  lewis
 *		Made PathComponent (PathComponent&) CTOR out-of-line, since used
 *		in declare macro before declared inline... Could have declared inline
 *		in class declaration - but I don;t like todo that. Same for op=.
 *
 *		Revision 1.11  1992/07/16  06:32:51  lewis
 *		Added PathComponent (const PathComponent& from); and op= because BCC
 *		seemed very unhappy without them (using templates - had to give up anyhow
 *		since that appeared to be the tip of the iceburg).
 *		Also, added ifdef for qLongCharacterConstantsSupported to say ???? or
 *		associated # for creator/type args to createfile functions.
 *
 *		Revision 1.10  1992/07/16  05:09:19  lewis
 *		Add new overload of op+: PathName        operator+ (const PathComponent&, const PathComponent&);
 *		Move various subclasses of PathComponent into the .cc file, and added overloaded PathName
 *		CTORs for wdrefnums, vrefnums etc. Also, moved typedefs for these types into scope pathName.
 *
 *		Revision 1.9  1992/07/14  23:57:23  lewis
 *		Removed extral declaration of virtual void CreateDirectory_ (const PathName&) in
 *		FileSystem_UnixOS. Was only caught by gcc.
 *
 *		Revision 1.8  1992/07/14  19:39:12  lewis
 *		Lots of work -- cleaning up const usage, adding FileSystem::Get_XXXOS methods,
 *		adding GetVisible/etc to Dos filesystem, and getting rid of mode_t stuff there,a
 *		and using mode_t more properly for UNIX file systems.
 *		Most importantly, got working well enuf for PCXLate to work (at least limp
 *		along).
 *
 *		Revision 1.7  1992/07/10  23:36:48  lewis
 *		Fix off_t to be a long - and fix SetCreationTime_ to be non-const.
 *
 *		Revision 1.6  1992/07/10  22:26:38  lewis
 *		Almost totally rewrote. No more global functions like open - instead
 *		use FileSystem::Get ().Open(). Got rid of unixy names, and use more
 *		Stroika like names (capitalized, and dont loose vowels).
 *		Got rid of explict classes for Directory/Volumne iterators - use MakeIterator
 *		methods of AbstractFileSystem instead.
 *		Changed defintion of GetSize () to return total size, intead of data fork size
 *		for the mac.
 *		Actually that may have been a mistake - I think I will revert it.
 *		Pathnames no longer generate exceptions when appending them - only when
 *		passed along to a filesystem to they get validated.
 *		New PathComponent family of classes - they should be scoped with PathName but
 *		for CFront 21 bugs.
 *		Added support for creation/modification dates, etc...
 *		Defined iostream operators for PathName - though probably not very well.
 *		Separate class hierarchies for different file systems.
 *		(That should make kdj happy).
 *		Lots more - oh yeah - and broke it - barely works on mac, and nowhere else.
 *		Much work to be done to actually get it all working again - though one of the
 *		main reasons I rewrote it, was that it was just barely working, and was very
 *		bug ridden.
 *
 *		Revision 1.3  1992/06/28  03:57:48  lewis
 *		Add ||qGCC to some qCFront21 hacks - clean this stuff up soon.
 *
 *		Revision 1.19  92/03/30  13:23:50  13:23:50  lewis (Lewis Pringle)
 *		Ported to BorlandC++ and added ifdefs for qRealTemplatesAvaialble.
 *		
 *		Revision 1.17  1992/02/08  04:30:55  lewis
 *		cleanups for UNIX of includes, etc, and added GetFileVisible for MacOS (tom kents request).
 *
 *		Revision 1.16  1992/01/29  16:43:52  lewis
 *		Added access permission denied exception.
 *
 *		Revision 1.15  1992/01/13  15:46:17  lewis
 *		Did hack to get rid of pathname::FileType scoping warnings with 2.1 based compilers.
 *
 */
#include	<fcntl.h>		// for open, O_RDWR, etc, prototypes/defines
#if		qUnixOS
	#include	<unistd.h>		// for close prototype...
	#include	<sys/types.h>	// for off_t
#endif

#include	"DateTime.hh"
#include	"Exception.hh"
#include	"Shared.hh"
#include	"Sequence.hh"
#include	"Set_BitString.hh"
#include	"String.hh"





// Not quite sure what #defines to test/set - both of these should be done
// in a POSIX compliant sys/types.h
#if		!defined (_OFF_T) && !defined (__off_t__)
#define  _OFF_T 
#define  __off_t__
typedef	long	off_t;
#endif

#if		!defined (_MODE_T) && !defined (__mode_t__)
#define  _MODE_T 
#define  __mode_t__
typedef	unsigned	short	mode_t;
#endif

#if		qBorlandCPlus
extern "C" int close (int);	// hack cuz doesn't get defined otherwise -
							// see where it is supposed to be...
#endif



#if		qMacOS
struct MacOS_ExtendedParamBlockHeader;
#endif


	// sb nested in PathComponent except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	// AND CUZ OF LACK OF REAL TEMPLATES...
	class	PathComponentRep {
		protected:
			PathComponentRep ();
	
		public:
			virtual ~PathComponentRep ();
	
		/*
		 * Get some printed representation of the component. Not necessary pretty, and
		 * not necessarily the same as what will be handed to the OS.
		 */
		public:
			virtual	String	GetName () const										=	Nil;
	
		/*
		 * This should generally not be used. This is the low level support the filesystem
		 * uses to generate structures that the OS will recognize to access the file,
		 * refered to by the PathName, which this PathComponent is a part.
		 */
		public:
	#if		qMacOS
			virtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
																					=	Nil;
	#elif	qDOS || qWinOS || qWinNT || qUnixOS
			virtual	void	ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const
																					=	Nil;
	#endif
	};

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfPathComponentRep_hh
#endif


class	PathComponent {
	public:
		PathComponent (const PathComponent& from);
		PathComponent (const String& name);
		PathComponent (const char* name);
		PathComponent ();

	protected:
		PathComponent (PathComponentRep* pathComponentRep);

	public:
		const PathComponent& operator= (const PathComponent& rhs);

	public:
		nonvirtual	String	GetName () const;

	public:
#if		qMacOS
		nonvirtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
#elif	qDOS || qWinNT || qWinOS || qUnixOS
		nonvirtual	void	ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const;
#endif

	private:
#if		qRealTemplatesAvailable
		Shared<PathComponentRep>	fRep;
#else
		Shared(PathComponentRep)	fRep;
#endif
};
Boolean	operator== (const PathComponent& lhs, const PathComponent& rhs);
Boolean	operator!= (const PathComponent& lhs, const PathComponent& rhs);

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SequenceOfPathComponent_hh
#endif


class	CurrentDirectoryPathComponent : public PathComponent {
	public:
		CurrentDirectoryPathComponent ();
};

class	ParentDirectoryPathComponent : public PathComponent {
	public:
		ParentDirectoryPathComponent ();
};











/*
 * A PathName is what you are most likely to instantiate, and keep. It is a name of a file,
 * in some filesystem. It is not necessary unique, or systactic, or pointing to an existing
 * file. These questions can be answered by posing appropriate queries of a FileSystem.
 *
 * Use of a PathName, including building, examining, and appending/prepending should cause
 * no excpetions, except possibly running out memory. Exceptions for bad formatting
 * come from use with respect to filesystem. (GetFileName () can return exception if no file
 * name - ie no components).
 */

class	PathName {
	public:
		// allow 1 or more path components to be specified directly...
		PathName (const PathComponent& p1);
		PathName (const PathComponent& p1, const PathComponent& p2);
		PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3);
		PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3,
			const PathComponent& p4);
		PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3,
			const PathComponent& p4, const PathComponent& p5);
		PathName (const PathName& pathName);
		PathName ();

		// allow prepending, appending of PathComponents, or a named sequence.
		PathName (const PathComponent& pathComponent, const PathName& pathName);
		PathName (const PathName& pathName, const PathComponent& pathComponent);
#if		qRealTemplatesAvailable
		PathName (const Sequence <PathComponent>& pathComponents);
#else
		PathName (const Sequence (PathComponent)& pathComponents);
#endif

	/*
	 * These 5 constructors are really only useful under the MacOS, but are included otherwise
	 * so that file/io and overloading can work the same across machines.
	 */
	public:
		typedef	short	WDRefNum;
		typedef	short	VRefNum;
		typedef	long	DirID;
#if		qMacOS
		PathName (const struct osFSSpec& itsFSSpec);
#endif	/*qMacOS*/
		PathName (WDRefNum wdRefNum);
		PathName (WDRefNum wdRefNum, const String& pathName);
		PathName (VRefNum vRefNum, DirID dirID);
		PathName (VRefNum vRefNum, DirID dirID, const String& pathName);


	public:
		nonvirtual	const PathName& operator= (const PathName& rhs);


	/*
	 * Access to the components. First some simple, common questions, and then general
	 * access.
	 */
	public:
		nonvirtual	size_t			GetLength () const;		// # components
		nonvirtual	PathName		GetPrefix () const;		// All components but first - always safe... Can return kBad
		nonvirtual	PathComponent	GetFileName () const;	// Last component - usualy file name - exception if none...

#if		qRealTemplatesAvailable
// Only bother with iterator version cuz of bug with gcc 2.2.1 - and possibly CFront 2.1 also - investigate...
	 	nonvirtual	operator Iterator<PathComponent> () const;
	 	nonvirtual	operator SequenceIterator<PathComponent> () const;
#else
	 	nonvirtual	operator Iterator(PathComponent) () const;
	 	nonvirtual	operator SequenceIterator(PathComponent) () const;
#endif

		nonvirtual	const PathName& operator+= (const PathComponent& rhs);
		nonvirtual	const PathName& operator+= (const PathName& rhs);

#if		qRealTemplatesAvailable
		const	Sequence<PathComponent>&	GetComponents () const;
#else
		const	Sequence(PathComponent)&	GetComponents () const;
#endif

	public:
		static	const	PathName		kBad;
		static	const	PathComponent	kDot;		// curDir relative (Unix ".")
		static	const	PathComponent	kDotDot;	// up one dir from current (Unix "..")


	/*
	 * Not intended for public consumption, except in those rare cases where you must write non-portable
	 * code. This family of functions is intended to convert a Stroika pathname into one your OS
	 * will understand. It is used internally in the filesystem code, and can be used by you, if
	 * you wish to make calls we don't support of your OS's filesystem.
	 */
	public:
#if		qMacOS
		nonvirtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
		nonvirtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								struct WDPBRec& paramBlkHeader) const;
		nonvirtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								struct HParamBlockRec& paramBlkHeader) const;
		nonvirtual	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
								union CInfoPBRec& paramBlkHeader) const;
#elif	qDOS || qWinNT || qWinOS || qUnixOS
		nonvirtual	void	ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const;
#endif

	private:
#if		qRealTemplatesAvailable
		Sequence<PathComponent>	fPathElements;
#else
		Sequence(PathComponent)	fPathElements;
#endif
};




/*
 * This does no checking for valididty with respect to any file system. It simply contatenates
 * components. Interpretation of the pathnames is done thru the FileSystem object.
 */
PathName	operator+ (const PathName& lhs, const PathName& rhs);
PathName	operator+ (const PathName& pathName, const PathComponent& pathComponent);
PathName	operator+ (const PathComponent& pathComponent, const PathName& pathName);
PathName	operator+ (const PathComponent& pathComponent1, const PathComponent& pathComponent2);




/*
 * Must be clear about what these mean - I'm not too sure. Tough to do
 * good job because of multiple polymorphism.
 *
 * For now just define in terms of String (NAME) equality of the components.
 */
Boolean	operator== (const PathName& lhs, const PathName& rhs);
Boolean	operator!= (const PathName& lhs, const PathName& rhs);




/*
 * Convert the given pathname to String format and print it that way. For the extracter
 * read a string (using operator>> (istream&, String&)) and convert it to a pathName with
 * the current file system. Note that these operations are not necessarily symetric - 
 * writing something out, and then reading it back is not gauranteed to work.
 */
class	istream;
class	ostream;
istream&	operator>> (istream& in, PathName& pathName);
ostream&	operator<< (ostream& out, const PathName& pathName);





/************ From here on, we are talking about FileSystems and using a PathName wrto a FileSystem
 				can cause exceptions ***************/




	// sb nested in AbstractFileSystem except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	FileException : public Exception {
		public:
			FileException ();
			FileException (const FileException& isAnException);
	
			override	void	Raise ();
	
			nonvirtual	void	Raise (int osErr);
	
			nonvirtual	int		GetErrNumber () const;
	
		private:
			int	fErr;
	};


	// sb nested in AbstractFileSystem except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	FileNameBadCharacter : public FileException {
		public:
			FileNameBadCharacter ();
			FileNameBadCharacter (const FileException& isAnException);
	
			override	void	Raise ();
	
			nonvirtual	void	Raise (Character badChar);
		private:
			Character	fBadChar;
	};


class	AbstractFileSystem {
	/*
	 * Abstract class. This just defines the interface to all file systems.
	 */
	protected:
		AbstractFileSystem ();


	/*
	 * Validate () will take the given pathname and validate that it is legitimate for
	 * use with this file system. It will simply return if all is well. Otherwise, it will
	 * throw an exception to indicate what is wrong with the given pathname (eg. illegal characters
	 * for this filesystem). This can be used even if this filesystem is not current.
	 */
	public:
		nonvirtual	void	Validate (const PathName& pathName) const;


	/*
	 * ParseStringToPathName () will take the given string, and parse it for the given
	 * filesystem into a valid, portable (as much as possible) PathName. Instead of
	 * saying:
	 *		PathName ("/tmp")
	 * say
	 *		AbstractFileSystem_UnixOS ().ParseStringToPathName ("/tmp")
	 *
	 *		This extra work will yeild you a valid PathName regardless of what system you
	 * have built for (IE this method is gaurenteed to work properly regardless of the
	 * OS you are running).
	 *
	 *		The name it returns is NOT gauranteed to pass validation, since Validate
	 * may check for system defined MAXPATHLEN only when really running under that OS
	 * (because with pathconf/POSIX only possible then) and so it might not be valid,
	 * but generally should be.
	 */
	public:
		nonvirtual	PathName	ParseStringToPathName (const String& pathName) const;


	/*
	 * PathNameToString () will take the given pathName and generate a string which
	 * should look proper when displayed to a user for that particular system. It
	 * is not gauranteed that calling ParseStringToPathName () with the result of this
	 * function will produce the original pathname. I'm not even sure its likely.
	 * This is mainly for being able to display in textual form the given pathname.
	 * It is not intended for use in saving pathNames.
	 */
	public:
		nonvirtual	String	PathNameToString (const PathName& pathName) const;


	/*
	 * Expand the given PathName into a Fully Resolved PathName, for the given filesystem. This
	 * can fail, for a number of reasons, including references to PathComponents that don't
	 * resolve. This is generally a bad idea to do, except possibly as a display/diagnostic.
	 * Once a PathName is expanded to its Full Path Name, it is unlikely to be usable on
	 * other computers, and is VERY unlikely to work accross platforms.
	 */
	public:
		nonvirtual	PathName	GetFullPathName (const PathName& pathName) const;


	/*
	 * Return the type of file the given pathname corresponds to in this file system. This
	 * can raise an exception if it fails.
	 */
	public:
		enum FileType {
			eBadFileType,
			ePlainFileType,
			eDirectoryFileType,
			eSymbolicLink,			//	aka alias
			eSpecialFileType,		//	things like /dev/null, or on the mac .Ain
		};

		nonvirtual	FileType	GetFileType (const PathName& pathName) const;


	/*
	 * Get the size of the given named file (on mac = data fork + resource fork).
	 */
	public:
		nonvirtual	off_t		GetSize (const PathName& pathName) const;


	/*
	 * Return True if the given pathName exists in the filesystem, and false if it does not.
	 * This can still raise exceptions under a number of circumstances, like IO/Error,
	 * bad characters in pathname etc. It will not fail, and return False, if some pathname
	 * component is syntactic, but does not exist.
	 */
	public:
		nonvirtual	Boolean		Exists (const PathName& pathName) const;


	/*
	 * GetLastModificationTime () returns the DateTime representing the last time the file
	 * named by the given pathName was modified. All the filesystems we support, have this
	 * capability. Interestingly, it is the only Date Stamp capability they all share!
	 */
	public:
		nonvirtual	DateTime	GetLastModificationTime (const PathName& pathName) const;


	/*
	 * Create a file at the given pathname. This will fail if permisions are insuffient,
	 * if some prefixing portion of the pathname is invalid (or does not exist),
	 * IO errors, and perhaps more. Really, you should feel lucky if this works.
	 */
	public:
		nonvirtual	void		Create (const PathName& pathName);



	/*
	 * Create a directory at the given pathname. This will fail if permisions are insuffient,
	 * if some prefixing portion of the pathname is invalid (or does not exist),
	 * IO errors, etc...
	 */
	public:
		nonvirtual	void		CreateDirectory (const PathName& pathName);


	/*
	 * Delete the file/directory specified by the given pathName. This can fail for a host
	 * of system depended reasons, including if the directory is non-empty (assuming thats how
	 * the underly os works).
	 *
	 *
	 * NB: Even in UNIX, this function will delete both files, and directories.
	 */
	public:
		nonvirtual	void		Delete (const PathName& pathName);


	/*
	 * Rename the file named by "from", to "to". Some filesystems (e.g. UNIX) place
	 * restrictions on what files can be renamed across directories that are not
	 * immediately obvious, or natural (eg in UNIX not obvious of two directories
	 * are on the same filesystem, and rename does not work across filesystems).
	 *
	 * We try to use the underlying rename facility, but if this fails, the file will
	 * be copied, and the original deleted. Of course if the copy fails, the original
	 * will not be touched. In the event of a copy, an attempt will be made to presever
	 * the rename semantics as much as possible, so if the copy succeeds, and the delete
	 * of the original file fails, we will (try to) delete the new file we just copied,
	 * and return an error.
	 *
	 *
	 * NB: This decsion has great ramifications in the case of directories. Maybe it
	 * was a bad idea for this reason... Maybe add new function to do this???
	 */
	public:
		nonvirtual	void		Rename (const PathName& from, const PathName& to);


	/*
	 * Return a PathName corresponding to the parent directory of the given
	 * pathName. If there is none, or it does not exist in this filesystem,
	 * or if the pathName is somehow malformed, an exception will be raised.
	 */
	public:
		nonvirtual	PathName	GetParentDirectory (const PathName& pathName) const;


	/*
	 *  	Returns the number of items contained in the directory named by pathName. If
	 * pathName does not refer to a directory, and exception will be raised. All entries
	 * in the directory are counted, including special ones (eg. ".", "..").
	<<< Not sure we should count these???>>>
	<<< Same question for iterators >>>
	 */
	public:
		nonvirtual	UInt32		CountContainedItems (const PathName& pathName) const;


	/*
	 * Build a Iterator<PathComponent> to retrieve all of top level file system
	 * starting points. On UNIX, this is simply "/". Under DOS, it might be a: c: d:.
	 * On the mac, it might be "Lucy:", "Linus:", "HD20:".
	 */
	public:
#if		qRealTemplatesAvailable
		nonvirtual	Iterator<PathComponent>	MakeMountedVolumeIterator () const;
#else
		nonvirtual	Iterator(PathComponent)	MakeMountedVolumeIterator () const;
#endif


	/*
	 * Build a Iterator<PathComponent> to retrieve all of the file names contained in
	 * the given directory. We could have returned PathNames to those files, but this was deemed
	 * more flexible. To get the pathhame for a particular element, just say "directory + it.Current ()".
	 */
	public:
#if		qRealTemplatesAvailable
		nonvirtual	Iterator<PathComponent>	MakeDirectoryIterator (const PathName& directory) const;
#else
		nonvirtual	Iterator(PathComponent)	MakeDirectoryIterator (const PathName& directory) const;
#endif



	/*
	 * Get/Set current directory. This is used in interpretting the CurrentDirectoryRelative 
	 * pathname component.
	 */
	public:
		nonvirtual	PathName	GetCurrentDirectory () const;
		nonvirtual	void		SetCurrentDirectory (const PathName& pathName);


	/*
	 * Get some standard directories.
	 */
	public:
		nonvirtual	PathName	GetTemporaryDirectory () const;
		nonvirtual	PathName	GetPreferencesDirectory () const;




	/*
	 * File opening/closing. Creating accesspaths.
	 *
	 *		We just document the typedef of FileDescriptor to int. We do no object
	 * wrapper for this for now. It will be difficult, because so much of the software
	 * we count on assumes this already - eg stdio, and iostream lib.
	 *
	 * 		<< this area needs lots of work >>
	 */
	public:
		typedef	int	FileDescriptor;
		// openModes defined by ANSI C open???
		nonvirtual	FileDescriptor	Open (const PathName& pathName, int openModes);



	/*
	 * File system excpetions
	 */
	public:
	    static	FileException			sException;
	    static	FileException			sOperationNotSupported;
	    static	FileException			sPathComponentNotSupported;
		static	FileException			sFileNotFound;
		static	FileException			sTooManyFilesOpen;
		static	FileException			sFileExists;
		static	FileException			sFileNameTooLong;
		static	FileException			sAccessPermissionDenied;
		static	FileNameBadCharacter	sFileNameBadCharacter;
		static	FileException			sDirectoryOperationOnNonDirectory;
		static	FileException			sPlainFileOperationOnDirectory;


	/*
	 * Concrete subclasses override these to make us real...
	 */
	protected:
		virtual	void		Validate_ (const PathName& pathName) const					=	Nil;
		virtual	PathName	ParseStringToPathName_ (const String& pathName) const		=	Nil;
		virtual	String		PathNameToString_ (const PathName& pathName) const			=	Nil;
		virtual	PathName	GetFullPathName_ (const PathName& pathName) const			=	Nil;
		virtual	FileType	GetFileType_ (const PathName& pathName) const				=	Nil;
		virtual	off_t		GetSize_ (const PathName& pathName) const					=	Nil;
		virtual	Boolean		Exists_ (const PathName& pathName) const					=	Nil;
		virtual	DateTime	GetLastModificationTime_ (const PathName& pathName) const	=	Nil;
		virtual	void		Create_ (const PathName& pathName)							=	Nil;
		virtual	void		CreateDirectory_ (const PathName& pathName)					=	Nil;
		virtual	void		Delete_ (const PathName& pathName)							=	Nil;
		virtual	void		Rename_ (const PathName& from, const PathName& to)			=	Nil;
		virtual	PathName	GetParentDirectory_ (const PathName& pathName) const		=	Nil;
		virtual	UInt32		CountContainedItems_ (const PathName& pathName) const		=	Nil;
#if		qRealTemplatesAvailable
		virtual	Iterator<PathComponent>	MakeMountedVolumeIterator_ () const	
																						=	Nil;
		virtual	Iterator<PathComponent>	MakeDirectoryIterator_ (const PathName& directory) const
																						=	Nil;
#else
		virtual	Iterator(PathComponent)	MakeMountedVolumeIterator_ () const
																						=	Nil;
		virtual	Iterator(PathComponent)	MakeDirectoryIterator_ (const PathName& directory) const
																						=	Nil;
#endif
		virtual	PathName		GetCurrentDirectory_ () const							=	Nil;
		virtual	void			SetCurrentDirectory_ (const PathName& pathName)			=	Nil;
		virtual	PathName		GetTemporaryDirectory_ () const							=	Nil;
		virtual	PathName		GetPreferencesDirectory_ () const						=	Nil;
		virtual	FileDescriptor	Open_ (const PathName& name, int openModes)				=	Nil;
};








class	AbstractFileSystem_MacOS : public AbstractFileSystem {
	protected:
		AbstractFileSystem_MacOS ();

	/*
	 * Assuming we are a plain file.  Failure (sPlainFileOperationOnDirectory) otherwise.
	 */
	public:
		nonvirtual	off_t		GetDataForkSize (const PathName& pathName) const;
		nonvirtual	off_t		GetResourceForkSize (const PathName& pathName) const;


	/*
	 * SetLastModificationTime () allows you to change the modification date of the file
	 * named by the given pathName. If the date given is out of the range represented by
	 * this filesystem, the date will be pinned to the appropriate maxima/minima.
	 */
	public:
		nonvirtual	void		SetLastModificationTime (const PathName& pathName,
									const DateTime& modificationTime);


	/*
	 * Get/SetCreationTime () allows you to get/change the creation date of the file
	 * named by the given pathName. If the date given is out of the range represented by
	 * this filesystem, the date will be pinned to the appropriate maxima/minima.
	 */
	public:
		nonvirtual	DateTime	GetCreationTime (const PathName& pathName) const;
		nonvirtual	void		SetCreationTime (const PathName& pathName,
									const DateTime& creationTime);


	/*
	 * Return/set the 4 byte quantity refered to in the mac file system as the file creator.
	 * This is typically something like 'MPW ', or 'EXCL'. This is also sometimes refered to
	 * as the signature(?).
	 *
	 * Assumes we are a plain file.  Failure (sPlainFileOperationOnDirectory) otherwise.
	 */
	public:
		nonvirtual	UInt32		GetCreator (const PathName& pathName) const;
		nonvirtual	void		SetCreator (const PathName& pathName, UInt32 creator);


	/*
	 * Return/set the 4 byte quantity refered to in the mac file system as the file type.
	 * This is typically something like 'TEXT', 'APPL', or 'PICT'.
	 *
	 * Assumes we are a plain file.  Failure (sPlainFileOperationOnDirectory) otherwise.
	 */
	public:
		nonvirtual	UInt32		GetType (const PathName& pathName) const;
		nonvirtual	void		SetType (const PathName& pathName, UInt32 type);


	/*
	 * Get or set the file locked attribute for the given file. This corresponds to the locked
	 * status you can see/set in the finder GetInfo dialog (system 7).
	 *
	 * Assumes we are a plain file.  Failure (sPlainFileOperationOnDirectory) otherwise.
	 */
	public:
		nonvirtual	Boolean		GetLocked (const PathName& pathName) const;
		nonvirtual	void		SetLocked (const PathName& pathName, Boolean locked);


	/*
	 * Get or set the file visible attribute for the given file. This is respected by SFGetFile,
	 * and the finder (i think - I dont know what else).
	 */
	public:
		nonvirtual	Boolean		GetVisible (const PathName& pathName) const;
		nonvirtual	void		SetVisible (const PathName& pathName, Boolean visible);



	/*
	 * Check if the volume that the given pathName refers to is ejectable or not, and eject
	 * it (if Eject called). pathName need not refer directly to the mounted device which
	 * is ejectable - pointing someplace inside is good enuf. If pathName does not refer to
	 * a valid file, then we raise an exception (cuz other definition wouldn't work well
	 * with SLinks).
	 */
	public:
		nonvirtual	Boolean		Ejectable (const PathName& pathName);
		nonvirtual	void		Eject (const PathName& pathName);



	/*
	 * Get some standard directories.
	 */
	public:
		nonvirtual	PathName	GetBootVolume () const;
		nonvirtual	PathName	GetCurrentSFGetFileDirectory () const;
		nonvirtual	PathName	GetSystemDirectory () const;
		nonvirtual	PathName	GetDeskTopDirectory (const PathName& forVolume) const;




	/*
	 * File opening/closing. Creating accesspaths.
	 * 		<< this area needs lots of work >>
	 * FOR OPEN/CREAT add creator/type... optional params - also add OpenResFile for mac.
	 */
	public:
		nonvirtual	FileDescriptor	Open (const PathName& pathName, int openModes,

#if		qLongCharacterConstantsSupported
										UInt32 creator = '????', UInt32 fileType = '????');
#else
										UInt32 creator = 0x3f3f3f3ful, UInt32 fileType = 0x3f3f3f3ful);
#endif
		nonvirtual	void			Create (const PathName& pathName,
#if		qLongCharacterConstantsSupported
										UInt32 creator = '????', UInt32 fileType = '????');
#else
										UInt32 creator = 0x3f3f3f3ful, UInt32 fileType = 0x3f3f3f3ful);
#endif
		nonvirtual	short			OpenResFile (const PathName& name, int openModes);





	/*
	 * Return the volume associated with the file refered to by the given pathname.
	 * We only look at enuf of the initial part of the pathName to ascertain the volume.
	 * Raise an exception if this is not on a valid volume.
	 *
	 * NB: this does NOT return a wdRefNum.
	 */
	public:
		nonvirtual	short		GetVRefNum (const PathName& pathName) const;


	/*
	 * Get the DirID associated with the directory refered to by the given pathname.
	 * Raise an exception if the pathName does not refer to a valid directory.
	 */
	public:
		nonvirtual	long		GetDirID (const PathName& pathName) const;





	/*
	 * Some mac filesystem only exceptions...
	 */
	public:
		static	FileException			sFileLocked;
		static	FileException			sFileProjectorLocked;

		static	FileException			sResourceNotFound;
		static	FileException			sNoResourceFork;



	/*
	 * Concrete subclasses override these to make us real...
	 */
	protected:
		virtual	off_t			GetDataForkSize_ (const PathName& pathName) const					=	Nil;
		virtual	off_t			GetResourceForkSize_ (const PathName& pathName) const				=	Nil;
		virtual	void			SetLastModificationTime_ (const PathName& pathName, const DateTime& modificationTime) const
																									=	Nil;
		virtual	DateTime		GetCreationTime_ (const PathName& pathName) const					=	Nil;
		virtual	void			SetCreationTime_ (const PathName& pathName, const DateTime& creationTime)
																									=	Nil;
		virtual	UInt32			GetCreator_ (const PathName& pathName) const						=	Nil;
		virtual	void			SetCreator_ (const PathName& pathName, UInt32 creator)				=	Nil;
		virtual	UInt32			GetType_ (const PathName& pathName) const							=	Nil;
		virtual	void			SetType_ (const PathName& pathName, UInt32 type)					=	Nil;
		virtual	Boolean			GetLocked_ (const PathName& pathName) const							=	Nil;
		virtual	void			SetLocked_ (const PathName& pathName, Boolean locked)				=	Nil;
		virtual	Boolean			GetVisible_ (const PathName& pathName) const						=	Nil;
		virtual	void			SetVisible_ (const PathName& pathName, Boolean visible)				=	Nil;
		virtual	Boolean			Ejectable_ (const PathName& pathName)								=	Nil;
		virtual	void			Eject_ (const PathName& pathName)									=	Nil;
		virtual	PathName		GetBootVolume_ () const												=	Nil;
		virtual	PathName		GetCurrentSFGetFileDirectory_ () const								=	Nil;
		virtual	PathName		GetSystemDirectory_ () const										=	Nil;
		virtual	PathName		GetDeskTopDirectory_ (const PathName& forVolume) const				=	Nil;
		virtual	FileDescriptor	Open_ (const PathName& pathName, int openModes, UInt32 creator, UInt32 fileType)
																									=	Nil;
		virtual	void			Create_ (const PathName& pathName, UInt32 creator, UInt32 fileType)	=	Nil;
		virtual	short			OpenResFile_ (const PathName& name, int openModes)					=	Nil;
		virtual	short			GetVRefNum_ (const PathName& pathName) const						=	Nil;
		virtual	long			GetDirID_ (const PathName& pathName) const							=	Nil;

	/*
	 * Redirect Open_, and Create_ that were originally defined with 2/1 params to extended
	 * versions.
	 */
	protected:
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes);
		override	void			Create_ (const PathName& pathName);
};











class	AbstractFileSystem_UnixOS : public AbstractFileSystem {
	protected:
		AbstractFileSystem_UnixOS ();

	/*
	 * Get or set the file visible attribute for the given file. This is respected by SFGetFile,
	 * and the finder (i think - I dont know what else).
	 */
	public:
		nonvirtual	mode_t		GetPermissions (const PathName& pathName) const;
		nonvirtual	void		SetPermissions (const PathName& pathName, mode_t permissions);



	/*
	 * File opening/closing. Creating accesspaths.
	 * 		<< this area needs lots of work >>
	 * FOR OPEN/CREAT add creator/type... optional params.
	 */
	public:
		nonvirtual	FileDescriptor	Open (const PathName& pathName, int openModes, mode_t mode = 0666);
		nonvirtual	void			Create (const PathName& pathName, mode_t mode = 0666);
		nonvirtual	void			CreateDirectory (const PathName& pathName, mode_t mode = 0777);


	protected:
		virtual	mode_t			GetPermissions_ (const PathName& pathName) const					=	Nil;
		virtual	void			SetPermissions_ (const PathName& pathName, mode_t permissions)		=	Nil;


	protected:
		virtual	FileDescriptor	Open_ (const PathName& pathName, int openModes, mode_t mode)
																									=	Nil;
		virtual	void			Create_ (const PathName& pathName, mode_t mode)						=	Nil;
		virtual	void			CreateDirectory_ (const PathName& pathName, mode_t mode)			=	Nil;

	/*
	 * Redirect Open_, and Create_, and CreateDirectory_, that were originally defined
	 * with 2/1 params to extended versions.
	 */
	protected:
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes);
		override	void			Create_ (const PathName& pathName);
		override	void			CreateDirectory_ (const PathName& pathName);
};




class	AbstractFileSystem_DOS : public AbstractFileSystem {
	protected:
		AbstractFileSystem_DOS ();

	/*
	 * Get or set the file visible attribute for the given file. This is respected by SFGetFile,
	 * and the finder (i think - I dont know what else).
	 */
	public:
		nonvirtual	Boolean		GetVisible (const PathName& pathName) const;
		nonvirtual	void		SetVisible (const PathName& pathName, Boolean visible);


	/*
	 * Get or set the file locked attribute for the given file.
	 *
	 * Assumes we are a plain file.  Failure (sPlainFileOperationOnDirectory) otherwise.
	 */
	public:
		nonvirtual	Boolean		GetLocked (const PathName& pathName) const;
		nonvirtual	void		SetLocked (const PathName& pathName, Boolean locked);



	protected:
		virtual	Boolean			GetVisible_ (const PathName& pathName) const						=	Nil;
		virtual	void			SetVisible_ (const PathName& pathName, Boolean visible)				=	Nil;
		virtual	Boolean			GetLocked_ (const PathName& pathName) const							=	Nil;
		virtual	void			SetLocked_ (const PathName& pathName, Boolean locked)				=	Nil;
};









/*
 * Note that it IS legal to build a temporary FileSystem_MacOS, but there are no guarantees as to
 * what will work - typically just file name validation. But even this can be useful.
 */
class	FileSystem_MacOS : public AbstractFileSystem_MacOS {
	public:
		FileSystem_MacOS ();

	protected:
		override	void		Validate_ (const PathName& pathName) const;
		override	PathName	ParseStringToPathName_ (const String& pathName) const;
		override	String		PathNameToString_ (const PathName& pathName) const;
		override	PathName	GetFullPathName_ (const PathName& pathName) const;
		override	FileType	GetFileType_ (const PathName& pathName) const;
		override	off_t		GetSize_ (const PathName& pathName) const;
		override	Boolean		Exists_ (const PathName& pathName) const;
		override	DateTime	GetLastModificationTime_ (const PathName& pathName) const;
		override	void		CreateDirectory_ (const PathName& pathName);
		override	void		Delete_ (const PathName& pathName);
		override	void		Rename_ (const PathName& from, const PathName& to);
		override	PathName	GetParentDirectory_ (const PathName& pathName) const;
		override	UInt32		CountContainedItems_ (const PathName& pathName) const;
#if		qRealTemplatesAvailable
		override	Iterator<PathComponent>	MakeMountedVolumeIterator_ () const;	
		override	Iterator<PathComponent>	MakeDirectoryIterator_ (const PathName& directory) const;
#else
		override	Iterator(PathComponent)	MakeMountedVolumeIterator_ () const;
		override	Iterator(PathComponent)	MakeDirectoryIterator_ (const PathName& directory) const;
#endif
		override	PathName		GetCurrentDirectory_ () const;
		override	void			SetCurrentDirectory_ (const PathName& pathName);
		override	PathName		GetTemporaryDirectory_ () const;
		override	PathName		GetPreferencesDirectory_ () const;

	protected:
		override	off_t			GetDataForkSize_ (const PathName& pathName) const;
		override	off_t			GetResourceForkSize_ (const PathName& pathName) const;
		override	void			SetLastModificationTime_ (const PathName& pathName, const DateTime& modificationTime) const;
		override	DateTime		GetCreationTime_ (const PathName& pathName) const;
		override	void			SetCreationTime_ (const PathName& pathName, const DateTime& creationTime);
		override	UInt32			GetCreator_ (const PathName& pathName) const;
		override	void			SetCreator_ (const PathName& pathName, UInt32 creator);
		override	UInt32			GetType_ (const PathName& pathName) const;
		override	void			SetType_ (const PathName& pathName, UInt32 type);
		override	Boolean			GetLocked_ (const PathName& pathName) const;
		override	void			SetLocked_ (const PathName& pathName, Boolean locked);
		override	Boolean			GetVisible_ (const PathName& pathName) const;
		override	void			SetVisible_ (const PathName& pathName, Boolean visible);
		override	Boolean			Ejectable_ (const PathName& pathName);
		override	void			Eject_ (const PathName& pathName);
		override	PathName		GetBootVolume_ () const;
		override	PathName		GetCurrentSFGetFileDirectory_ () const;
		override	PathName		GetSystemDirectory_ () const;
		override	PathName		GetDeskTopDirectory_ (const PathName& forVolume) const;
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes, UInt32 creator, UInt32 fileType);
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes);
		override	void			Create_ (const PathName& pathName, UInt32 creator, UInt32 fileType);
		override	void			Create_ (const PathName& pathName);
		override	short			OpenResFile_ (const PathName& name, int openModes);
		override	short			GetVRefNum_ (const PathName& pathName) const;
		override	long			GetDirID_ (const PathName& pathName) const;
};






/*
 * Note that it IS legal to build a temporary FileSystem_UnixOS, but there are no guarantees as to
 * what will work - typically just file name validation. But even this can be useful.
 */
class	FileSystem_UnixOS : public AbstractFileSystem_UnixOS {
	public:
		FileSystem_UnixOS ();

	protected:
		override	void		Validate_ (const PathName& pathName) const;
		override	PathName	ParseStringToPathName_ (const String& pathName) const;
		override	String		PathNameToString_ (const PathName& pathName) const;
		override	PathName	GetFullPathName_ (const PathName& pathName) const;
		override	FileType	GetFileType_ (const PathName& pathName) const;
		override	off_t		GetSize_ (const PathName& pathName) const;
		override	Boolean		Exists_ (const PathName& pathName) const;
		override	DateTime	GetLastModificationTime_ (const PathName& pathName) const;
		override	void		Delete_ (const PathName& pathName);
		override	void		Rename_ (const PathName& from, const PathName& to);
		override	PathName	GetParentDirectory_ (const PathName& pathName) const;
		override	UInt32		CountContainedItems_ (const PathName& pathName) const;
#if		qRealTemplatesAvailable
		override	Iterator<PathComponent>	MakeMountedVolumeIterator_ () const;	
		override	Iterator<PathComponent>	MakeDirectoryIterator_ (const PathName& directory) const;
#else
		override	Iterator(PathComponent)	MakeMountedVolumeIterator_ () const;
		override	Iterator(PathComponent)	MakeDirectoryIterator_ (const PathName& directory) const;
#endif
		override	PathName		GetCurrentDirectory_ () const;
		override	void			SetCurrentDirectory_ (const PathName& pathName);
		override	PathName		GetTemporaryDirectory_ () const;
		override	PathName		GetPreferencesDirectory_ () const;

	protected:
		override	mode_t			GetPermissions_ (const PathName& pathName) const;
		override	void			SetPermissions_ (const PathName& pathName, mode_t permissions);

	protected:
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes, mode_t mode);
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes);
		override	void			Create_ (const PathName& pathName, mode_t mode);
		override	void			Create_ (const PathName& pathName);
		override	void			CreateDirectory_ (const PathName& pathName, mode_t mode);
		override	void			CreateDirectory_ (const PathName& pathName);
};







/*
 * Note that it IS legal to build a temporary FileSystem_DOS, but there are no guarantees as to
 * what will work - typically just file name validation. But even this can be useful.
 */
class	FileSystem_DOS : public AbstractFileSystem_DOS {
	public:
		FileSystem_DOS ();

	protected:
		override	void		Validate_ (const PathName& pathName) const;
		override	PathName	ParseStringToPathName_ (const String& pathName) const;
		override	String		PathNameToString_ (const PathName& pathName) const;
		override	PathName	GetFullPathName_ (const PathName& pathName) const;
		override	FileType	GetFileType_ (const PathName& pathName) const;
		override	off_t		GetSize_ (const PathName& pathName) const;
		override	Boolean		Exists_ (const PathName& pathName) const;
		override	DateTime	GetLastModificationTime_ (const PathName& pathName) const;
		override	void		Create_ (const PathName& pathName);
		override	void		CreateDirectory_ (const PathName& pathName);
		override	void		Delete_ (const PathName& pathName);
		override	void		Rename_ (const PathName& from, const PathName& to);
		override	PathName	GetParentDirectory_ (const PathName& pathName) const;
		override	UInt32		CountContainedItems_ (const PathName& pathName) const;
#if		qRealTemplatesAvailable
		override	Iterator<PathComponent>	MakeMountedVolumeIterator_ () const;	
		override	Iterator<PathComponent>	MakeDirectoryIterator_ (const PathName& directory) const;
#else
		override	Iterator(PathComponent)	MakeMountedVolumeIterator_ () const;
		override	Iterator(PathComponent)	MakeDirectoryIterator_ (const PathName& directory) const;
#endif
		override	PathName		GetCurrentDirectory_ () const;
		override	void			SetCurrentDirectory_ (const PathName& pathName);
		override	PathName		GetTemporaryDirectory_ () const;
		override	PathName		GetPreferencesDirectory_ () const;
		override	FileDescriptor	Open_ (const PathName& pathName, int openModes);


	protected:
		override	Boolean			GetVisible_ (const PathName& pathName) const;
		override	void			SetVisible_ (const PathName& pathName, Boolean visible);
		override	Boolean			GetLocked_ (const PathName& pathName) const;
		override	void			SetLocked_ (const PathName& pathName, Boolean locked);
};





/*
 * The class FileSystem is the class you will use most often. It is the appropriate subclass of
 * AbstractFileSystem for the current operating system you have compiled for.
 */
class	FileSystem : public
#if		qMacOS
	FileSystem_MacOS
#elif	qUnixOS
	FileSystem_UnixOS
#elif	qDOS || qWinNT || qWinOS
	FileSystem_DOS
#endif
	{
	public:
		FileSystem ();

	/*
	 * Get a reference to the currently installed FileSystem.
	 *
	 *		Note that we return an AbstractFileSystem&. This is to make it harder
	 * for system dependent code to creep in. If you know what you are doing
	 * something system dependent, then use the Get_YOUROS (), and note that it won't
	 * compile unless the right filesystem is in place.
	 *
	 *		All these getters return a pointer to the same underlying filesystem object,
	 * just as different subtypes.
	 */
	public:
		static	AbstractFileSystem&	Get ();

#if		qMacOS
		static	AbstractFileSystem_MacOS&	Get_MacOS ();
#elif	qUnixOS
		static	AbstractFileSystem_UnixOS&	Get_UnixOS ();
#elif	qDOS || qWinNT || qWinOS
		static	AbstractFileSystem_DOS&	Get_DOS ();
#endif
	private:
		static	FileSystem*	sThe;
};


#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	typedef	AbstractFileSystem::FileType	FileTypeX;
	#include Set_BitStringOfFileTypeX_hh
#endif


#if		qRealTemplatesAvailable
	extern	const	Set_BitString<FileSystem::FileType>	kAnyFileTypeSet;
	extern	const	Set_BitString<FileSystem::FileType>	kPlainFileTypeSet;
	extern	const	Set_BitString<FileSystem::FileType>	kDirectoryFileTypeSet;
#else
	extern	const	Set_BitString(FileTypeX)	kAnyFileTypeSet;
	extern	const	Set_BitString(FileTypeX)	kPlainFileTypeSet;
	extern	const	Set_BitString(FileTypeX)	kDirectoryFileTypeSet;
#endif











/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

// class PathComponentRep
	inline	PathComponentRep::PathComponentRep ()	{}


// class PathComponent
	inline	String	PathComponent::GetName () const
	{
		return (fRep->GetName ());
	}

#if		qMacOS
	inline	void	PathComponent::ApplySelfToPartialOSPathname (unsigned char* bigBuf,
		size_t bigBufSize, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
	{
		fRep->ApplySelfToPartialOSPathname (bigBuf, bigBufSize, paramBlkHeader);
	}
#elif	qUnixOS || qDOS || qWinNT || qWinOS
	inline	void	PathComponent::ApplySelfToPartialOSPathname (String& pathSoFar,
		char separatorChar) const
	{
		fRep->ApplySelfToPartialOSPathname (pathSoFar, separatorChar);
	}
#endif


// class PathName
#if		qRealTemplatesAvailable
	inline	const	Sequence<PathComponent>&	PathName::GetComponents () const
#else
	inline	const	Sequence(PathComponent)&	PathName::GetComponents () const
#endif
	{
		return (fPathElements);
	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__File__*/
