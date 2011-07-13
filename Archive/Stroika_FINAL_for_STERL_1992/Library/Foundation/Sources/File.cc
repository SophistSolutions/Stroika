/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/File.cc,v 1.45 1992/12/08 21:31:11 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *		-		Go thru code in Pathname and be sure we can never get an error,
 *				or exception there, even if that means reducing functionality
 *				(OK, just move into FileSystem).
 *
 *		-		Get rid of cases in bobs stuff where were sloppy about errno,
 *				and we do asserts that should be exceptions.
 *
 *		-		See why disk prefix stuff bob did not implemented same as
 *				Volumes in mac file system ????
 *
 *		-		Also, bob found bug in file stuff - mac version of open.
 *				returns wrong value it you open file with create flag, and file
 *				is busy, you get error message file exists. Also, code is funny
 *				there . ... could use a comment why so convoluted - answer is c
 *				eate with mac createor, etcc...
 *				Maybe do differnetly and see if we get EFNOTFOUND back from
 *				open???
 *
 *		-		Undecided whether or not its OK to have seperators embeded in
 *				SimplePathComponents. Probably the answer is NO, and must add
 *				method to FileSystem::ParseStringToPathName ().
 *
 *		-		Under DOS/Unix not sure we need seperatorChar parameter to
 *				ApplyXXX fuction - hardwire per os since only one compiled
 *				at a time ???? Maybe - look more closely.
 *
 *
 * Reference Material:
 *		Since all of this is so down-n-dirty, low level crapola, it is
 *	important to state, and validate this stuff with respect to the various
 *	reference manuals. Reference materials are also just helpful in trying
 *	to understand this mess.
 *
 *		Mac File System:
 *			-	Mostly use IM-IV, File system chapter.
 *				-	IM-IV-90:		file/volumne name legal characters
 *				-	IM-IV-99:		pathname format.
 *				-	IM-IV-100,100:	hints on how to fill in ParamBlocks with
 *									file name specs.
 *
 *		Unix File System:
 *			-	Posix Programmers Guide - Oreilly & Associates...
 *				Especially appendices with specs on what types/functions
 *				should be in what includes.
 *
 *		DOS File system:
 *			-	Folklore, rumor, and innuendo.
 *
 *
 *
 * Changes:
 *	$Log: File.cc,v $
 *		Revision 1.45  1992/12/08  21:31:11  lewis
 *		Add case qWinNT whereever we had qWinOS.
 *
 *		Revision 1.44  1992/12/05  17:34:34  lewis
 *		A bunch of places that were qDOS are now qDOS || qWinOS.
 *		Had to move DOSMountedVolumeIterator class out of function
 *		scope because of trouble with refernece in Clone() method - just
 *		like the UNIX one - not sure whose right here...
 *		I See that BCC supports ::stat() so implemented a few more DOS
 *		functions. Borland does NOT support all the POSIX names so I
 *		had to define things like  S_ISDIR(m) myself.
 *		Use X(X&) for cloning.
 *		Renamed Try->try, and Catch->catch - see exception.hh for explation.
 *
 *		Revision 1.43  1992/12/04  19:41:00  lewis
 *		Do quick hack mounted volume iterator for DOS - not sure how
 *		to do right - just return C: for now.
 *
 *		Revision 1.42  1992/12/03  07:11:33  lewis
 *		Add Done () overrides to iterators.
 *
 *		Revision 1.41  1992/11/26  02:47:40  lewis
 *		Got rid of special hack for kBad - test in 1 arg pathname CTOR - not sure
 *		why sterl put it back in?
 *		Moved CurrentDirectoryPathComponent and ParentDirectoryPathComponent declarations
 *		to .hh file so in stroika-globals we can initialize kDot and kDotDot.
 *		Got rid of hacks: mkFileTypeSets - now do all the magic needed in
 *		Stroika-Foundation-Globals.hh directly - and much of that may soon disapear.
 *
 *		Revision 1.40  1992/11/23  21:52:13  lewis
 *		Lots of changes to get Iterator<T> returned instead of IteratorRep<T>*.
 *
 *		Revision 1.39  1992/11/20  19:30:46  lewis
 *		Rework iterators to support new it.More() instead of it.Next/Done.
 *
 *		Revision 1.38  1992/11/16  04:43:31  lewis
 *		Get rid of qDontNestEnumUsedInTemplate, and the workarounds for
 *		this bug.
 *
 *		Revision 1.37  1992/11/15  16:09:27  lewis
 *		Fix for templates.
 *
 *		Revision 1.36  1992/11/12  08:22:29  lewis
 *		Fix up Set_BitString usage for templates.
 *
 *		Revision 1.35  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.34  1992/10/16  22:51:08  lewis
 *		Added op==/!= for PathComponent&
 *
 *		Revision 1.33  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.32  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.31  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.30  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.29  1992/10/09  04:43:28  lewis
 *		Added #include Set_BitStringOfFileType_cc cuz accidentally forgotten.
 *
 *		Revision 1.28  1992/10/07  23:14:39  sterling
 *		Tried (conditionally) to use new GenClass stuff.
 *
 *		Revision 1.27  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.26  1992/09/26  22:26:30  lewis
 *		Added PathName      operator+ (const PathName& lhs, const PathName& rhs).
 *		Fixed UNIX version of StringToPathname - accidentally left in trailing
 *		'/'s in pathcomponents.
 *
 *		Revision 1.25  1992/09/26  05:23:22  lewis
 *		A few cleanups to the UNIX stuff, and got rid of all reference to
 *		kEmptyString.
 *
 *		Revision 1.24  1992/09/25  01:26:53  lewis
 *		Fix UNIX get current directory code.
 *
 *		Revision 1.23  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.22  1992/09/15  17:21:57  lewis
 *		Because of Container class changes renamed a bunch of things. Also, the
 *		change evoked a gcc 2.2.1 compiler bug that needed to be worked around
 *		temporarily by qDontNestEnumUsedInTemplate
 *
 *		Revision 1.20  1992/09/09  03:24:24  lewis
 *		Use endl instead of newline.
 *
 *		Revision 1.19  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.18  1992/09/03  19:33:01  lewis
 *		Lots of minor cleanups, and filled in most of the missing
 *		stuff for UNIX - should work  now for most cases.
 *
 *		Revision 1.17  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.16  1992/07/22  02:32:47  lewis
 *		Quick hacks to support the new Character stuff.
 *
 *		Revision 1.15  1992/07/21  17:28:10  lewis
 *		Fixed typo and added PatchRemove () overrides to our two iterator subclasses
 *		for the macos.
 *
 *		Revision 1.14  1992/07/21  06:39:01  lewis
 *		Use qCFront_NestedTypesHalfAssed to work around trouble with nested types
 *		when trying to compile with gcc.
 *		Use privately externed functions for File.cc to build PathName globals -
 *		instead of doing it in the .cc file where we have no control over
 *		construction order, or here were we dont have the right types
 *		declared (private to .cc file).
 *		Fixedup/implemeted a few more UNIX functions and made iterators
 *		return Iterators.
 *
 *		Revision 1.13  1992/07/16  17:27:21  lewis
 *		Work around compiler bug on mac.
 *
 *		Revision 1.12  1992/07/16  13:31:55  lewis
 *		Moved PathComponent (PathComponent&) CTOR and op= here from header.
 *
 *		Revision 1.11  1992/07/16  06:50:35  lewis
 *		Misc changes to compile on PC.
 *
 *		Revision 1.10  1992/07/16  05:11:21  lewis
 *		Moved the definition of the various subclasses of PathComponent  and
 *		PathCompoenntRep to File.cc from File.hh. Get rid of relativePathName
 *		from HFSPathComponent. Added oprator (const PathComponent& pathComponent1,
 *		const PathComponent& pathComponent2).
 *		Added CTORs to PathName for DirID/vrefnum - and fsspec. Also, added typedfefs for these
 *		types to the PathName class.
 *		Use new String method Contains - instead of IndexOf () != kBadSequenceIndex.
 *
 *		Revision 1.8  1992/07/14  23:58:53  lewis
 *		Cleanup comments, and get to compile (mostly) under aux/gcc - still crashes
 *		gcc at the end. Fixed forgotten second parameters to AppleSelfToPartialOSPathName (),
 *		and bad return values in stubbed out mac filesystem code for other
 *		OS's.
 *
 *		Revision 1.7  1992/07/14  19:42:25  lewis
 *		Lots of work - a couple days - getting PCXLate working with this - sort
 *		of as a testsuite - since its hard to figure out how to do a better testsuite.
 *		Rationalized a bit more what goes into what filesystems - added new stuff to DOS
 *		filesystem (eg Visible) and got rid of mode_t since Simone says inappropriate.
 *		Added more support for permissions to Unix file system. Still lots todo there
 *		and not too close to working.
 *		Significant change to Get/SetVol code - now use PBSetVol/PBGetVol.
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
 *		Revision 1.5  1992/07/07  06:50:03  lewis
 *		Try to implement CreateDirectory for the mac, though never tested.
 *
 *		Revision 1.4  1992/07/01  07:22:15  lewis
 *		Added ifdefs to work around gcc qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope bug.
 *		Also, code that was ifdefed for qBorlandCPlus also required for gcc - My
 *		guess is that CFronts overloading was wrong - anyway, outvoted 2to1, so
 *		go with gcc way...
 *
 *		Revision 1.3  1992/06/22  15:44:04  lewis
 *		Commented out lots of stuff to get to compile with gcc - fix later.
 *
 *		Revision 1.2  1992/06/20  19:28:58  lewis
 *		Added a few hacks trying to get this to work with gcc-aux - still needs lots more
 *		work. Try to settle on something SOLEY POSIX compilaint....
 *
 *		Revision 1.30  1992/06/03  21:04:14  lewis
 *		Add cast to const void* of first arg to String ctor in PathName::PathName (const osFSSpec&)
 *		since for some reason failed otherwise on mac CFront under AUX - worked
 *		before when I built under mac OS - very strange - not sure which time the
 *		compiler was right/wrong - but should decide the same!!!
 *
 *		Revision 1.29  1992/05/21  17:29:06  lewis
 *		React to change in definition of String::SubString().
 *
 *		Revision 1.28  92/05/09  02:32:11  02:32:11  lewis (Lewis Pringle)
 *		Ported to BorlandC++.
 *		
 *		Revision 1.27  92/04/20  14:09:41  14:09:41  lewis (Lewis Pringle)
 *		Change name of mode #define constants in ::creat call to be Posix complaint.
 *		
 *		Revision 1.25  92/04/07  11:28:54  11:28:54  lewis (Lewis Pringle)
 *		Compile on pc.
 *		
 *		Revision 1.24  92/03/30  14:54:36  14:54:36  lewis (Lewis Pringle)
 *		Work around bugs in  CFront 21's implementation of nested class names.
 *		
 *		Revision 1.23  92/03/30  14:11:45  14:11:45  lewis (Lewis Pringle)
 *		Port to BorlandC++ and use qWinOS in all places we had qDOS - at least most.
 *		Add ifdefs for templates.
 *		
 *		Revision 1.21  1992/02/08  04:31:43  lewis
 *		General cleanups, and comments, and added unimplemented Get/SetFileVisible for MacOS.
 *
 *		Revision 1.20  1992/01/29  16:44:19  lewis
 *		Added access permission denied exception.
 *		Got rid of StringUnwrap, and used PATH_MAX, and ToCString () instead.
 *		Used exceptions in unix/dos implementation of open.
 *
 *		Revision 1.19  1992/01/25  07:02:50  lewis
 *		Slight cleanups.
 *
 *
 */



// Not needed - 9/26/92#include	<ctype.h>
#include	<errno.h>
#include	<string.h>
#include	<stdlib.h>
#include	<limits.h>


#include	"OSRenamePre.hh"
#if		qMacOS
	#include	<Errors.h>
	#include	<Files.h>
	#include	<Folders.h>
	#include	<OSUtils.h>
	#include	<Resources.h>
	#include	<SysEqu.h>		// for SFSaveDisk, etc globals
#elif	qUnixOS
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<unistd.h>
	#include	<dirent.h>
#elif	qDOS || qWinNT || qWinOS
	#include	<errno.h>
	#include	<fcntl.h>
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include 	<dir.h>
	#include	<dos.h>
	#include	<io.h>
#endif	/*OS*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Format.hh"
#include	"Math.hh"
#include	"Memory.hh"
#include	"OSConfiguration.hh"

#include	"File.hh"




// later to official bug name for this...
#if		qBorlandCPlus
	#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
	#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#endif




#if		qMacOS

