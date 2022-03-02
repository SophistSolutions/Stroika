/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stroika_Foundation_Globals__
#define	__Stroika_Foundation_Globals__

/*
 * $Header: /fuji/lewis/RCS/Stroika-Foundation-Globals.hh,v 1.21 1992/12/05 17:33:45 lewis Exp $
 *
 * Description:
 *
 * Global variables whose order of construction is important.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Stroika-Foundation-Globals.hh,v $
 *		Revision 1.21  1992/12/05  17:33:45  lewis
 *		Add case to building of gDebugStream for qWinOS. Use cerr,
 *		for now. Later probably do more like what we do for mac.
 *
 *		Revision 1.20  1992/11/26  02:50:06  lewis
 *		Add forgoten #undef of qUseAllGlobals in bcc bug workaround.
 *		Almost totally rewrote initialization of file globals - got rid of
 *		hacks in file.cc - now all done here. Not too bad right now...
 *
 *		Revision 1.19  1992/11/25  03:21:42  lewis
 *		Add workaround for qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug.
 *
 *		Revision 1.18  1992/10/09  18:48:48  lewis
 *		Got rid of all string globals stuff since kEmptyString is
 *		gone, and we no-longer have to construct templates here.
 *
 *		Revision 1.17  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.16  1992/10/07  23:06:47  sterling
 *		Conditionanly add supporot for new GenClass instead of declare macros.
 *
 *		Revision 1.15  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.14  1992/09/15  17:03:39  lewis
 *		React to new container design, and change what we do declares of. This
 *		evoked a new gcc 2.2.1 bug which we temporarily call qDontNestEnumUsedInTemplate
 *		to work around it.
 *		Also, work around qGCC_StaticInitializerObjectSliceBug.
 *
 *		Revision 1.13  1992/09/11  16:23:21  sterling
 *		renamed ReferenceCounted to Shared
 *
 *		Revision 1.12  1992/09/11  13:09:31  lewis
 *		Fixed silly bug which caused us always to pull in all globals - I had said #ifdef qUseAllGlobals
 *		rather than #if!!!!!
 *		Also, added flag for qUseDebugGlobals. And did undefs for each subsidiary globals flag is
 *		qUseAllGlobals is explicitly set.
 *		Reformatted so indentation follows new standards.
 *
 *		Revision 1.11  1992/09/01  15:20:18  sterling
 *		Lots.
 *
 *		Revision 1.10  1992/07/21  06:36:37  lewis
 *		Use privately externed functions for File.cc to build PathName globals -
 *		instead of doing it in the .cc file where we have no control over
 *		construction order, or here were we dont have the right types
 *		declared (private to .cc file).
 *
 *		Revision 1.9  1992/07/16  07:41:09  lewis
 *		Build gDebugStream under qDOS.
 *		Cleanup comments - 80 columns.
 *
 *		Revision 1.8  1992/07/10  22:26:03  lewis
 *		Changed some file constants names etc.
 *
 *		Revision 1.7  1992/07/08  00:31:33  lewis
 *		Scoped time constants.
 *
 *		Revision 1.6  1992/07/02  23:42:33  lewis
 *		Fixed minor bug with macro define on containers of String/Real - added #define
 *		for ifndef.
 *
 *		Revision 1.5  1992/07/02  03:18:04  lewis
 *		Moved Macro based template declartions and implementations here from
 *		Strings.hh/cc.
 *		Made declarations conditional on a defined macro so they could be defined
 *		elsewhere if needed.
 *
 *		Revision 1.4  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.3  1992/07/01  03:37:32  lewis
 *		Get rid of workaround for gcc bug.
 *
 *		Revision 1.2  1992/06/20  19:26:23  lewis
 *		Tmp hack to get things working with gcc.
 *
 *		Revision 1.12  1992/05/09  01:25:58  lewis
 *		Make right for templates...
 *		Also, note bug sterl had with strings and Mac C Compiler -b2 flag - what
 *		abouwhat we relocate and keep strings String::eReadOnly - think about problem
 *		and resolve it!!!
 *
 *		Revision 1.9  1992/03/02  22:35:15  lewis
 *		Only define debugstream (and class) if debug turned on!
 *
 *		Revision 1.7  1992/01/05  06:21:20  lewis
 *		For macos debugstream, translate tabs to spaces.
 *
 *		Revision 1.6  1991/12/27  16:45:56  lewis
 *		fixed mac version of debugstream.
 *
 *
 */


#include	<iostream.h>
#include	"Debug.hh"


#ifndef	qUseAllGlobals
	#define	qUseAllGlobals	1
#endif

#if		qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug
	#undef	qUseAllGlobals
	#define	qUseAllGlobals	1
#endif

#if		qUseAllGlobals
	#undef	qUseDebugGlobals
	#define	qUseDebugGlobals	1
	#undef	qUseFileGlobals
	#define	qUseFileGlobals		1
#endif