/*
 * 		These are all the Mac filesystem low level (PB) calls I now make (as of July 9, 1992)
 * They are listed by the type of the param block, and with all the fields I need to worry
 * about setting before the call.
 *
WDPBPtr		=	(WDPBRec *)
	pascal OSErr PBHGetVol(WDPBPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<--    18    ioNamePtr     pointer
	pascal OSErr PBGetWDInfo(WDPBPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<--    18    ioNamePtr     pointer
		<->    22    ioVRefNum     word
		-->    26    ioWDIndex     word
		<->    28    ioWDProcID    long word
		<->    32    ioWDVRefNum   word
	pascal OSErr PBOpenWD(WDPBPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		-->    18    ioNamePtr     pointer
		<->    22    ioVRefNum     word
		-->    28    ioWDProcID    long word
		-->    48    ioWDDirID     long word
	pascal OSErr PBCloseWD(WDPBPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<--    16    ioResult      word
		-->    22    ioVRefNum     word
	pascal OSErr PBHGetVol(WDPBPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<--    18    ioNamePtr     pointer


HParmBlkPtr	=	(HParamBlockRec *)
	pascal OSErr PBDirCreate(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		<->    48    ioDirID       long word
	pascal OSErr PBHDelete(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		-->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		-->    48    ioDirID       long word
	pascal OSErr PBHGetFInfo(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		-->    28    ioFDirIndex   word
		<->    48    ioDirID       long word
	pascal OSErr PBHSetFInfo(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		-->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		-->    32    ioFlFndrInfo  16 bytes
		-->    48    ioDirID       long word
		-->    72    ioFlCrDat     long word
		-->    76    ioFlMdDat     long word
	pascal OSErr PBHSetFLock(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		-->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		-->    48    ioDirID       long word
	pascal OSErr PBHRstFLock(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		-->    18    ioNamePtr     pointer
		-->    22    ioVRefNum     word
		-->    48    ioDirID       long word
	pascal OSErr PBHGetVInfo(HParmBlkPtr paramBlock,Boolean async); 
		-->    12    ioCompletion  pointer
		<->    18    ioNamePtr     pointer
		<->    22    ioVRefNum     word
		-->    28    ioVolIndex    word


CInfoPBPtr	=	(union CInfoPBRec {	HFileInfo hFileInfo;	DirInfo dirInfo;	};	*)
	pascal OSErr PBGetCatInfo(CInfoPBPtr paramBlock,Boolean async); 
		Files:                                   Directories:
		-->    12    ioCompletion   pointer      -->    12    ioCompletion  pointer
		<->    18    ioNamePtr      pointer      <->    18    ioNamePtr     pointer
		-->    22    ioVRefNum      word         -->    22    ioVRefNum     word
		-->    28    ioFDirIndex    word         -->    28    ioFDirIndex   word
		<->    48    ioDirID        long word    <->    48    ioDrDirID     long word

 *
 *		After careful examination, it is clear that what we need to set on an ongoing
 * basis does not exist in any one of Apples data structions in their PB file support.
 * While astoundingly stupid, pathetic, atrocious, and frustating, we have an out:
 * We will define our own structure, with the same layout as what apple clearly expects
 * and have our setting routines munge that. Take that Apple!!!
 *
 * 		This structure will be called MacOS_ExtendedParamBlockHeader, and will consist
 * of what apple has as a Macro for ParamBlockHeader, and add to it the DirID field
 * which was added in an ad-hoc manner for HFS. We add 6 long fillers to assure
 * that ioDirID is in the right place. I looked at a lot of apple examples (File.h) to be
 * sure this was right, and there is an assertion that offsetof (ioDirID) = 48,
 * in the main routine to fill this in.
 *
 *		Then, using this, do PathName::ApplySelfToPartialOSPathname () overloads using
 * the three main structures we've identified above - WDPBRec, HParamBlockRec, and
 * CInfoPBRec.
 */


	//	QElemPtr qLink;    			/*queue link in header*/
	//	short qType;    			/*type byte for safety check*/
	//	short ioTrap;    			/*FS: the Trap*/
	//	Ptr ioCmdAddr;    			/*FS: address to dispatch to*/
	//	ProcPtr ioCompletion;  		/*completion routine addr (0 for synch calls)*/
	//	OSErr ioResult;    			/*result code*/
	//	StringPtr ioNamePtr;  		/*ptr to Vol:FileName string*/
	//	short ioVRefNum;   			/*volume refnum (DrvNum for Eject and MountVol)*/
#include	"OSRenamePre.hh"
struct	MacOS_ExtendedParamBlockHeader {
	ParamBlockHeader
	long filler[6];
	long ioDirID;
};
#include	"OSRenamePost.hh"
#endif





/*
 * A HFSPathComponentRep is a special PathComponent available only under the MacOS. It is
 * to help deal with sfgetfile, and other mac os braindamage.
 *
 * This class is allowed to be used on other os's but is really only useful on the mac.
 *
 *		<< a more enlightened description would be appropriate >>
 */
// sb nested in ParentDirectoryPathComponent except for 
// qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	HFSPathComponentRep : public PathComponentRep {
		public:
#if		qCFront_NestedTypesHalfAssed
			HFSPathComponentRep (VRefNum vRefNum, DirID dirID);
#else
			HFSPathComponentRep (PathName::VRefNum vRefNum, PathName::DirID dirID);
#endif
		public:
			override	String	GetName () const;
	
		public:
#if		qMacOS
			override	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf,
									size_t bigBufSize,
									MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
#elif	qDOS || qWinNT || qWinOS || qUnixOS
			override	void	ApplySelfToPartialOSPathname (String& pathSoFar,
									char separatorChar) const;
#endif
		private:
#if		qCFront_NestedTypesHalfAssed
			VRefNum		fVRefNum;
			DirID		fDirID;
#else
			PathName::VRefNum	fVRefNum;
			PathName::DirID		fDirID;
#endif
	};

class	HFSPathComponent : public PathComponent {
	public:
#if		qCFront_NestedTypesHalfAssed
		HFSPathComponent (VRefNum vRefNum, DirID dirID);
#else
		HFSPathComponent (PathName::VRefNum vRefNum, PathName::DirID dirID);
#endif

	protected:
		HFSPathComponent (HFSPathComponentRep* pathComponentRep);
};







/*
 * A SimplePathComponent is what is created by PathComponent (const String&) by default.
 * It is simply a string path name. Its interpretation will vary accross FileSystems
 * and so its use should be limited.
 */
// sb nested in SimplePathComponent except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
		class	SimplePathComponentRep : public PathComponentRep {
			public:
				SimplePathComponentRep (const String& name);
		
			public:
				override	String	GetName () const;
		
			public:
		#if		qMacOS
				override	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf,
										size_t bigBufSize,
										MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
		#elif	qDOS || qWinNT || qWinOS || qUnixOS
				override	void	ApplySelfToPartialOSPathname (String& pathSoFar,
										char separatorChar) const;
		#endif
		
			private:
				String	fName;
		};

class	SimplePathComponent : public PathComponent {
	public:
		SimplePathComponent (const String& name);

	protected:
		SimplePathComponent (SimplePathComponentRep* pathComponentRep);
};









/*
 * A CurrentDirectoryPathComponent means that when looking up a PathName in a filesystem, start
 * from the current directory (as opposed to the root?). This is ignored anyplace but the
 * first entry in a PathName.
 */
// sb nested in CurrentDirectoryPathComponent except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	CurrentDirectoryPathComponentRep : public PathComponentRep {
		public:
			CurrentDirectoryPathComponentRep ();
	
		public:
			override	String	GetName () const;
	
		public:
	#if		qMacOS
			override	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
									MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
	#elif	qDOS || qWinNT || qWinOS || qUnixOS
			override	void	ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const;
	#endif
	};









/*
 * A ParentDirectoryPathComponent is interpretted as going up (to parent - in tree sense)
 * a directory in the process of following a directory path (eg. ".." in UNIX/DOS).
 */
// sb nested in ParentDirectoryPathComponent except for qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	ParentDirectoryPathComponentRep : public PathComponentRep {
		public:
			ParentDirectoryPathComponentRep ();
	
		public:
			override	String	GetName () const;
	
		public:
	#if		qMacOS
			override	void	ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize,
									MacOS_ExtendedParamBlockHeader& paramBlkHeader) const;
	#elif	qDOS || qWinNT || qWinOS || qUnixOS
			override	void	ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const;
	#endif
	};














/*
 ********************************************************************************
 ******************************* PathComponentRep *******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include SharedOfPathComponentRep_cc
#endif	/*!qRealTemplatesAvailable*/

PathComponentRep::~PathComponentRep ()
{
}










/*
 ********************************************************************************
 ********************************** PathComponent *******************************
 ********************************************************************************
 */


#if		!qRealTemplatesAvailable
	#include SequenceOfPathComponent_cc
#endif	/*!qRealTemplatesAvailable*/

PathComponent::PathComponent () :
	fRep (Nil, new SimplePathComponentRep (""))
{
}

PathComponent::PathComponent (const String& name):
	fRep (Nil, new SimplePathComponentRep (name))
{
}

PathComponent::PathComponent (const char* name):
	fRep (Nil, new SimplePathComponentRep (name))
{
	RequireNotNil (name);
}

PathComponent::PathComponent (PathComponentRep* pathComponentRep):
	fRep (Nil, pathComponentRep)
{
}

PathComponent::PathComponent (const PathComponent& from):
	fRep (from.fRep)
{
}

const PathComponent& PathComponent::operator= (const PathComponent& rhs)
{
	fRep = rhs.fRep;
	return (*this);
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
Boolean	operator== (const PathComponent& lhs, const PathComponent& rhs)
{
	// not a great way to compare, but best I can see for now...
	return (lhs.GetName () == rhs.GetName ());
}





/*
 ********************************************************************************
 ************************************* operator!= *******************************
 ********************************************************************************
 */
Boolean	operator!= (const PathComponent& lhs, const PathComponent& rhs)
{
	return (not (lhs == rhs));
}







/*
 ********************************************************************************
 **************************** SimplePathComponentRep ****************************
 ********************************************************************************
 */

SimplePathComponentRep::SimplePathComponentRep (const String& name):
	PathComponentRep (),
	fName (name)
{
}

String	SimplePathComponentRep::GetName () const
{
	return (fName);
}

#if		qMacOS
void	SimplePathComponentRep::ApplySelfToPartialOSPathname (unsigned char* bigBuf,
			size_t bigBufSize, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
{
	RequireNotNil (bigBuf);
	Assert (bigBuf == paramBlkHeader.ioNamePtr);		// we just munge the bigBuf directly...

	/*
	 * If we already have a partial path name specified, we must append a colon
	 * before adding our part to the path.
	 */
	String	relName	=	String (&bigBuf[1], bigBuf[0]);
	if (relName.GetLength () != 0 and (relName[relName.GetLength ()] != ':')) {
		relName += ':';
	}
	relName += fName;

	if (bigBufSize > 255) {
		bigBufSize = 255;		// for our purposes, since this is a pascal string.
	}

	/*
	 * Convert it back, and be sure it fits.
	 */
	if (relName.GetLength () < bigBufSize) {
		relName.ToPString (bigBuf, bigBufSize);
	}
	else {
		FileSystem::sFileNameTooLong.Raise ();
	}
}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
void	SimplePathComponentRep::ApplySelfToPartialOSPathname (String& pathSoFar,
			char separatorChar) const
{
	/*
	 * Append separator if there isn't one already (like if we were first component)
	 */
	size_t	len	=	pathSoFar.GetLength ();
	if ((len != 0) and pathSoFar[len].GetAsciiCode () != separatorChar) {
		pathSoFar += separatorChar;
	}

	pathSoFar += fName;
}
#endif









/*
 ********************************************************************************
 ******************************* SimplePathComponent ****************************
 ********************************************************************************
 */
SimplePathComponent::SimplePathComponent (const String& name):
	PathComponent (new SimplePathComponentRep (name))
{
}

SimplePathComponent::SimplePathComponent (SimplePathComponentRep* pathComponentRep):
	PathComponent (pathComponentRep)
{
}










/*
 ********************************************************************************
 ************************* CurrentDirectoryPathComponentRep *********************
 ********************************************************************************
 */

CurrentDirectoryPathComponentRep::CurrentDirectoryPathComponentRep ():
	PathComponentRep ()
{
}

String	CurrentDirectoryPathComponentRep::GetName () const
{
#if		qMacOS
	return (":");				// no good answer here - could be empty or ":"
								// since this should only be used at start of pathname
								//  ":" is mostlikely the right answer. But could  lead
								// to "::" which would be interpretted as UP DIR.
#elif	qDOS || qWinNT || qWinOS || qUnixOS
	return (".");
#endif
}

#if		qMacOS
void	CurrentDirectoryPathComponentRep::ApplySelfToPartialOSPathname (unsigned char* /*bigBuf*/,
			size_t /*bigBufSize*/, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
{
	/*
	 * If we have not yet set the vrefnum or dirID of the paramblock, nor the pathname,
	 * the we did come at the beginning of the path and we should set those things
	 * up properly to make this path current working directory relative. If any of those
	 * things are setup, then this CurrentDirectoryPathComponent is interpretted as
	 * being relative to where we are so far in the path as we traverse it - that is -
	 * a no-op. So we complete ignore ourselves, and do nothing.
	 */
	RequireNotNil (paramBlkHeader.ioNamePtr);
	if ((paramBlkHeader.ioNamePtr[0] == 0) and (paramBlkHeader.ioVRefNum == 0) and
		(paramBlkHeader.ioDirID == 0)) {
		WDPBRec		aWDParam;
		osStr255	nameBuf;
		memset (&aWDParam, 0, sizeof (aWDParam));
		aWDParam.ioCompletion = Nil;
		aWDParam.ioNamePtr = nameBuf;
		OSErr err = ::PBHGetVolSync (&aWDParam);
		Assert (err == 0);
		paramBlkHeader.ioVRefNum = aWDParam.ioWDVRefNum;					// See IM-IV-132
		paramBlkHeader.ioDirID = aWDParam.ioWDDirID;


		/*
		 * Start the name with a column. IM makes this optional, but prefered. For us it
		 * is required since if we append more than one name, we get a Volume relative spec
		 * (in the mac os's eyes) which we didn't want - (Fred:Joe seen as volumne relative even
		 * we've specified a starting ioVRefNum/ioDirID).
		 */
		paramBlkHeader.ioNamePtr[0] = 1;
		paramBlkHeader.ioNamePtr[1] = ':';
	}
}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
void	CurrentDirectoryPathComponentRep::ApplySelfToPartialOSPathname (String& /*pathSoFar*/,
			char /*separatorChar*/) const
{
	// Do nothing - we already are current directory relative...
}
#endif










/*
 ********************************************************************************
 ************************* CurrentDirectoryPathComponent ************************
 ********************************************************************************
 */
CurrentDirectoryPathComponent::CurrentDirectoryPathComponent ():
	PathComponent (new CurrentDirectoryPathComponentRep ())
{
}










/*
 ********************************************************************************
 ************************* ParentDirectoryPathComponentRep **********************
 ********************************************************************************
 */

ParentDirectoryPathComponentRep::ParentDirectoryPathComponentRep ():
	PathComponentRep ()
{
}

String	ParentDirectoryPathComponentRep::GetName () const
{
#if		qMacOS
	return ("::");				// no good answer here - could be "::" or ":"
								// really whole thing not such a good idea...
#elif	qDOS || qWinNT || qWinOS || qUnixOS
	return ("..");
#endif
}

#if		qMacOS
void	ParentDirectoryPathComponentRep::ApplySelfToPartialOSPathname (unsigned char* bigBuf,
			size_t bigBufSize, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
{
	// Just a first cut at this...July 10, 1992 LGP
	/*
	 * We must look at what has been done so far, and munge it so that the net effect
	 * is to move us up a directory.
	 */

	/*
	 * In order to say go up a directory, we must have specified some starting point
	 * directory. This we must have either a vrefnum, or a volume specification in our
	 * ioNamePtr. (my god, I'm think I'm actually starting to understand this ParamBlk
	 * naming stuff - and its only been two years!).
	 */
	Require (bigBuf == paramBlkHeader.ioNamePtr);
	RequireNotNil (paramBlkHeader.ioNamePtr);
	if ((paramBlkHeader.ioVRefNum == 0) and
		((paramBlkHeader.ioNamePtr[0] == 0) or
		 (paramBlkHeader.ioNamePtr[1] == ':')
		)
		) {
		FileSystem::sException.Raise ();
	}

	String	relName	=	String (&paramBlkHeader.ioNamePtr[1], paramBlkHeader.ioNamePtr[0]);
	/*
	 * If the name already starts with a : then we must add just one. Otherwise, add two.
	 * (See IM-IV-99).
	 */
	if ((relName.GetLength () == 0) or (relName[1] == ':')) {
		relName = String ("::") + relName;
	}
	else {
		relName = String (":") + relName;
	}

	/*
	 * Convert it to mac file system format (PSTRING), and be sure it fits.
	 */
	if (bigBufSize > 255) {
		bigBufSize = 255;		// for our purposes, since this is a pascal string.
	}
	Assert (bigBuf == paramBlkHeader.ioNamePtr);
	if (relName.GetLength () < bigBufSize) {
		relName.ToPString (bigBuf, bigBufSize);
	}
	else {
		FileSystem::sFileNameTooLong.Raise ();
	}
}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
void	ParentDirectoryPathComponentRep::ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const
{
	/*
	 * Append separator if there isn't one already (like if we were first component)
	 */
	size_t	len	=	pathSoFar.GetLength ();
	if ((len != 0) and pathSoFar[len].GetAsciiCode () != separatorChar) {
		pathSoFar += separatorChar;
	}

	pathSoFar += "..";
	pathSoFar += separatorChar;
}
#endif









/*
 ********************************************************************************
 ************************* ParentDirectoryPathComponent *************************
 ********************************************************************************
 */
ParentDirectoryPathComponent::ParentDirectoryPathComponent ():
	PathComponent (new ParentDirectoryPathComponentRep ())
{
}










/*
 ********************************************************************************
 ********************************** HFSPathComponentRep *************************
 ********************************************************************************
 */

#if		qCFront_NestedTypesHalfAssed
HFSPathComponentRep::HFSPathComponentRep (VRefNum vRefNum, DirID dirID):
#else
HFSPathComponentRep::HFSPathComponentRep (PathName::VRefNum vRefNum, PathName::DirID dirID):
#endif
	PathComponentRep (),
	fVRefNum (vRefNum),
	fDirID (dirID)
{
}

String	HFSPathComponentRep::GetName () const
{
	return (String ("<") + ltostring (fVRefNum) + String (", ") + ltostring (fDirID) + String (">"));
}

#if		qMacOS
void	HFSPathComponentRep::ApplySelfToPartialOSPathname (unsigned char* bigBuf,
			size_t /*bigBufSize*/, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
{
	RequireNotNil (bigBuf);

	if (bigBuf [0] != 0) {
		// HFSPathComponentRep must be a prefix - must come before others...
		// If we lift this restriction, then we must say ignore THIS, or
		// what came before??? Probably IGNORE what came before - I think
		// that makes the most sense...
		FileSystem::sException.Raise ();
	}
	// Then we were using a DirID and real VRefNum
	paramBlkHeader.ioVRefNum = fVRefNum;
	paramBlkHeader.ioDirID = fDirID;
}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
void	HFSPathComponentRep::ApplySelfToPartialOSPathname (String& pathSoFar,
			char separatorChar) const
{
	FileSystem::sPathComponentNotSupported.Raise ();
}
#endif










/*
 ********************************************************************************
 ******************************* HFSPathComponent *******************************
 ********************************************************************************
 */
#if		qCFront_NestedTypesHalfAssed
HFSPathComponent::HFSPathComponent (VRefNum vRefNum, DirID dirID):
#else
HFSPathComponent::HFSPathComponent (PathName::VRefNum vRefNum, PathName::DirID dirID):
#endif
	PathComponent (new HFSPathComponentRep (vRefNum, dirID))
{
}

HFSPathComponent::HFSPathComponent (HFSPathComponentRep* pathComponentRep):
	PathComponent (pathComponentRep)
{
}










/*
 ********************************************************************************
 *************************************** PathName *******************************
 ********************************************************************************
 */

PathName::PathName () :
	fPathElements ()
{
}

PathName::PathName (const PathComponent& p1):
	fPathElements ()
{
}

PathName::PathName (const PathComponent& p1, const PathComponent& p2):
	fPathElements ()
{
	fPathElements.Append (p1);
	fPathElements.Append (p2);
}

PathName::PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3):
	fPathElements ()
{
	fPathElements.Append (p1);
	fPathElements.Append (p2);
	fPathElements.Append (p3);
}

PathName::PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3,
			const PathComponent& p4):
	fPathElements ()
{
	fPathElements.Append (p1);
	fPathElements.Append (p2);
	fPathElements.Append (p3);
	fPathElements.Append (p4);
}

PathName::PathName (const PathComponent& p1, const PathComponent& p2, const PathComponent& p3,
			const PathComponent& p4, const PathComponent& p5):
	fPathElements ()
{
	fPathElements.Append (p1);
	fPathElements.Append (p2);
	fPathElements.Append (p3);
	fPathElements.Append (p4);
	fPathElements.Append (p5);
}

PathName::PathName (const PathName& pathName):
	fPathElements (pathName.fPathElements)
{
}

PathName::PathName (const PathComponent& pathComponent, const PathName& pathName):
	fPathElements (pathName.fPathElements)
{
	fPathElements.Prepend (pathComponent);
}

PathName::PathName (const PathName& pathName, const PathComponent& pathComponent):
	fPathElements (pathName.fPathElements)
{
	fPathElements.Append (pathComponent);
}

#if		qRealTemplatesAvailable
PathName::PathName (const Sequence <PathComponent>& pathComponents):
	fPathElements (pathComponents)
{
}
#else
PathName::PathName (const Sequence (PathComponent)& pathComponents):
	fPathElements (pathComponents)
{
}
#endif

#define	qSomeMPW_CFrontBug	1
#if		qMacOS
PathName::PathName (const struct osFSSpec& itsFSSpec):
	fPathElements ()
{
	if (itsFSSpec.parID == 0) {
AssertNotImplemented ();
#if 0
#if		qSomeMPW_CFrontBug
		PathComponent	xx	=	HFSPathComponent (itsFSSpec.vRefNum);
		fPathElements.Append (xx);
#else
		fPathElements.Append (HFSPathComponent (itsFSSpec.vRefNum));
#endif
#endif
	}
	else {
#if		qSomeMPW_CFrontBug
		PathComponent	yyy	=	HFSPathComponent (itsFSSpec.vRefNum, itsFSSpec.parID);
		fPathElements.Append (yyy);
#else
		fPathElements.Append (HFSPathComponent (itsFSSpec.vRefNum, itsFSSpec.parID));
#endif
	}
	if (itsFSSpec.name[0] != 0) {
		fPathElements.Append (PathComponent (String ((const void*)&itsFSSpec.name[1], itsFSSpec.name[0])));
	}
}
#endif	/*qMacOS*/
#if	0
PathName::PathName (WDRefNum wdRefNum):
	fPathElements ()
{
#if		qSomeMPW_CFrontBug
	PathComponent	xxx	=	HFSPathComponent (wdRefNum);
	fPathElements.Append (xxx);
#else
	fPathElements.Append (HFSPathComponent (wdRefNum));
#endif
}

PathName::PathName (WDRefNum wdRefNum, const String& pathName):
	fPathElements ()
{
#if		qSomeMPW_CFrontBug
	PathComponent	xxx	=	HFSPathComponent (wdRefNum);
	fPathElements.Append (xxx);
#else
	fPathElements.Append (HFSPathComponent (wdRefNum));
#endif
	fPathElements.Append (PathComponent (pathName));
}
#endif

PathName::PathName (VRefNum vRefNum, DirID dirID):
	fPathElements ()
{
#if		qSomeMPW_CFrontBug
	PathComponent	xxx	=	HFSPathComponent (vRefNum, dirID);
	fPathElements.Append (xxx);
#else
	fPathElements.Append (HFSPathComponent (vRefNum, dirID));
#endif
}

PathName::PathName (VRefNum vRefNum, DirID dirID, const String& pathName):
	fPathElements ()
{
#if		qSomeMPW_CFrontBug
	PathComponent	xxx	=	HFSPathComponent (vRefNum, dirID);
	fPathElements.Append (xxx);
#else
	fPathElements.Append (HFSPathComponent (vRefNum, dirID));
#endif
	fPathElements.Append (PathComponent (pathName));
}

const PathName& PathName::operator= (const PathName& rhs)
{
	fPathElements = rhs.fPathElements;
	return (*this);
}

size_t	PathName::GetLength () const
{
	return (fPathElements.GetLength ());
}

PathName	PathName::GetPrefix () const
{
	if (fPathElements.GetLength () < 2) {
		// either already bad, or no prefix - just top level dir...
		return (kBad);
	}
#if		qRealTemplatesAvailable
	Sequence<PathComponent>	initialSegment	=	fPathElements;
#else
	Sequence(PathComponent)	initialSegment	=	fPathElements;
#endif
	initialSegment.RemoveAt (initialSegment.GetLength ());	// remove last item and rest is prefix
	return (initialSegment);
}

PathComponent	PathName::GetFileName () const
{
	if (fPathElements.GetLength () == 0) {
		FileSystem::sException.Raise ();
	}
	return (fPathElements[fPathElements.GetLength ()]);
}

#if		qRealTemplatesAvailable
PathName::operator Iterator<PathComponent> () const
#else
PathName::operator Iterator(PathComponent) () const
#endif
{
#if		qGCC_ConversionOperatorInitializerConfusionBug
	#if		qRealTemplatesAvailable
		return (Iterator<PathComponent>)(fPathElements);
	#else
		return (Iterator(PathComponent))(fPathElements);
	#endif
#else
	return (fPathElements);
#endif
}

#if		qRealTemplatesAvailable
PathName::operator SequenceIterator<PathComponent> () const
#else
PathName::operator SequenceIterator(PathComponent) () const
#endif
{
	return (fPathElements);
}

const PathName& PathName::operator+= (const PathComponent& rhs)
{
	fPathElements += rhs;
	return (*this);
}

const PathName& PathName::operator+= (const PathName& rhs)
{
	ForEach (PathComponent, it, rhs) {
		fPathElements += it.Current ();
	}
	return (*this);
}


#if		qMacOS
void	PathName::ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize, MacOS_ExtendedParamBlockHeader& paramBlkHeader) const
{
	Require (bigBufSize >= 256);

	if (*this == kBad) {
		FileSystem::sException.Raise ();
	}
	Assert (fPathElements.GetLength () > 0);

	bigBuf[0] = 0;

#if		0
// Must be compiler bug, or subtle problem with all these macros, but
// somehow these asserts are failing, and doing printfs of the values
// indicates they are right???
// LGP 7/9/92
//
	// See comment where we define the structure for why we do this...
	Assert (sizeof (paramBlkHeader) == 52);
	Assert (sizeof (MacOS_ExtendedParamBlockHeader) == 52);
	Assert (offsetof (MacOS_ExtendedParamBlockHeader, ioCompletion) == 12);
	Assert (offsetof (MacOS_ExtendedParamBlockHeader, ioNamePtr) == 18);
	Assert (offsetof (MacOS_ExtendedParamBlockHeader, ioVRefNum) == 22);
	Assert (offsetof (MacOS_ExtendedParamBlockHeader, ioDirID) == 48);
#endif

	/*
	 * Setup the ParamBlockHeader common fields.
	 *
	 * Because of the fact that we allways have to fill in the same params, the same
	 * way, and cuz of how MPW C defines the ParamBlockHeader (as a #define) we
	 * must fake things a bit to fill it in in only one place.
	 *
	 */
	memset (&paramBlkHeader, 0, sizeof (paramBlkHeader));

	paramBlkHeader.ioCompletion			=	Nil;	//	We do everything syncronously, by default...
	paramBlkHeader.ioNamePtr			=	bigBuf;	//	Init our bufPtr with argument bigBuf
	paramBlkHeader.ioVRefNum			=	0;		//	A bad value - must be override someplace
	paramBlkHeader.ioDirID				=	0;		//	A bad value - must be override someplace

	ForEach (PathComponent, it, fPathElements) {
		it.Current ().ApplySelfToPartialOSPathname (bigBuf, bigBufSize, paramBlkHeader);
	}

	/*
	 * Some quick sanity checking to be sure we have a properly formatted
	 * pathName.
	 */
	Assert (paramBlkHeader.ioNamePtr == bigBuf);
	if ((paramBlkHeader.ioVRefNum == 0) and (bigBuf[0] == 0)) {
		FileSystem::sException.Raise ();
	}
}

void	PathName::ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize, WDPBRec& paramBlkHeader) const
{
	memset (&paramBlkHeader, 0, sizeof (paramBlkHeader));
	// all these param things are the same since we only look at ParamBlock part...
	ApplySelfToPartialOSPathname (bigBuf, bigBufSize, (MacOS_ExtendedParamBlockHeader&)paramBlkHeader);
}

void	PathName::ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize, HParamBlockRec& paramBlkHeader) const
{
	memset (&paramBlkHeader, 0, sizeof (paramBlkHeader));
	// all these param things are the same since we only look at ParamBlock part...
	ApplySelfToPartialOSPathname (bigBuf, bigBufSize, (MacOS_ExtendedParamBlockHeader&)paramBlkHeader);
}