#if		qUseDebugGlobals
	#if		qDebug
		#if		qMacOS
			class	DebugStreamBuf : public streambuf {
				public:
					DebugStreamBuf ():
						streambuf (fSmallBuf, 1),
						fCurIndex (0)
						{
						}
			
					override	int		DebugStreamBuf::overflow (int c)
						{
							if (c == '\t') c = ' ';		// tabs dont look right in macsbug
			
							fRealBuf [fCurIndex++] = c;
							if (fCurIndex >= sizeof (fRealBuf)-1) {
								DebugMessage ("overflowing in overflow, first part of message is");
								fRealBuf[fCurIndex-1] = '\0';
								DebugMessage ("%s", fRealBuf);
								fCurIndex = 0;
								return (c);
							}
							if ((c == EOF) or (c == '\r') or (c == '\n')) {
								fRealBuf[fCurIndex-1] = '\0';
								if (fCurIndex > 0) {
									DebugMessage ("%s", fRealBuf);
								}
								fCurIndex = 0;
							}
							return (c);
						}
				int		fCurIndex;
				char 	fSmallBuf[10];	// leave space in case
				char	fRealBuf[250];
			};
			
			class	DebugStream : public ostream {
				public:
					DebugStream ():
						fStreamBuf ()
					{
						init (&fStreamBuf);
					}
				private:
					DebugStreamBuf	fStreamBuf;
			};
			
			static	DebugStream	xxxFred;
			ostream&	gDebugStream	=	xxxFred;
		#elif	qUnixOS
			ostream&	gDebugStream	=	cerr;
		#elif	qDOS
			ostream&	gDebugStream	=	cerr;
		#elif	qWinOS
			// not really what we wan't to do, but not too much choice right now - eventually do
			// same sort of thing as MacOS...
			ostream&	gDebugStream	=	cerr;
		#endif	/*OS*/
	#endif	/*qDebug*/
#endif	/*qUseDebugGlobals*/




#if		qUseFileGlobals
	#include	"File.hh"

	#if		qRealTemplatesAvailable
		Set<FileSystem::FileType>	mkAnyFileTypeSet ()
		{
			Set_BitString<FileSystem::FileType>	s;
			s.Add (FileSystem::ePlainFileType);
			s.Add (FileSystem::eDirectoryFileType);
			s.Add (FileSystem::eSymbolicLink);
			s.Add (FileSystem::eSpecialFileType);
			return (s);
		}
		Set<FileSystem::FileType>	mkPlainFileTypeSet ()
		{
			Set_BitString<FileSystem::FileType>	s;
			s.Add (FileSystem::ePlainFileType);
			return (s);
		}
		Set<FileSystem::FileType>	mkDirectoryFileTypeSet ()
		{
			Set_BitString<FileSystem::FileType>	s;
			s.Add (FileSystem::eDirectoryFileType);
			return (s);
		}
		const	Set_BitString<FileSystem::FileType>	kAnyFileTypeSet (mkAnyFileTypeSet ());
		const	Set_BitString<FileSystem::FileType>	kPlainFileTypeSet (mkPlainFileTypeSet ());
		const	Set_BitString<FileSystem::FileType>	kDirectoryFileTypeSet (mkDirectoryFileTypeSet ());
	#else
		Set(FileTypeX)	mkAnyFileTypeSet ()
		{
			Set_BitString(FileTypeX)	s;
			s.Add (FileSystem::ePlainFileType);
			s.Add (FileSystem::eDirectoryFileType);
			s.Add (FileSystem::eSymbolicLink);
			s.Add (FileSystem::eSpecialFileType);
			return (s);
		}
		Set(FileTypeX)	mkPlainFileTypeSet ()
		{
			Set_BitString(FileTypeX)	s;
			s.Add (FileSystem::ePlainFileType);
			return (s);
		}
		Set(FileTypeX)	mkDirectoryFileTypeSet ()
		{
			Set_BitString(FileTypeX)	s;
			s.Add (FileSystem::eDirectoryFileType);
			return (s);
		}
		const	Set_BitString(FileTypeX)	kAnyFileTypeSet (mkAnyFileTypeSet ());
		const	Set_BitString(FileTypeX)	kPlainFileTypeSet (mkPlainFileTypeSet ());
		const	Set_BitString(FileTypeX)	kDirectoryFileTypeSet (mkDirectoryFileTypeSet ());
	#endif
	
	const	PathName		PathName::kBad;
#if		qGCC_StaticInitializerObjectSliceBug
	inline	PathComponent	Hack_PathName_Init1 ()
	{
		return (CurrentDirectoryPathComponent ());
	}
	inline	PathComponent	Hack_PathName_Init2 ()
	{
		return (ParentDirectoryPathComponent ());
	}
	const	PathComponent	PathName::kDot		=	Hack_PathName_Init1 ();
	const	PathComponent	PathName::kDotDot 	=	Hack_PathName_Init2 ();
#else
	const	PathComponent	PathName::kDot		=	CurrentDirectoryPathComponent ();
	const	PathComponent	PathName::kDotDot 	=	ParentDirectoryPathComponent ();
#endif
#endif







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Stroika_Foundation_Globals__*/