void	PathName::ApplySelfToPartialOSPathname (unsigned char* bigBuf, size_t bigBufSize, CInfoPBRec& paramBlkHeader) const
{
	memset (&paramBlkHeader, 0, sizeof (paramBlkHeader));
	// all these param things are the same since we only look at ParamBlock part...
	ApplySelfToPartialOSPathname (bigBuf, bigBufSize, (MacOS_ExtendedParamBlockHeader&)paramBlkHeader);
}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
void	PathName::ApplySelfToPartialOSPathname (String& pathSoFar, char separatorChar) const
{
	pathSoFar = "";
	ForEach (PathComponent, it, fPathElements) {
		it.Current ().ApplySelfToPartialOSPathname (pathSoFar, separatorChar);
	}
}
#endif








/*
 ********************************************************************************
 ************************************** operator+ *******************************
 ********************************************************************************
 */
PathName	operator+ (const PathName& lhs, const PathName& rhs)
{
	PathName	result	=	lhs;
	result += rhs;
	return (result);
}

PathName	operator+ (const PathName& pathName, const PathComponent& pathComponent)
{
	return (PathName (pathName, pathComponent));
}

PathName	operator+ (const PathComponent& pathComponent, const PathName& pathName)
{
	return (PathName (pathComponent, pathName));
}

PathName	operator+ (const PathComponent& pathComponent1, const PathComponent& pathComponent2)
{
	return (PathName (pathComponent1, pathComponent2));
}








/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */

Boolean	operator== (const PathName& lhs, const PathName& rhs)
{
	/*
	 * For lack of a better definition, define in terms of string equality
	 * of the components...
	 */
	if (lhs.GetComponents ().GetLength () != rhs.GetComponents ().GetLength ()) {
		return (False);
	}
	
	ForEachT (SequenceIterator, PathComponent, it, lhs) {
		if (it.Current ().GetName () != rhs.GetComponents ()[it.CurrentIndex ()].GetName ()) {
			return (False);
		}
	}
	return (True);
}










/*
 ********************************************************************************
 ************************************* operator!= *******************************
 ********************************************************************************
 */

Boolean	operator!= (const PathName& lhs, const PathName& rhs)
{
	return (not (lhs == rhs));		// define using == operator...
}








/*
 ********************************************************************************
 ********************************* iostream operators ***************************
 ********************************************************************************
 */

istream&	operator>> (istream& in, PathName& pathName)
{
	String	s;
	in >> s;
	if (in) {
		pathName = FileSystem::Get ().ParseStringToPathName (s);
	}
	return (in);
}

ostream&	operator<< (ostream& out, const PathName& pathName)
{
	out << FileSystem::Get ().PathNameToString (pathName);
	return (out);
}








/*
 ********************************************************************************
 ********************************** FileException *******************************
 ********************************************************************************
 */

#if		!qMacOS
const	int	noErr = 0;
#endif	/*!qMacOS*/

FileException::FileException ():
	fErr (noErr)
{
}

FileException::FileException (const FileException& isAnException):
	Exception (isAnException),
	fErr (noErr)
{
}

void	FileException::Raise ()
{
#if		qDebug
	gDebugStream << "Throwing file exception ..." << endl;
#endif	/*qDebug*/
	if (Exception::GetCurrent () != this) {		// if were not the current exception return something
												// indicating unknown error, otherwise preserve our
												// error #
		fErr = noErr;
	}
	Exception::Raise ();
}

void	FileException::Raise (int osErr)
{
	fErr = osErr;
#if		qDebug
	gDebugStream << "Throwing file exception (#" << osErr << ")..." << endl;
#endif	/*qDebug*/
	Exception::Raise ();
}

int	FileException::GetErrNumber () const
{
	return (fErr);
}










/*
 ********************************************************************************
 ***************************** FileNameBadCharacter *****************************
 ********************************************************************************
 */

FileNameBadCharacter::FileNameBadCharacter ():
	fBadChar ('\0')
{
}

FileNameBadCharacter::FileNameBadCharacter (const FileException& isAnException):
	FileException (isAnException),
	fBadChar ('\0')
{
}

void	FileNameBadCharacter::Raise ()
{
	Exception::Raise ();
}

void	FileNameBadCharacter::Raise (Character badChar)
{
	fBadChar = badChar;
	Raise ();
}










/*
 ********************************************************************************
 ******************************* AbstractFileSystem *****************************
 ********************************************************************************
 */

FileException			AbstractFileSystem::sException;
FileException			AbstractFileSystem::sOperationNotSupported				=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sPathComponentNotSupported			=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sFileNotFound						=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sTooManyFilesOpen					=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sFileExists							=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sFileNameTooLong					=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sAccessPermissionDenied				=	AbstractFileSystem::sException;
FileNameBadCharacter	AbstractFileSystem::sFileNameBadCharacter				=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sDirectoryOperationOnNonDirectory	=	AbstractFileSystem::sException;
FileException			AbstractFileSystem::sPlainFileOperationOnDirectory		=	AbstractFileSystem::sException;


AbstractFileSystem::AbstractFileSystem ()
{
}

void	AbstractFileSystem::Validate (const PathName& pathName) const
{
	Validate_ (pathName);
}

PathName	AbstractFileSystem::ParseStringToPathName (const String& pathName) const
{
	return (ParseStringToPathName_ (pathName));
}

String	AbstractFileSystem::PathNameToString (const PathName& pathName) const
{
	return (PathNameToString_ (pathName));
}

PathName	AbstractFileSystem::GetFullPathName (const PathName& pathName) const
{
	return (GetFullPathName_ (pathName));
}

AbstractFileSystem::FileType	AbstractFileSystem::GetFileType (const PathName& pathName) const
{
	return (GetFileType_ (pathName));
}

off_t	AbstractFileSystem::GetSize (const PathName& pathName) const
{
	return (GetSize_ (pathName));
}

Boolean		AbstractFileSystem::Exists (const PathName& pathName) const
{
	return (Exists_ (pathName));
}

DateTime	AbstractFileSystem::GetLastModificationTime (const PathName& pathName) const
{
	return (GetLastModificationTime_ (pathName));
}

void		AbstractFileSystem::Create (const PathName& pathName)
{
	Create_ (pathName);
}

void		AbstractFileSystem::CreateDirectory (const PathName& pathName)
{
	CreateDirectory_ (pathName);
}

void		AbstractFileSystem::Delete (const PathName& pathName)
{
	Delete_ (pathName);
}

void	AbstractFileSystem::Rename (const PathName& from, const PathName& to)
{
	Rename_ (from, to);
}

PathName	AbstractFileSystem::GetParentDirectory (const PathName& pathName) const
{
	return (GetParentDirectory_ (pathName));
}

UInt32		AbstractFileSystem::CountContainedItems (const PathName& pathName) const
{
	return (CountContainedItems_ (pathName));
}

#if		qRealTemplatesAvailable
Iterator<PathComponent>	AbstractFileSystem::MakeMountedVolumeIterator () const
#else
Iterator(PathComponent)	AbstractFileSystem::MakeMountedVolumeIterator () const
#endif
{
	return (MakeMountedVolumeIterator_ ());
}

#if		qRealTemplatesAvailable
Iterator<PathComponent>	AbstractFileSystem::MakeDirectoryIterator (const PathName& directory) const
#else
Iterator(PathComponent)	AbstractFileSystem::MakeDirectoryIterator (const PathName& directory) const
#endif
{
	return (MakeDirectoryIterator_ (directory));
}

PathName	AbstractFileSystem::GetCurrentDirectory () const
{
	return (GetCurrentDirectory_ ());
}

void	AbstractFileSystem::SetCurrentDirectory (const PathName& pathName)
{
	SetCurrentDirectory_ (pathName);
}

PathName	AbstractFileSystem::GetTemporaryDirectory () const
{
	return (GetTemporaryDirectory_ ());
}

PathName	AbstractFileSystem::GetPreferencesDirectory () const
{
	return (GetPreferencesDirectory_ ());
}

AbstractFileSystem::FileDescriptor	AbstractFileSystem::Open (const PathName& pathName, int openModes)
{
	return (Open_ (pathName, openModes));
}













/*
 ********************************************************************************
 *************************** AbstractFileSystem_MacOS ***************************
 ********************************************************************************
 */

FileException			AbstractFileSystem_MacOS::sFileLocked						=	AbstractFileSystem_MacOS::sException;
FileException			AbstractFileSystem_MacOS::sFileProjectorLocked				=	AbstractFileSystem_MacOS::sException;

FileException			AbstractFileSystem_MacOS::sResourceNotFound					=	AbstractFileSystem_MacOS::sException;
FileException			AbstractFileSystem_MacOS::sNoResourceFork					=	AbstractFileSystem_MacOS::sException;


AbstractFileSystem_MacOS::AbstractFileSystem_MacOS ():
	AbstractFileSystem ()
{
}

off_t	AbstractFileSystem_MacOS::GetDataForkSize (const PathName& pathName) const
{
	return (GetDataForkSize_ (pathName));
}

off_t	AbstractFileSystem_MacOS::GetResourceForkSize (const PathName& pathName) const
{
	return (GetResourceForkSize_ (pathName));
}

void	AbstractFileSystem_MacOS::SetLastModificationTime (const PathName& pathName, const DateTime& modificationTime)
{
	SetLastModificationTime_ (pathName, modificationTime);
}

DateTime	AbstractFileSystem_MacOS::GetCreationTime (const PathName& pathName) const
{
	return (GetCreationTime_ (pathName));
}

void	AbstractFileSystem_MacOS::SetCreationTime (const PathName& pathName, const DateTime& creationTime)
{
	SetCreationTime_ (pathName, creationTime);
}

UInt32	AbstractFileSystem_MacOS::GetCreator (const PathName& pathName) const
{
	return (GetCreator_ (pathName));
}

void	AbstractFileSystem_MacOS::SetCreator (const PathName& pathName, UInt32 creator)
{
	SetCreator_ (pathName, creator);
}

UInt32	AbstractFileSystem_MacOS::GetType (const PathName& pathName) const
{
	return (GetType_ (pathName));
}

void	AbstractFileSystem_MacOS::SetType (const PathName& pathName, UInt32 type)
{
	SetType_ (pathName, type);
}

Boolean	AbstractFileSystem_MacOS::GetLocked (const PathName& pathName) const
{
	return (GetLocked_ (pathName));
}

void	AbstractFileSystem_MacOS::SetLocked (const PathName& pathName, Boolean locked)
{
	SetLocked_ (pathName, locked);
}

Boolean	AbstractFileSystem_MacOS::GetVisible (const PathName& pathName) const
{
	return (GetVisible_ (pathName));
}

void	AbstractFileSystem_MacOS::SetVisible (const PathName& pathName, Boolean visible)
{
	SetVisible_ (pathName, visible);
}

Boolean	AbstractFileSystem_MacOS::Ejectable (const PathName& pathName)
{
	return (Ejectable_ (pathName));
}

void	AbstractFileSystem_MacOS::Eject (const PathName& pathName)
{
	Eject (pathName);
}

PathName	AbstractFileSystem_MacOS::GetBootVolume () const
{
	return (GetBootVolume_ ());
}

PathName	AbstractFileSystem_MacOS::GetCurrentSFGetFileDirectory () const
{
	return (GetCurrentSFGetFileDirectory_ ());
}

PathName	AbstractFileSystem_MacOS::GetSystemDirectory () const
{
	return (GetSystemDirectory_ ());
}

PathName	AbstractFileSystem_MacOS::GetDeskTopDirectory (const PathName& forVolume) const
{
	return (GetDeskTopDirectory_ (forVolume));
}

AbstractFileSystem::FileDescriptor	AbstractFileSystem_MacOS::Open (const PathName& pathName,
										int openModes, UInt32 creator, UInt32 fileType)
{
	return (Open_ (pathName, openModes, creator, fileType));
}

void	AbstractFileSystem_MacOS::Create (const PathName& pathName, UInt32 creator,
			UInt32 fileType)
{
	Create_ (pathName, creator, fileType);
}

short	AbstractFileSystem_MacOS::OpenResFile (const PathName& pathName, int openModes)
{
	return (OpenResFile_ (pathName, openModes));
}

short	AbstractFileSystem_MacOS::GetVRefNum (const PathName& pathName) const
{
	return (GetVRefNum_ (pathName));
}

long	AbstractFileSystem_MacOS::GetDirID (const PathName& pathName) const
{
	return (GetDirID_ (pathName));
}

AbstractFileSystem::FileDescriptor	AbstractFileSystem_MacOS::Open_ (const PathName& pathName, int openModes)
{
	// call our new overloaded version which will in-turn call open_/4
	return (Open (pathName, openModes));
}

void	AbstractFileSystem_MacOS::Create_ (const PathName& pathName)
{
	// call our new overloaded version which will in-turn call Create_/3
	Create (pathName);
}









/*
 ********************************************************************************
 *************************** AbstractFileSystem_UnixOS **************************
 ********************************************************************************
 */

AbstractFileSystem_UnixOS::AbstractFileSystem_UnixOS ():
	AbstractFileSystem ()
{
}

mode_t	AbstractFileSystem_UnixOS::GetPermissions (const PathName& pathName) const
{
	return (GetPermissions_ (pathName));
}

void	AbstractFileSystem_UnixOS::SetPermissions (const PathName& pathName, mode_t permissions)
{
	SetPermissions_ (pathName, permissions);
}

AbstractFileSystem::FileDescriptor	AbstractFileSystem_UnixOS::Open (const PathName& pathName,
	int openModes, mode_t mode)
{
	return (Open_ (pathName, openModes, mode));
}

void	AbstractFileSystem_UnixOS::Create (const PathName& pathName, mode_t mode)
{
	Create_ (pathName, mode);
}

void	AbstractFileSystem_UnixOS::CreateDirectory (const PathName& pathName, mode_t mode)
{
	CreateDirectory_ (pathName, mode);
}

AbstractFileSystem::FileDescriptor	AbstractFileSystem_UnixOS::Open_ (const PathName& pathName,
	int openModes)
{
	// call our new overloaded version which will in-turn call open_/4
	return (Open (pathName, openModes));
}

void	AbstractFileSystem_UnixOS::Create_ (const PathName& pathName)
{
	// call our new overloaded version which will in-turn call Create_/3
	Create (pathName);
}

void	AbstractFileSystem_UnixOS::CreateDirectory_ (const PathName& pathName)
{
	// call our new overloaded version which will in-turn call CreateDirectoryDirectory_/3
	CreateDirectory (pathName);
}









/*
 ********************************************************************************
 ***************************** AbstractFileSystem_DOS ***************************
 ********************************************************************************
 */

AbstractFileSystem_DOS::AbstractFileSystem_DOS ():
	AbstractFileSystem ()
{
}

Boolean	AbstractFileSystem_DOS::GetVisible (const PathName& pathName) const
{
	return (GetVisible_ (pathName));
}

void	AbstractFileSystem_DOS::SetVisible (const PathName& pathName, Boolean visible)
{
	SetVisible_ (pathName, visible);
}

Boolean	AbstractFileSystem_DOS::GetLocked (const PathName& pathName) const
{
	return (GetLocked_ (pathName));
}

void	AbstractFileSystem_DOS::SetLocked (const PathName& pathName, Boolean locked)
{
	SetLocked_ (pathName, locked);
}













/*
 ********************************************************************************
 ********************************** FileSystem_MacOS ****************************
 ********************************************************************************
 */

FileSystem_MacOS::FileSystem_MacOS ():
	AbstractFileSystem_MacOS ()
{
}

void	FileSystem_MacOS::Validate_ (const PathName& pathName) const
{
	// for now assume all pathnames are kosher - do more work to validate later...
	ForEach (PathComponent, it, pathName) {
		PathComponent	cur		=	it.Current ();
		String			curName	=	cur.GetName ();

// This is not quite right - wrong for "special Pathcomponents like : ::.
// Must merge this into virtual method of pathcomponents???
		if (curName.Contains (':') and curName.GetLength () > 2) {
			sFileNameBadCharacter.Raise (':');
		}
		if (curName.GetLength () > 31) {
			sFileNameTooLong.Raise ();
		}
	}
}

PathName	FileSystem_MacOS::ParseStringToPathName_ (const String& pathName) const
{
#if		qRealTemplatesAvailable
	Sequence<PathComponent>	elts;
#else
	Sequence(PathComponent)	elts;
#endif
	/*
	 *
	 * Iterate over the elements of the pathname. If we have non-delimiters, add them to
	 * the current component string. If we get a delimiter, we have some work todo.
	 *
	 * We must see if it is to be intpretted as being CurrentDirectoryRelative - this happens
	 * if we are at the start of the string.
	 *
	 * Or, if it is an ordinary seperator of components - this happens in the middle of a string,
	 * but with no colons before it.
	 *
	 * Or if it is at the end of the string, with no colons before it, in which case it is simply
	 * a hint that we are looking at a directory, but we ignore it.
	 *
	 * One special case is that if we get a simple path name, with no prefixing :, and no trailing : then
	 * we interpret this as being a CurrentDirectoryRelative path. This is differnet
	 * that the interpretation of PathName (PathComponent ("Fred")) - which would be a 
	 * root relative reference. This is because on the mac file system, saying just "Fred"
	 * is considered to be CurrentDirectoryRelative - UNLESS it ends in a :.
	 *
	 * < GOD, I hope I got this right --- >
	 *
	 * Note: we treat the special case of a Nil string correctly - an empty list of
	 * elements EQUALS PathName::kBad...
	 *
	 */
	String	tmpComponent;
	for (size_t i = 1; i <= pathName.GetLength (); i++) {
		if (pathName[i] == ':') {
			if (tmpComponent == "") {
				if (i == 1) {
					elts.Prepend (PathName::kDot);
				}
				else {
					elts.Prepend (PathName::kDotDot);
				}
			}
			else {
				elts.Append (PathComponent (tmpComponent));
				tmpComponent = "";
			}
		}
		else {
			tmpComponent += pathName[i];
		}
	}

	/*
	 * We ended without a ':' so the last string is also a component.
	 */
	if (tmpComponent != "") {
		elts.Append (PathComponent (tmpComponent));
	}

	/*
	 * Special case...
	 */
	if ((elts.GetLength () == 1) and (not pathName.Contains (':'))) {
		elts.Prepend (PathName::kDot);
	}
	return (elts);
}

String	FileSystem_MacOS::PathNameToString_ (const PathName& pathName) const
{
// This is wrong - and will probably require a change to the class PathComponent to get right... Similar to
// Apply... But what do we do for HFSPathguyts - I guess they must translate themselves to strings...!!!
//
	String	result;
	ForEach (PathComponent, it, pathName) {
		if ((result.GetLength () != 0) and (result [result.GetLength ()] != ':')) {
			result += ':';
		}
		result += it.Current ().GetName ();
	}
	return (result);
}

PathName	FileSystem_MacOS::GetFullPathName_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err;
	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		/*
		 * If get get an error here, ignore it as the file may not exist.
		 * If its directory does, we can still give a sensible answer.
		 * Just tell the OS to ignore name, and then munge the results...
		 */
		pbRec.dirInfo.ioFDirIndex = -1;		// ignore name - since file may not exist...
		if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
			FileSystem::sException.Raise (err);		// if we've still got an error, vrefnum/dirID must be bad
		}

		// hack to set things up so below code works - assume first bit worked...
		pbRec.dirInfo.ioDrParID = pbRec.dirInfo.ioDrDirID;
		pbRec.dirInfo.ioFlAttrib &= ~ioDirMask;		// shut off directory bit

		{
			// re-fillin namebuf, but dont disturb anything else...
			CInfoPBRec		pbRec2;
			pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec2);
		}
	}

	// hack to undo above and move us up one...
	pbRec.dirInfo.ioDrDirID = pbRec.dirInfo.ioDrParID;

	PathName	result	=	PathComponent (String (&nameBuf[1], size_t (nameBuf[0])));


	// Now crawl back up the path
	// while not at top of hierarchy
	pbRec.dirInfo.ioFDirIndex = -1;							// ignore ioNamePtr - use dirID & vrefnum
	for (; pbRec.dirInfo.ioDrDirID != 1; ) {
		if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
			FileSystem::sException.Raise (err);
		}
		// move up a directory (parent dir)
		pbRec.dirInfo.ioDrDirID = pbRec.dirInfo.ioDrParID;

		// Prepend this directories name onto the result...
		result = PathComponent (String (&nameBuf[1], size_t (nameBuf[0]))) + result;
	}

	return (result);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

AbstractFileSystem::FileType	FileSystem_MacOS::GetFileType_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}

/* Need to somehow detect symbolic links and return that!!! */
	return ((pbRec.dirInfo.ioFlAttrib & ioDirMask)? eDirectoryFileType: ePlainFileType);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (ePlainFileType);
#endif	/*qMacOS*/
}

off_t	FileSystem_MacOS::GetSize_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}

/* Need to somehow detect symbolic links and return that!!! */
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}
	else {
		Assert (pbRec.hFileInfo.ioFlLgLen >= 0);
		Assert (pbRec.hFileInfo.ioFlRLgLen >= 0);
		return (pbRec.hFileInfo.ioFlLgLen + pbRec.hFileInfo.ioFlRLgLen);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

Boolean		FileSystem_MacOS::Exists_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBGetCatInfoSync (&pbRec);

	switch (err) {
		case	noErr:	return (True);
		case	fnfErr:	return (False);
		default:		sException.Raise (err);
	}
	AssertNotReached (); return (False);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (False);
#endif	/*qMacOS*/
}

DateTime	FileSystem_MacOS::GetLastModificationTime_ (const PathName& pathName) const
{
	AssertNotImplemented ();
	return (DateTime (0, 0));
}

void	FileSystem_MacOS::CreateDirectory_ (const PathName& pathName)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBDirCreateSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

void		FileSystem_MacOS::Delete_ (const PathName& pathName)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	if ((err = ::PBHDeleteSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}
	AssertNotReached ();
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::Rename_ (const PathName& from, const PathName& to)
{
// NB: pascal OSErr PBHRename(HParmBlkPtr paramBlock,Boolean async); 
// can be used to rename a file, but not move it accross directories...
// Use catmove instead???
	AssertNotReached ();
}

PathName	FileSystem_MacOS::GetParentDirectory_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

// Not quite sure if we only do this for PBGetCatInfo, or if we ALWAYS want to
// do this...
if (nameBuf[0] == 0) {
	pbRec.hFileInfo.ioFDirIndex = -1;	//	IM-IV-156
}

	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}
	if (pbRec.hFileInfo.ioFlParID < fsRtDirID) {		// has no parent - root dir...
		sException.Raise ();
	}
	return (HFSPathComponent (pbRec.hFileInfo.ioVRefNum, pbRec.hFileInfo.ioFlParID));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

UInt32	FileSystem_MacOS::CountContainedItems_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (!(pbRec.dirInfo.ioFlAttrib & ioDirMask)) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	return (pbRec.dirInfo.ioDrNmFls);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}


// SB SCOPED BUG FOR q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
#if		qMacOS
	/*
	 * On the macintosh, there is considered to be no single root directory, but rather a set
	 * of root volumes, each of which has a root directory. This iterator accesses these
	 * volumes in-turn. Under UNIX, this just returns a single entity PathName ("/"). Under
	 * DOS, it returns each mounted (pseudo)disk, a:, b:, etc...
	 */
	class	MacMountedVolumeIterator : public
#if		qRealTemplatesAvailable
		IteratorRep<PathComponent>
#else
		IteratorRep(PathComponent)
#endif
	{
		public:
			MacMountedVolumeIterator ():
#if		qRealTemplatesAvailable
				IteratorRep<PathComponent> (),
#else
				IteratorRep(PathComponent) (),
#endif
				fCurrentIndex (0)
			{
			}

#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
virtual ~MacMountedVolumeIterator ();
virtual void ptbl_hack ();
#endif	/*qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG*/
			override	Boolean		Done () const
			{
				unsigned char 	nameBuf [1024];
				HParamBlockRec	aPBRec;

				nameBuf[0] = 0;
				aPBRec.volumeParam.ioNamePtr = nameBuf;
				aPBRec.volumeParam.ioVRefNum = 0;			// ignored since ioVolIndex>0
				Assert (fCurrentIndex == short (fCurrentIndex));	// no roundoff
				aPBRec.volumeParam.ioVolIndex = short (fCurrentIndex);
	
				OSErr	err	=	::PBHGetVInfoSync (&aPBRec);
				switch (err) {
					case	noErr:		return (False);
					case	nsvErr:		return (True);		// how else do you tell there are no more?
					default: 			FileSystem::sException.Raise (err);
				}
				AssertNotReached (); return (False);
			}
			override	Boolean		More ()
			{
				fCurrentIndex++;
				return (not Done ());
			}

			override	PathComponent	Current () const
			{
				unsigned char 	nameBuf [1024];
				HParamBlockRec	aPBRec;

				memset (&aPBRec, 0, sizeof (aPBRec));
				nameBuf[0] = 0;
				aPBRec.volumeParam.ioNamePtr = nameBuf;
				Assert (fCurrentIndex == short (fCurrentIndex));	// no roundoff
				/*
				 * IM-IV-129 says if volIndex > 0 ignore other params...
				 */
				aPBRec.volumeParam.ioVolIndex = short (fCurrentIndex);

				OSErr	err	=	::PBHGetVInfoSync (&aPBRec);
				if (err != noErr) {
					FileSystem::sException.Raise (err);
				}

// just as a quick hack - try returning the name - either way should work, but
// by refnum better for efficentcy, and a couple other reasons - renaming works better,
// etc.... Just a quick temp hack...

return (PathComponent (String (&aPBRec.volumeParam.ioNamePtr[1], size_t (aPBRec.volumeParam.ioNamePtr[0]))));

// dont ifdef so I get warnings that remind me this is a temp hack...


// if HFSPathComponent doesnt work - we may need to invent one for volumes...
				return (HFSPathComponent (aPBRec.volumeParam.ioVRefNum));
			}

			override	size_t	CurrentIndex () const
			{
				return (fCurrentIndex);
			}

#if		qRealTemplatesAvailable
			override	IteratorRep<PathComponent>*		Clone () const
#else
			override	IteratorRep(PathComponent)*		Clone () const
#endif
			{
				return (new MacMountedVolumeIterator (*this));
			}

		private:
			size_t	fCurrentIndex;
	};
#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
MacMountedVolumeIterator::~MacMountedVolumeIterator () {}
void MacMountedVolumeIterator::ptbl_hack () {}
#endif	/*qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG*/
#endif	/*qMacOS*/


#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_MacOS::MakeMountedVolumeIterator_ () const
#else
Iterator(PathComponent)	FileSystem_MacOS::MakeMountedVolumeIterator_ () const
#endif
{
#if		qMacOS
	return (new MacMountedVolumeIterator ());
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (Nil);
#endif	/*qMacOS*/
}



// SB SCOPED BUG FOR q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
#if		qMacOS
	class	MacDirectoryIterator : public
#if		qRealTemplatesAvailable
		IteratorRep<PathComponent>
#else
		IteratorRep(PathComponent)
#endif
	{
		public:
			MacDirectoryIterator (const PathName& directory):
#if		qRealTemplatesAvailable
				IteratorRep<PathComponent> (),
#else
				IteratorRep(PathComponent) (),
#endif
				fDirectory (directory),
				fCurrentIndex (0)
			{
			}
#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
virtual 	void ptbl_hack ();
#endif	/*qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG*/
			override	Boolean		Done () const
			{
				return Boolean (fCurrentIndex > FileSystem::Get ().CountContainedItems (fDirectory));
			}
			override	Boolean		More ()
			{
				fCurrentIndex++;
				return Boolean (fCurrentIndex <= FileSystem::Get ().CountContainedItems (fDirectory));
			}

			override	PathComponent	Current () const
			{
				unsigned char 	nameBuf [1024];
				CInfoPBRec		pbRec;
			
				fDirectory.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

				// Do getCatInfo on dir, which fills in dirID, then patch ioFDirIndex
				// to get it to look for the index subDir????
				OSErr	err	=	::PBGetCatInfoSync (&pbRec);
				if (err != noErr) {
					FileSystem::sException.Raise (err);
				}
				pbRec.dirInfo.ioFDirIndex = (short)fCurrentIndex;
				err	=	::PBGetCatInfoSync (&pbRec);
				if (err != noErr) {
					FileSystem::sException.Raise (err);
				}
				AssertNotNil (pbRec.dirInfo.ioNamePtr);
				Assert (pbRec.dirInfo.ioNamePtr == nameBuf);
				String theName = String (&nameBuf[1], nameBuf[0]);
				return (theName);
			}

			override	size_t	CurrentIndex () const
			{
				return (fCurrentIndex);
			}

#if		qRealTemplatesAvailable
			override	IteratorRep<PathComponent>*		Clone () const
#else
			override	IteratorRep(PathComponent)*		Clone () const
#endif
			{
				return (new MacDirectoryIterator (*this));
			}

		private:
			PathName	fDirectory;
			size_t		fCurrentIndex;
	};
#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
void MacDirectoryIterator::ptbl_hack () {}
#endif	/*qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG*/
#endif	/*qMacOS*/

#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_MacOS::MakeDirectoryIterator_ (const PathName& directory) const
#else
Iterator(PathComponent)	FileSystem_MacOS::MakeDirectoryIterator_ (const PathName& directory) const
#endif
{
#if		qMacOS
	if (not Exists (directory)) {
		sFileNotFound.Raise ();
	}
	if (GetFileType (directory) != eDirectoryFileType) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	if (GetFileType (directory) != eDirectoryFileType) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	return (new MacDirectoryIterator (directory));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (Nil);
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetCurrentDirectory_ () const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	WDPBRec			curWDPBRec;

	// zeroing uncessessary, but I'm paranoid - and til this stuff works flawlessly,
	// thats probably best... LGP 7/14/92
	memset (&curWDPBRec, 0, sizeof (curWDPBRec));
	memset (&nameBuf, 0, sizeof (nameBuf));
	curWDPBRec.ioNamePtr = nameBuf;

	OSErr err	=	::PBHGetVolSync (&curWDPBRec); 
	if (err != 0) {
		sException.Raise (err);
	}

	/*
	 * From 411 Help...
	 *
	 *	Warning:	IOVRefNum will return a working directory reference number
	 *          	(instead of the volume reference number) if, in the last call
	 *          	to PBSetVol or PBHSetVol, a working directory reference number
	 *          	was passed in this field.
	 *
	 *	The volume reference number of the volume on which the default directory exists is
	 *	returned in ioWDVRefNum. The directory ID of the default directory is returned in
	 *	ioWDDirID.
	 *
	 */

	/*
	 * Since we dont know if the ioVRefNum is a real vrefnum or a wdrefnum (we could figure it
	 * out) just use the REAL VREFNUM, and DIRID, which are guaranteed to be correct. Also, by
	 * not using a wdrefnum which could change, we are safe in not calling GetFullPathName ().
	 */
	return (HFSPathComponent (curWDPBRec.ioWDVRefNum, curWDPBRec.ioWDDirID));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

void		FileSystem_MacOS::SetCurrentDirectory_ (const PathName& pathName)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr err	=	::HSetVol (pbRec.dirInfo.ioNamePtr, pbRec.dirInfo.ioVRefNum, pbRec.dirInfo.ioDrDirID); 
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetTemporaryDirectory_ () const
{
#if		qMacOS
	if (OSConfiguration ().HasSpecialFolders ()) {
		VRefNum	foundVRefNum;
		DirID	foundDirID;
		/*
		 * Pass along True to ::FindFolder indicating that we should create the temporary folder
		 * if it does not exist. This is because system 7.0b1 seems to not actually create it
		 * until someone asks for it, and if were asking, we probably want it created!
		 * Somewhat bizarre behaviour on system 7's part, but I cannot see how creating it here
		 * is harmful. We fail gracefully, of course, if we cannot create it for some reason.
		 */
		OSErr	err	=	::FindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &foundVRefNum, &foundDirID);
		if (err == noErr) {
			return (HFSPathComponent (foundVRefNum, foundDirID));
		}
		// sometimes seems to not have one - somebody must create it first?
	}
	return (GetCurrentDirectory ());	// default to using the GetCurrentDirectory for temp files
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetPreferencesDirectory_ () const
{
#if		qMacOS
	if (OSConfiguration ().HasSpecialFolders ()) {
		VRefNum	foundVRefNum;
		DirID	foundDirID;
		/*
		 * Pass along True to ::FindFolder indicating that we should create the temporary folder
		 * if it does not exist. This is because system 7.0b1 seems to not actually create it
		 * until someone asks for it, and if were asking, we probably want it created!
		 * Somewhat bizarre behaviour on system 7's part, but I cannot see how creating it here
		 * is harmful. We fail gracefully, of course, if we cannot create it for some reason.
		 */
		OSErr	err	=	::FindFolder (kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &foundVRefNum, &foundDirID);
		if (err == noErr) {
			return (HFSPathComponent (foundVRefNum, foundDirID));
		}
		// sometimes seems to not have one - somebody must create it first?
	}
	return (GetSystemDirectory ());			// default to using the system folder for prefs files
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

off_t	FileSystem_MacOS::GetDataForkSize_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}

/* Need to somehow detect symbolic links and return that!!! */
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}
	else {
		Assert (pbRec.hFileInfo.ioFlLgLen >= 0);
		return (pbRec.hFileInfo.ioFlLgLen);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

off_t	FileSystem_MacOS::GetResourceForkSize_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;
	OSErr			err;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	if ((err = ::PBGetCatInfoSync (&pbRec)) != noErr) {
		sException.Raise (err);
	}

/* Need to somehow detect symbolic links and return that!!! */
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}
	else {
		Assert (pbRec.hFileInfo.ioFlRLgLen >= 0);
		return (pbRec.hFileInfo.ioFlRLgLen);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::SetLastModificationTime_ (const PathName& pathName, const DateTime& modificationTime) const
{
	AssertNotImplemented ();
}

DateTime	FileSystem_MacOS::GetCreationTime_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}

AssertNotImplemented ();	// almost right - need to convert date to right format...
//	return (pbRec.hFileInfo.ioFlFndrInfo.fdCreator);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (DateTime (0, 0));
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::SetCreationTime_ (const PathName& pathName, const DateTime& creationTime)
{
#if		qMacOS
AssertNotImplemented ();	// almost right - need to convert date to right format...
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBHGetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}

//	pbRec.fileParam.ioFlFndrInfo.fdCreator = creator;

	// hack - reset various params
	HParamBlockRec	savedPBRec	=	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	pbRec.fileParam.ioFlFndrInfo = savedPBRec.fileParam.ioFlFndrInfo;
	pbRec.fileParam.ioFlCrDat = savedPBRec.fileParam.ioFlCrDat;
	pbRec.fileParam.ioFlMdDat = savedPBRec.fileParam.ioFlMdDat;

	err = ::PBHSetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

UInt32		FileSystem_MacOS::GetCreator_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}

	return (pbRec.hFileInfo.ioFlFndrInfo.fdCreator);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

void		FileSystem_MacOS::SetCreator_ (const PathName& pathName, UInt32 creator)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBHGetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (pbRec.fileParam.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}

	pbRec.fileParam.ioFlFndrInfo.fdCreator = creator;

	// hack - reset various params
	HParamBlockRec	savedPBRec	=	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	pbRec.fileParam.ioFlFndrInfo = savedPBRec.fileParam.ioFlFndrInfo;
	pbRec.fileParam.ioFlCrDat = savedPBRec.fileParam.ioFlCrDat;
	pbRec.fileParam.ioFlMdDat = savedPBRec.fileParam.ioFlMdDat;

	err = ::PBHSetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

UInt32		FileSystem_MacOS::GetType_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}
	return (pbRec.hFileInfo.ioFlFndrInfo.fdType);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::SetType_ (const PathName& pathName, UInt32 type)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBHGetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (pbRec.fileParam.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}

	pbRec.fileParam.ioFlFndrInfo.fdType = type;

	// hack - reset various params
	HParamBlockRec	savedPBRec	=	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	pbRec.fileParam.ioFlFndrInfo = savedPBRec.fileParam.ioFlFndrInfo;
	pbRec.fileParam.ioFlCrDat = savedPBRec.fileParam.ioFlCrDat;
	pbRec.fileParam.ioFlMdDat = savedPBRec.fileParam.ioFlMdDat;

	err = ::PBHSetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

Boolean		FileSystem_MacOS::GetLocked_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (pbRec.dirInfo.ioFlAttrib & ioDirMask) {
		sPlainFileOperationOnDirectory.Raise ();
	}

	return Boolean (!!(pbRec.hFileInfo.ioFlAttrib & 0x1));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (False);
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::SetLocked_ (const PathName& pathName, Boolean locked)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err;
	if (locked) {
		err = ::PBHSetFLockSync (&pbRec);
	}
	else {
		err = ::PBHRstFLockSync (&pbRec);
	}
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

Boolean	FileSystem_MacOS::GetVisible_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	return (not (pbRec.hFileInfo.ioFlFndrInfo.fdFlags & fInvisible));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (False);
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::SetVisible_ (const PathName& pathName, Boolean visible)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	OSErr	err	=	::PBHGetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}

	if (visible) {
		pbRec.fileParam.ioFlFndrInfo.fdType &= ~fInvisible;
	}
	else {
		pbRec.fileParam.ioFlFndrInfo.fdType |= fInvisible;
	}

// not sure we restore enuf params??? here???
	// hack - reset various params
	HParamBlockRec	savedPBRec	=	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);
	pbRec.fileParam.ioFlFndrInfo = savedPBRec.fileParam.ioFlFndrInfo;
	pbRec.fileParam.ioFlCrDat = savedPBRec.fileParam.ioFlCrDat;
	pbRec.fileParam.ioFlMdDat = savedPBRec.fileParam.ioFlMdDat;

	err = ::PBHSetFInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

Boolean		FileSystem_MacOS::Ejectable_ (const PathName& pathName)
{
	AssertNotImplemented ();
	return (False);
}

void	FileSystem_MacOS::Eject_ (const PathName& pathName)
{
	AssertNotImplemented ();
}

PathName	FileSystem_MacOS::GetBootVolume_ () const
{
#if		qMacOS
	// I think this just means the volume that has the blessed folder???
	// fsRtDirID is defined to be the top level directory for a given volume
	if (OSConfiguration ().HasSpecialFolders ()) {
		short	foundVRefNum;
		long	foundDirID;
		OSErr	err	=	::FindFolder (kOnSystemDisk, kSystemFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
		if (err == noErr) {
			return (HFSPathComponent (foundVRefNum, fsRtDirID));
		}
	}
	AssertNotReached (); return (PathName::kBad);	// shouldn't happen, I don't think?
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetCurrentSFGetFileDirectory_ () const
{
#if		qMacOS
	// Ugly and unsafe, but the only way I know???
	return (HFSPathComponent (*((short*)SFSaveDisk), *((long*)CurDirStore)));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetSystemDirectory_ () const
{
#if		qMacOS
	if (OSConfiguration ().HasSpecialFolders ()) {
		short	foundVRefNum;
		long	foundDirID;
		OSErr	err	=	::FindFolder (kOnSystemDisk, kSystemFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
		if (err == noErr) {
			return (HFSPathComponent (foundVRefNum, foundDirID));
		}
		else {
			AssertNotReached ();		// shouldn't happen, I don't think?
		}
	}

	/*
	 * According to IM V-7,8 do this to find the system folder. Only for compatability with systems
	 * older than system 7.
	 */
	SysEnvRec	aSysEnvRec;
	OSErr err = ::SysEnvirons (curSysEnvVers, &aSysEnvRec);
	if (err != noErr) {
		sException.Raise (err);
	}
	return (HFSPathComponent (aSysEnvRec.sysVRefNum, fsRtDirID));
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

PathName	FileSystem_MacOS::GetDeskTopDirectory_ (const PathName& forVolume) const
{
#if		qMacOS

AssertNotReached ();
#if 0
	if (OSConfiguration ().HasSpecialFolders ()) {
		short	foundVRefNum;
		long	foundDirID;
		Assert (forVolume.GetVRefNum () != 0);
		OSErr	err	=	::FindFolder (forVolume.GetVRefNum (), kDesktopFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
		if (err == noErr) {
			return (PathName (foundVRefNum, foundDirID));
		}
		else {
			AssertNotReached ();		// shouldn't happen, I don't think?
		}
	}
#endif

	/*
	 * If for some reason, we cannot find the desktop folder- for example, because we are
	 * running system 6, just use the root folder for that purpose. Its the closest approximation.
	 */
	return (forVolume);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (PathName::kBad);
#endif	/*qMacOS*/
}

AbstractFileSystem::FileDescriptor	FileSystem_MacOS::Open_ (const PathName& pathName, int openModes, UInt32 creator, UInt32 fileType)
{
#if		qMacOS
	/*
	 * This code is really a mess - not proplery done w.r.t. exception handling.
	 *
	 * The main reason this code is so hairy is that apple gives us no way to get a
	 * file descriptor (what is returned from open, and used by iostream library)
	 * from an accesspath (what is returned by apples file system open). And the UNIX
	 * open does not work with wdrefnums, etc, just pathnames. So, we must carefully
	 * set the current directory to be the dir in which the file resides, and then
	 * do the open, and be sure to restore things afterwards. What a nightmare!
	 */

	PathName	oldDir	=	GetCurrentDirectory ();

	try {
		/*
		 * Actually open the file. Copy all the components into a tmp list, and
		 * the initial segment become the path to the dir, and the last one is
		 * the file name we open.
		 */
		PathName	openDir	=	pathName.GetPrefix ();

		if (openDir != PathName::kBad) {			// if relative to current Directory
			/*
			 * Set the current directory to be our consed up wdrefnum
			 */
			SetCurrentDirectory (openDir);
		}

		String	rName	=	pathName.GetFileName ().GetName ();
		char*	tmp		=	rName.ToCString ();
		Assert ((openModes & O_EXCL) == 0);		// we don't properly support this flag - requires a bit
												// more work...
		int	fd	=	::open (tmp, openModes & ~O_CREAT);
		if (fd < 0) {
			if (openModes & O_CREAT) {
				osStr255	tttmmmpp;
				OSErr err = ::HCreate (GetVRefNum (openDir), GetDirID (openDir), rName.ToPStringTrunc (tttmmmpp, sizeof (tttmmmpp)), creator, fileType);
				if (err != noErr) {
					sException.Raise (err);
				}
				fd	=	::open (tmp, openModes & ~O_CREAT);
			}
#if		qDebug
			if (fd < 0) {
				gDebugStream << "Open of '" << tmp << "' mode=" << openModes << " failed (fd = " << fd << ")" << endl;
			}
#endif	/*qDebug*/
			if (fd < 0) {
				delete (tmp);		// delete tmp here since we will definitely raise an exception below...
				switch (errno) {
					case	ENOENT:		sFileNotFound.Raise ();
					case	EMFILE:		sTooManyFilesOpen.Raise ();
					case	EEXIST:		sFileExists.Raise ();
					default:			sException.Raise ();
				}
				AssertNotReached ();
			}
		}
		delete (tmp);
		SetCurrentDirectory (oldDir);
		return (fd);
	}
	catch () {
		SetCurrentDirectory (oldDir);
		_this_catch_->Raise ();		// propogate exception
	}
	AssertNotReached (); return (0);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

AbstractFileSystem::FileDescriptor	FileSystem_MacOS::Open_ (const PathName& pathName, int openModes)
{
	// override to avoid cfront warning only...
	return (AbstractFileSystem_MacOS::Open_ (pathName, openModes));
}

void	FileSystem_MacOS::Create_ (const PathName& pathName, UInt32 creator, UInt32 fileType)
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	HParamBlockRec	pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBHCreateSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}

	// maybe should find a better way to do this all at once...
	SetCreator (pathName, creator);
	SetType (pathName, fileType);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qMacOS*/
}

void	FileSystem_MacOS::Create_ (const PathName& pathName)
{
	// override to avoid cfront warning only...
	AbstractFileSystem_MacOS::Create_ (pathName);
}

short	FileSystem_MacOS::OpenResFile_ (const PathName& pathName, int openModes)
{
// Not sure right - re-examine...
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	// See if we need to create it first...
	if (openModes & O_CREAT) {
		::HCreateResFile (pbRec.dirInfo.ioVRefNum, pbRec.dirInfo.ioDrDirID, nameBuf);
		// should check resError () but be careful - ok if already exists!
	}


	// really should straighten out permissions stuff!!! For now, simulate ::open () style permiusstions
	//
	char permission	=	0;
	// not totally sure this is portable??!! even across mac compilers...
	switch (openModes & 0x3) {
		case		O_RDONLY:	permission	=	fsRdPerm; break;
		case		O_WRONLY:	permission	=	fsWrPerm; break;
		case		O_RDWR:		permission	=	fsRdWrPerm; break;
		default:				AssertNotReached ();
	}

	short	fd	=	::HOpenResFile (pbRec.dirInfo.ioVRefNum, pbRec.dirInfo.ioDrDirID, nameBuf, permission);
	if (fd == -1) {
		OSErr	err	=	::ResError ();
		if (err == eofErr) {
			FileSystem::sNoResourceFork.Raise ();
		}
		else {
			FileSystem::sException.Raise (err);
		}
	}
	return (fd);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

short	FileSystem_MacOS::GetVRefNum_ (const PathName& pathName) const
{
// Not sure right - re-examine...
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	return (pbRec.dirInfo.ioVRefNum);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}

long	FileSystem_MacOS::GetDirID_ (const PathName& pathName) const
{
#if		qMacOS
	unsigned char 	nameBuf [1024];
	CInfoPBRec		pbRec;

	pathName.ApplySelfToPartialOSPathname (nameBuf, sizeof (nameBuf), pbRec);

	OSErr	err	=	::PBGetCatInfoSync (&pbRec);
	if (err != 0) {
		sException.Raise (err);
	}
	if (!(pbRec.dirInfo.ioFlAttrib & ioDirMask)) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	return (pbRec.dirInfo.ioDrDirID);
#else	/*qMacOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qMacOS*/
}











/*
 ********************************************************************************
 ********************************** FileSystem_UnixOS ***************************
 ********************************************************************************
 */

FileSystem_UnixOS::FileSystem_UnixOS ():
	AbstractFileSystem_UnixOS ()
{
}

void	FileSystem_UnixOS::Validate_ (const PathName& pathName) const
{
	// for now assume all pathnames are kosher - do more work to validate later...
	ForEach (PathComponent, it, pathName) {
		PathComponent	cur		=	it.Current ();
		String			curName	=	cur.GetName ();
		if (curName.Contains ('/')) {
			sFileNameBadCharacter.Raise ('/');
		}
#if 0
// use pathConf...
		if (curName.GetLength () > 31) {
			sFileNameTooLong.Raise ();
		}
#endif
	}
}

PathName	FileSystem_UnixOS::ParseStringToPathName_ (const String& pathName) const
{
// quick hack - not really right - does not work with rel path names, ., or .. yet...
	PathName		result		=	PathName::kBad;
	size_t	lastSlash	=	0;					// carefully chosen so loop works...
	for (size_t i = 1; i <= pathName.GetLength (); i++) {
		if (pathName [i] == '/') {
//			String	item	=	pathName.SubString (lastSlash+1, i-lastSlash);
			String	item	=	pathName.SubString (lastSlash+1, i-lastSlash-1);
			lastSlash = i;
			if (item.GetLength () > 0) {
				result += PathComponent (item);
			}
		}
	}
	if (lastSlash != pathName.GetLength ()) {
//		result += PathComponent (pathName.SubString (lastSlash+1));
		result += PathComponent (pathName.SubString (lastSlash+1, i-lastSlash-1));
	}
	return (result);
}

String	FileSystem_UnixOS::PathNameToString_ (const PathName& pathName) const
{
#if		qUnixOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	return (nameBuf);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return ("");
#endif	/*qUnixOS*/
}

PathName	FileSystem_UnixOS::GetFullPathName_ (const PathName& pathName) const
{
/*
 * We must specify the semantics of this routine more carefully in the
 * header. I belive they should be that it looks at the filesystem, and
 * returns a normalized pathname. In which case this implementation
 * is all wrong. Instead, recurively call stat()  winding up the parent
 * chain, and return the accumulated path.
 */



	if (pathName == PathName::kBad) {
		return (GetCurrentDirectory ());
	}

	String	nameBuf;
#if		qDOS || qWinNT || qWinOS || qUnixOS
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
#endif
	Assert (nameBuf.GetLength () > 0);	// could only happen with
										// PathName::kBad?
	if (nameBuf[1].GetAsciiCode () == '/') {
		/*
		 * In this case, it already is a full path name. We probably should
		 * take this opportunity to normalize it, but that would be somewhat
		 * tricky, and not worth while right now.
		 */
		return (pathName);
	}
	else {
		/*
		 * The pathname was relative, so just prepend the current directory.
		 * Again, just as above, this would be a good time to normalize, but
		 * we leave that for later.
		 */
		return (GetCurrentDirectory () + pathName);
	}
}

AbstractFileSystem::FileType	FileSystem_UnixOS::GetFileType_ (const PathName& pathName) const
{
#if		qUnixOS
	struct	stat s;
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
#if		qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope
	int j = stat (tmp, &s);
#else
	int j = ::stat (tmp, &s);
#endif
	delete (tmp);
	if (j != 0) {
		sException.Raise (j);
	}
// This is insufficient...

	return (S_ISDIR (s.st_mode)? eDirectoryFileType: ePlainFileType);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (ePlainFileType);
#endif	/*qUnixOS*/
}

off_t	FileSystem_UnixOS::GetSize_ (const PathName& pathName) const
{
#if		qUnixOS
	struct	stat s;
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
#if		qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope
	int j = stat (tmp, &s);
#else
	int j = ::stat (tmp, &s);
#endif
	delete (tmp);
	if (j != 0) {
		sException.Raise (j);
	}
	if (not S_ISREG (s.st_mode)) {
		sPlainFileOperationOnDirectory.Raise ();		// not quite right - but close...
	}
	return (s.st_size);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qUnixOS*/
}

Boolean		FileSystem_UnixOS::Exists_ (const PathName& pathName) const
{
#if		qUnixOS
	struct	stat s;
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
#if		qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope
	int j = stat (tmp, &s);
#else
	int j = ::stat (tmp, &s);
#endif
	delete (tmp);
	if (j != 0) {
		return (False);		// half assed - really must examine errno???
	}
	return (True);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (False);
#endif	/*qUnixOS*/
}

DateTime	FileSystem_UnixOS::GetLastModificationTime_ (const PathName& pathName) const
{
	AssertNotImplemented ();
	return (Date (0), Time (0));
}

void		FileSystem_UnixOS::Delete_ (const PathName& pathName)
{
#if		qUnixOS
// This code is not right - try unlink and then rmdir!!!! Also, look at errno!!!
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
	int j = ::unlink (tmp);
	delete (tmp);
	if (j != 0) {
		sException.Raise (j);
	}
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

void	FileSystem_UnixOS::Rename_ (const PathName& from, const PathName& to)
{
	AssertNotReached ();
}

PathName	FileSystem_UnixOS::GetParentDirectory_ (const PathName& pathName) const
{
	/*
	 * Count on GetFullPathName to expand the ".." for us, and return a proper
	 * pathname.
	 */
	return (GetFullPathName (pathName + PathName::kDotDot));
}

UInt32		FileSystem_UnixOS::CountContainedItems_ (const PathName& pathName) const
{
	// Does not appear to be any way to compute this directly, except counting
	// as we iterate (at least according to POSIX manual).
	UInt32	count	=	0;
	ForEach (PathComponent, it, MakeDirectoryIterator (pathName)) {
		count++;
	}
	return (count);
}

#if		qUnixOS
	/*
	 * In UNIX there is not a separate notion of volumes, like there is in DOS, VMS, and
	 * the MacOS file systems. In UNIX, everything is some path off of root ("/"), and so
	 * for compatability sake we simple build an iterator that produces this 1 "volume".
	 *
	 * This class must be declared outside of the MakeMountedVolumeIterator_() function
	 * because of its call to clone itself - cannot see own classes name there???
	 * Perhaps a bug with gcc 2.2.1???? This is OK, for now....
	 */
	class	UNIXMountedVolumeIterator : public
#if		qRealTemplatesAvailable
		IteratorRep<PathComponent>
#else
		IteratorRep(PathComponent)
#endif
	{
		public:
			UNIXMountedVolumeIterator ():
#if		qRealTemplatesAvailable
				IteratorRep<PathComponent> (),
#else
				IteratorRep(PathComponent) (),
#endif
				fCurrentIndex (0)
			{
			}

			override	Boolean	Done () const
			{
				return Boolean (fCurrentIndex > 1);
			}

			override	Boolean	More ()
			{
				fCurrentIndex++;
				return Boolean (fCurrentIndex == 1);
			}

			override	PathComponent	Current () const
			{
				Require (fCurrentIndex == 1);
				return (PathComponent ("/"));
			}

			override	size_t	CurrentIndex () const
			{
				return (fCurrentIndex);
			}

#if		qRealTemplatesAvailable
			override	IteratorRep<PathComponent>*		Clone () const
#else
			override	IteratorRep(PathComponent)*		Clone () const
#endif
			{
				return (new UNIXMountedVolumeIterator (*this));
			}

		private:
			size_t	fCurrentIndex;
	};
#endif	/*qUnixOS*/

#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_UnixOS::MakeMountedVolumeIterator_ () const
#else
Iterator(PathComponent)	FileSystem_UnixOS::MakeMountedVolumeIterator_ () const
#endif
{
#if		qUnixOS
	return (new UNIXMountedVolumeIterator ());
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

#if		qUnixOS
	/*
	 * Here we use the POSIX routines opendir/readdir/closedir to implement iterating
	 * thu the list of file names.
	 *		One subtle design point is that we throw out ".", and "..". This is for
	 * a number of reasons. First, it makes the iterator behave more like the mac version,
     * where there are no such magical directory entries. If someone wants to consider that
	 * they are there, they can always be magically added, either by a subclass, or a delegating
	 * class. Also, their presense makes more difficult writing (especially portably) recursive
	 * traversals of a directory tree because then the USERS code must skip these directories.
	 *
	 * This class must be declared outside of the MakeMountedVolumeIterator_() function
	 * because of its call to clone itself - cannot see own classes name there???
	 * Perhaps a bug with gcc 2.2.1???? This is OK, for now....
	 */
	class	UNIXDirectoryIterator : public
#if		qRealTemplatesAvailable
		IteratorRep<PathComponent>
#else
		IteratorRep(PathComponent)
#endif
	{
		public:
	  		UNIXDirectoryIterator (const PathName& directory):
#if		qRealTemplatesAvailable
				IteratorRep<PathComponent> (),
#else
				IteratorRep(PathComponent) (),
#endif
				fDirectory (directory),
				fDirStream (Nil),
				fCurrent (Nil),
				fCurrentIndex (0)
			{
				String	nameBuf;
				directory.ApplySelfToPartialOSPathname (nameBuf, '/');
				char*	tmp	=	nameBuf.ToCString ();
				fDirStream = ::opendir (tmp);
				delete (tmp);
			}

			~UNIXDirectoryIterator ()
			{
				if (fDirStream != Nil) {
					if (::closedir (fDirStream) == -1) {
						Exception ().Raise ();// should do better exception than this -
											  // see errno...
					}
				}
			}

			override	Boolean	Done () const
			{
				return Boolean (fCurrent == Nil);
			}

			override	Boolean	More ()
			{
				fCurrentIndex++;
				// Skip ., .. without counting them as extra files...
			Again:
				fCurrent = ::readdir (fDirStream);
				if (fCurrent != Nil and
					((::strcmp (fCurrent->d_name, ".") == 0) or
					 (::strcmp (fCurrent->d_name, "..") == 0)
					 )
					) {
					goto Again;
				}
				return Boolean (fCurrent != Nil);
			}

			override	PathComponent	Current () const
			{
				return (PathComponent (String (fCurrent->d_name)));
			}

			override	size_t	CurrentIndex () const
			{
				return (fCurrentIndex);
			}

#if		qRealTemplatesAvailable
			override	IteratorRep<PathComponent>*		Clone () const
#else
			override	IteratorRep(PathComponent)*		Clone () const
#endif
			{
				// cannot use X(X&) since DIR* and dirent* allocated by system
				// utilities, and have no API for copying...
				UNIXDirectoryIterator*	clone	=	new UNIXDirectoryIterator (fDirectory);
				// Dont really know a better way...
				while (clone->More () and clone->CurrentIndex () < CurrentIndex ())
					;
				return (clone);
			}

		private:
			PathName	fDirectory;
			DIR*		fDirStream;
			dirent*		fCurrent;
			size_t		fCurrentIndex;
	};
#endif	/*qUnixOS*/


#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_UnixOS::MakeDirectoryIterator_ (const PathName& directory) const
#else
Iterator(PathComponent)	FileSystem_UnixOS::MakeDirectoryIterator_ (const PathName& directory) const
#endif
{
#if		qUnixOS
	if (not Exists (directory)) {
		sFileNotFound.Raise ();
	}
	if (GetFileType (directory) != eDirectoryFileType) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	if (GetFileType (directory) != eDirectoryFileType) {
		sDirectoryOperationOnNonDirectory.Raise ();
	}
	return (new UNIXDirectoryIterator (directory));
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

PathName	FileSystem_UnixOS::GetCurrentDirectory_ () const
{
#if		qUnixOS
// This code is not right - should work in many cases, like LONG path, but
// not in case of ERROR conditions - look up in POSIX manual if this is
// right way???
	size_t	bufSize	=	1024;
	char*	buf		=	Nil;
	for (bufSize = 1024, buf = new char [bufSize]; getcwd (buf, bufSize) == Nil;
		bufSize*= 2, delete buf, buf = new char [bufSize])
			;
	AssertNotNil (buf);
	PathName	result	=	ParseStringToPathName (buf);
	delete (buf);
	return (result);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

void		FileSystem_UnixOS::SetCurrentDirectory_ (const PathName& pathName)
{
	AssertNotImplemented ();
}

PathName	FileSystem_UnixOS::GetTemporaryDirectory_ () const
{
	return (ParseStringToPathName ("/tmp"));
}

PathName	FileSystem_UnixOS::GetPreferencesDirectory_ () const
{
#if		qUnixOS
	char*	home	=	::getenv ("HOME");
	if (home == Nil) {
		sException.Raise ();
	}
	else {
		return (ParseStringToPathName (home));
	}
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
#endif	/*qUnixOS*/
	AssertNotReached (); return (PathName::kBad);
}

mode_t	FileSystem_UnixOS::GetPermissions_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (0);
}

void	FileSystem_UnixOS::SetPermissions_ (const PathName& pathName, mode_t permissions)
{
	AssertNotImplemented ();
}

AbstractFileSystem::FileDescriptor	FileSystem_UnixOS::Open_ (const PathName& pathName, int openModes, mode_t mode)
{
#if		qUnixOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
	::errno = 0;
	int	fd	=	::open (tmp, openModes, mode);
	delete (tmp);
	if (fd < 0) {
		int theErrno = errno;
		::errno = 0;
		switch (theErrno) {
			case	0:			AssertNotReached ();
			case	EACCES:		sAccessPermissionDenied.Raise ();
			case	EAGAIN:		sException.Raise (theErrno);
			default:			sException.Raise (theErrno);
		}
	}
	else {
		return (fd);
	}
	AssertNotReached (); return (0);
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

AbstractFileSystem::FileDescriptor	FileSystem_UnixOS::Open_ (const PathName& pathName, int openModes)
{
	// override to avoid cfront warning only...
	return (AbstractFileSystem_UnixOS::Open_ (pathName, openModes));
}

void	FileSystem_UnixOS::Create_ (const PathName& pathName, mode_t mode)
{
#if		qUnixOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
	int err = ::creat (tmp, mode);
	delete (tmp);
	if (err < 0) {
		int err = ::errno;
		::errno = 0;
		sException.Raise (err);
	}
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

void	FileSystem_UnixOS::Create_ (const PathName& pathName)
{
	// override to avoid cfront warning only...
	AbstractFileSystem_UnixOS::Create_ (pathName);
}

void		FileSystem_UnixOS::CreateDirectory_ (const PathName& pathName, mode_t mode)
{
#if		qUnixOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '/');
	char*	tmp	=	nameBuf.ToCString ();
	int err = ::mkdir (tmp, mode);
	delete (tmp);
	if (err < 0) {
		int err = ::errno;
		::errno = 0;
		sException.Raise (err);
	}
#else	/*qUnixOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qUnixOS*/
}

void		FileSystem_UnixOS::CreateDirectory_ (const PathName& pathName)
{
	// override to avoid cfront warning only...
	AbstractFileSystem_UnixOS::CreateDirectory_ (pathName);
}







/*
 ********************************************************************************
 ********************************** FileSystem_DOS ******************************
 ********************************************************************************
 */

FileSystem_DOS::FileSystem_DOS ():
	AbstractFileSystem_DOS ()
{
}

void	FileSystem_DOS::Validate_ (const PathName& pathName) const
{
	// for now assume all pathnames are kosher - do more work to validate later...
}

PathName	FileSystem_DOS::ParseStringToPathName_ (const String& pathName) const
{
#if		qDOS || qWinNT || qWinOS
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
#endif	/*qDOS || qWinNT || qWinOS*/
	AssertNotReached (); return (PathName::kBad);
}

String	FileSystem_DOS::PathNameToString_ (const PathName& pathName) const
{
#if		qDOS || qWinNT || qWinOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	return (nameBuf);
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return ("");
#endif	/*qDOS || qWinNT || qWinOS*/
}

PathName	FileSystem_DOS::GetFullPathName_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (PathName::kBad);
}

AbstractFileSystem::FileType	FileSystem_DOS::GetFileType_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (ePlainFileType);
}

off_t	FileSystem_DOS::GetSize_ (const PathName& pathName) const
{
#if		qDOS || qWinNT || qWinOS
	struct	stat s;
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	char*	tmp	=	nameBuf.ToCString ();
	int j = ::stat (tmp, &s);
	delete (tmp);
	if (j != 0) {
		sException.Raise (j);
	}
	if (not S_ISREG (s.st_mode)) {
		sPlainFileOperationOnDirectory.Raise ();		// not quite right - but close...
	}
	return (s.st_size);
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (0);
#endif	/*qDOS || qWinNT || qWinOS*/
}

Boolean		FileSystem_DOS::Exists_ (const PathName& pathName) const
{
#if		qDOS || qWinNT || qWinOS
	struct	stat s;
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	char*	tmp	=	nameBuf.ToCString ();
	int j = ::stat (tmp, &s);
	delete (tmp);
	if (j != 0) {
		return (False);		// half assed - really must examine errno???
	}
	return (True);
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (False);
#endif	/*qDOS || qWinNT || qWinOS*/
}

DateTime	FileSystem_DOS::GetLastModificationTime_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (DateTime (Date (0), Time (0)));
}

void	FileSystem_DOS::Create_ (const PathName& pathName)
{
	AssertNotImplemented ();
}

void	FileSystem_DOS::CreateDirectory_ (const PathName& pathName)
{
#if		qDOS || qWinNT || qWinOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	char*	tmp	=	nameBuf.ToCString ();
	int err = ::mkdir (tmp);
	delete (tmp);
	if (err < 0) {
		int err = ::errno;
		::errno = 0;
		sException.Raise (err);
	}
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qDOS || qWinNT || qWinOS*/
}

void	FileSystem_DOS::Delete_ (const PathName& pathName)
{
#if		qDOS || qWinNT || qWinOS
// This code is not right - try unlink and then rmdir!!!! Also, look at errno!!!
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	char*	tmp	=	nameBuf.ToCString ();
	int j = ::unlink (tmp);
	delete (tmp);
	if (j != 0) {
		sException.Raise (j);
	}
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
	AssertNotReached ();
#endif	/*qDOS || qWinNT || qWinOS*/
}

void	FileSystem_DOS::Rename_ (const PathName& from, const PathName& to)
{
	AssertNotReached ();
}

PathName	FileSystem_DOS::GetParentDirectory_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (PathName::kBad);
}

UInt32	FileSystem_DOS::CountContainedItems_ (const PathName& pathName) const
{
	// Does not appear to be any way to compute this directly, except counting
	// as we iterate (RESEARCH MORE - LGP Sept 1, 1992).
	UInt32	count	=	0;
	ForEach (PathComponent, it, MakeDirectoryIterator (pathName)) {
		count++;
	}
	return (count);
}

#if		qDOS || qWinNT || qWinOS
	// temp - hack - not sure how to do this right...
	class	DOSMountedVolumeIterator : public
#if		qRealTemplatesAvailable
		IteratorRep<PathComponent>
#else
		IteratorRep(PathComponent)
#endif
	{
		public:
			DOSMountedVolumeIterator ():
#if		qRealTemplatesAvailable
				IteratorRep<PathComponent> (),
#else
				IteratorRep(PathComponent) (),
#endif
				fCurrentIndex (0)
			{
			}

			override	Boolean	Done () const
			{
				return Boolean (fCurrentIndex > 1);
			}

			override	Boolean	More ()
			{
				fCurrentIndex++;
				return Boolean (fCurrentIndex == 1);
			}

			override	PathComponent	Current () const
			{
				Require (fCurrentIndex == 1);
				return (PathComponent ("c:"));
			}

			override	size_t	CurrentIndex () const
			{
				return (fCurrentIndex);
			}

#if		qRealTemplatesAvailable
			override	IteratorRep<PathComponent>*		Clone () const
#else
			override	IteratorRep(PathComponent)*		Clone () const
#endif
			{
				return (new DOSMountedVolumeIterator (*this));
			}

		private:
			size_t	fCurrentIndex;
	};
#endif	/*qDOS || qWinNT || qWinOS*/

#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_DOS::MakeMountedVolumeIterator_ () const
#else
Iterator(PathComponent)	FileSystem_DOS::MakeMountedVolumeIterator_ () const
#endif
{
#if		qDOS || qWinNT || qWinOS
	return (new DOSMountedVolumeIterator ());
#else
	sOperationNotSupported.Raise ();
	AssertNotReached (); return (Nil);
#endif
}

#if		qRealTemplatesAvailable
Iterator<PathComponent>	FileSystem_DOS::MakeDirectoryIterator_ (const PathName& directory) const
#else
Iterator(PathComponent)	FileSystem_DOS::MakeDirectoryIterator_ (const PathName& directory) const
#endif
{
	AssertNotImplemented (); return (Nil);
}

PathName	FileSystem_DOS::GetCurrentDirectory_ () const
{
	AssertNotImplemented (); return (PathName::kBad);
}

void	FileSystem_DOS::SetCurrentDirectory_ (const PathName& pathName)
{
	AssertNotImplemented ();
}

PathName	FileSystem_DOS::GetTemporaryDirectory_ () const
{
	AssertNotImplemented (); return (PathName::kBad);
}

PathName	FileSystem_DOS::GetPreferencesDirectory_ () const
{
// I'm not sure this is right for DOS???
#if		qDOS || qWinNT || qWinOS
	char*	home	=	::getenv ("HOME");
	if (home == Nil) {
		sException.Raise ();
	}
	else {
		return (PathComponent (home));
	}
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
#endif	/*qDOS || qWinNT || qWinOS*/
	AssertNotReached (); return (PathName::kBad);
}

AbstractFileSystem::FileDescriptor	FileSystem_DOS::Open_ (const PathName& pathName, int openModes)
{
#if		qDOS || qWinNT || qWinOS
	String	nameBuf;
	pathName.ApplySelfToPartialOSPathname (nameBuf, '\\');
	char*	tmp	=	nameBuf.ToCString ();
	::errno = 0;
	int	fd	=	::open (tmp, openModes);
	delete (tmp);
	if (fd < 0) {
		int theErrno = errno;
		::errno = 0;
		switch (theErrno) {
			case	0:			AssertNotReached ();
			case	EACCES:		sAccessPermissionDenied.Raise ();
			case	EAGAIN:		sException.Raise (theErrno);
			default:			sException.Raise (theErrno);
		}
	}
	else {
		return (fd);
	}
#else	/*qDOS || qWinNT || qWinOS*/
	sOperationNotSupported.Raise ();
#endif	/*qDOS || qWinNT || qWinOS*/
	AssertNotReached (); return (0);
}

Boolean		FileSystem_DOS::GetVisible_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (False);
}

void		FileSystem_DOS::SetVisible_ (const PathName& pathName, Boolean visible)
{
	AssertNotImplemented ();
}

Boolean	FileSystem_DOS::GetLocked_ (const PathName& pathName) const
{
	AssertNotImplemented (); return (False);
}

void	FileSystem_DOS::SetLocked_ (const PathName& pathName, Boolean locked)
{
	AssertNotImplemented ();
}












/*
 ********************************************************************************
 ************************************ FileSystem ********************************
 ********************************************************************************
 */
FileSystem*	FileSystem::sThe;

FileSystem::FileSystem ():
#if		qMacOS
	FileSystem_MacOS ()
#elif	qUnixOS
	FileSystem_UnixOS ()
#elif	qDOS || qWinNT || qWinOS
	FileSystem_DOS ()
#endif
{
}

AbstractFileSystem&	FileSystem::Get ()
{
	if (sThe == Nil) {
		sThe = new FileSystem ();
	}
	return (*sThe);
}

#if		qMacOS
AbstractFileSystem_MacOS&	FileSystem::Get_MacOS ()
#elif	qUnixOS
AbstractFileSystem_UnixOS&	FileSystem::Get_UnixOS ()
#elif	qDOS || qWinNT || qWinOS
AbstractFileSystem_DOS&	FileSystem::Get_DOS ()
#endif
{
	if (sThe == Nil) {
		sThe = new FileSystem ();
	}
	return (*sThe);
}







#if		!qRealTemplatesAvailable
	#include SetOfFileTypeX_cc
	#include Set_BitStringOfFileTypeX_cc
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

