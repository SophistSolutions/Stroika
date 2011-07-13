/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Memory.cc,v 1.12 1992/12/08 21:30:36 lewis Exp $
 *
 * TODO:
 *			-	On non-mac systems, we must call outofmemory proc if malloc fails...
 *
 * Changes:
 *	$Log: Memory.cc,v $
 *		Revision 1.12  1992/12/08  21:30:36  lewis
 *		Add case to #ifs for includes for qWinNT.
 *
 *		Revision 1.11  1992/12/05  17:38:11  lewis
 *		Add || qWinOS to qDOS case.
 *
 *		Revision 1.10  1992/11/15  16:11:45  lewis
 *		Add size_t PreferredAllocSize() - at kdjs suggestion. Just quick
 *		hack implemenation for now.
 *
 *		Revision 1.9  1992/11/10  04:44:05  lewis
 *		Work around qGCC_DeleteOfVoidStarProducesWarningBug.
 *
 *		Revision 1.8  1992/10/18  07:03:25  lewis
 *		Check for nil and return in no-debug case of operator delete.
 *
 *		Revision 1.6  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/21  05:20:27  lewis
 *		Replace usage of qBorlandCPlus_ShortCharConstantsBug  with
 *		qLongCharacterConstantsSupported and fix values of kHeaderMagicNumber
 *		and kTrailerMagicNumber to be the numeric value of 'STRT' and 'END*'
 *		in case of the workaround.
 *
 *		Revision 1.3  1992/07/03  06:20:26  lewis
 *		Use qGCC_TroubleWithFunctionsReturningFunctionPtrs instead of qGCC for
 *		function pointer bug.
 *
 *		Revision 1.2  1992/06/20  19:31:21  lewis
 *		Hack around old gcc parse trouble with function pointers.
 *
 *		Revision 1.22  1992/06/10  02:18:53  lewis
 *		Got rid of old qDOS ptr conversion code - unused, and probably wrong for anything but realmode.
 *
 *		Revision 1.21  92/05/22  23:47:28  23:47:28  lewis (Lewis Pringle)
 *		Moved BlockAllocated/Shared stuff to sepatate files.
 *		
 *		Revision 1.20  92/05/13  18:49:30  18:49:30  lewis (Lewis Pringle)
 *		Hack Shared DTOR temporarily to get working again - talk to sterl about best solution.
 *		
 *		Revision 1.19  92/05/13  00:05:32  00:05:32  lewis (Lewis Pringle)
 *		Made DTOR for Shared NON-VIRTUAL, and only define it with debug ON.
 *		
 *		Revision 1.18  92/04/15  13:37:06  13:37:06  sterling (Sterling Wight)
 *		turned off heap validation by default
 *		
 *		Revision 1.17  92/04/09  14:53:47  14:53:47  lewis (Lewis Pringle)
 *		Turn sValidateOnALloc on by default.
 *		
 *		Revision 1.15  92/03/30  14:34:41  14:34:41  lewis (Lewis Pringle)
 *		Ported to BorlandC++
 *		
 *		Revision 1.14  92/03/28  03:25:56  03:25:56  lewis (Lewis Pringle)
 *		Get rid of addressTranslation stuff for dos - not needed???
 *		
 *		Revision 1.12  1992/03/22  09:16:21  lewis
 *		On memory overwrites in op delete, print friendlier message before doing assertion failure.
 *
 *		Revision 1.11  1992/02/21  17:38:57  lewis
 *		No longer support qGPlus_FunctParamBug. Also, noted bug on non-mac systems
 *		with growzoneproc in todo.
 *
 *		Revision 1.10  1992/02/14  02:29:22  lewis
 *		Got rid of qDebug_SupportValidateOnAlloc define. Always allow if debug turned on.
 *
 *		Revision 1.9  1992/01/28  07:10:51  lewis
 *		Changed magic cookies to something that shows up better in a debugger.
 *		Also, implemented tail patched magic - was very helpful in tracing down bug with XPutPixel...
 *
 *		Revision 1.8  1992/01/22  07:32:57  lewis
 *		Print memory alloc bias under UNIX in PrintMemoryStatistics.
 *
 *		Revision 1.7  1992/01/09  06:35:39  lewis
 *		Cleaned up, plus fixed bug in Realloc where it would sometimes
 *		read past the end of its source buffer in its call to memcpy - fine on machines
 *		with no memory protection (ususally) but not so under UNIX, and sort of a wawste anyhow. Now just
 *		copy the right amount.
 *
 *
 */



#include	<stdlib.h>
#include	<string.h>

#include	"OSRenamePre.hh"
	#if		qMacOS
		#include	<Memory.h>		//	Needed for things like mac growzone stuff, etc...
		#include	<SysEqu.h>
	#elif       qDOS || qWinNT || qWinOS
		#include	<mem.h>			//	See what this is really needed for???
	#endif	/*OS*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"OSConfiguration.hh"
#include	"Math.hh"

#include	"Memory.hh"


#if		qDebug
static	Boolean	gMemoryAllocsEnabled	=	True;
#endif	/*qDebug*/

MemoryException	gMemoryException;



#if		qDebug
#if		qLongCharacterConstantsSupported
const	UInt32	kHeaderMagicNumber	=	'STRT';		// useful value when looking in debugger
const	UInt32	kTrailerMagicNumber	=	'END*';		// useful value when looking in debugger
#else
const	UInt32	kHeaderMagicNumber	=	0x53545254;
const	UInt32	kTrailerMagicNumber	=	0x454E442A;
#endif
struct	AllocationBlock {	/* template for debug mode allocations */
	UInt32				fMagic;
	size_t				fSize;
	AllocationBlock*	fNext;
	AllocationBlock*	fPrev;
	char				fMemory[1];					// obviously critical that this field come last...

	static	AllocationBlock*		sFirstBlock;
};
AllocationBlock*		AllocationBlock::sFirstBlock	=	Nil;


static	UInt32	gMallocCount		=	0;
static	UInt32	gTotalMalloced		=	0;
static	UInt32	gFreeCount			=	0;
static	UInt32	gTotalFreed			=	0;
static	UInt32	gReAllocCount		=	0;
static	Int32	gReAllocBias		=	0;


/*
 * Try turning this on by default. Maybe set it conditional on hardware flags. Nice to have on when
 * debugging by default, and overhead isn't too bad. Perhaps selectively turn off for hardware configs
 * that are very slow (but what would anyone do developemnt on such a machine??).
 */
static	Boolean	sValidateOnAlloc	=	False;
#endif	/*qDebug*/






#if		qMacOS
/*
 ********************************************************************************
 ********************************* macGrowZoneProc ******************************
 ********************************************************************************
 */
// See IM Vol 2, Page 42, 43
static	pascal	long	macGrowZoneProc (osSize cbNeeded)
{
	if (GetOutOfMemoryProc () != Nil) {
		Boolean	result	=	(*GetOutOfMemoryProc ()) (size_t (cbNeeded));
#if		qDebug
		DebugMessage ("macGrowZoneProc (cbNeeded = 0x%x), and our outofmemoryproc returned %s.", cbNeeded, result? "True": "False");
#endif	/*qDebug*/
		return (result? 1: 0);
	}
	else {
#if		qDebug
		DebugMessage ("macGrowZoneProc (cbNeeded = 0x%x) called- not handled.", cbNeeded);
#endif	/*qDebug*/
	}
	return (0);
}
#endif	/*qMacOS*/






/*
 ********************************************************************************
 ****************************** G/SetOutOfMemoryProc ****************************
 ********************************************************************************
 */
static	Boolean	(*sOutOfMemoryProc) (size_t amountNeeded)	=	Nil;


#if		qGCC_TroubleWithFunctionsReturningFunctionPtrs
extern
#endif
Boolean (*GetOutOfMemoryProc ()) (size_t)	
{
	return (sOutOfMemoryProc);
}

void	SetOutOfMemoryProc (Boolean (*outOfMemoryProc) (size_t amountNeeded))
{
	sOutOfMemoryProc = outOfMemoryProc;
#if		qMacOS
	::SetGrowZone (&macGrowZoneProc);
#endif	/*qMacOS*/
}




#if		qDebug
/*
 ********************************************************************************
 **************************** SetMemoryAllocsEnabled ****************************
 ********************************************************************************
 */
Boolean	SetMemoryAllocsEnabled (Boolean memoryAllocsEnabled)
{
	Boolean	oldMemoryAllocsEnabled	=	gMemoryAllocsEnabled;
	gMemoryAllocsEnabled = memoryAllocsEnabled;
	return (oldMemoryAllocsEnabled);
}
#endif	/*qDebug*/






/*
 ********************************************************************************
 *********************************** MemoryException ****************************
 ********************************************************************************
 */

MemoryException::MemoryException ():
	Exception (),
	fErr (0)
{
}

MemoryException::MemoryException (const Exception& exception):
	Exception (exception),
	fErr (0)
{
}

MemoryException::MemoryException (const MemoryException& memoryException):
	Exception (memoryException),
	fErr (0)
{
}

Int16	MemoryException::GetOSErr () const
{
	return (fErr);
}

void	MemoryException::Raise ()
{
	if (Exception::GetCurrent () != this) {	// if were not the current exception return something
											// indicating unknown error, otherwise preserve our
											// error #
#if		qMacOS
		fErr = noErr;
#endif	/*qMacOS*/
	}
	Exception::Raise ();
}

void	MemoryException::Raise (Int16 osErr)
{
	fErr = osErr;
	Exception::Raise ();
}



#if		qDebug

#define	MallocedSize(_desired_size_)	(Align (offsetof (AllocationBlock, fMemory)) + Align (_desired_size_) + Align (sizeof (UInt32)))

#endif	/*qDebug*/









/*
 ********************************************************************************
 *********************************** operator new () ****************************
 ********************************************************************************
 */
void*	operator	new (size_t n)
{
#if		qDebug
	Require (gMemoryAllocsEnabled);

	if (sValidateOnAlloc)	ValidateHeapStorage ();

	AllocationBlock*	p;
	if ((p = (AllocationBlock*)malloc (MallocedSize (n))) == Nil) {
		gMemoryException.Raise ();
	}
	if (p->fMagic == kHeaderMagicNumber) {
		// cannot use assertion cuz does really happen sometimes...
		DebugMessage ("on new (p->fMagic == kHeaderMagicNumber) - strange - but OK!");
	}
	p->fMagic = kHeaderMagicNumber;
	*((UInt32*) (((char*)(&p->fMemory[0])) + Align (n))) = kTrailerMagicNumber;
	p->fSize = n;
	p->fPrev = Nil;
	p->fNext = AllocationBlock::sFirstBlock;
	if (AllocationBlock::sFirstBlock != Nil) {
		AllocationBlock::sFirstBlock->fPrev = p;
	}
	AllocationBlock::sFirstBlock = p;

	EnsureNotNil (AllocationBlock::sFirstBlock);
	Ensure (AllocationBlock::sFirstBlock->fPrev == Nil);
	gMallocCount++;
	gTotalMalloced += n;
	EnsureNotNil (p);
	return (&p->fMemory[0]);
#else	/*qDebug*/
	void*	p;
#if		qMPW_CFront_AppleC || __TURBOC__
	if (n == 0)	n = 1;		// turbo & MPW bug - or am I confused...
#endif	/*qMPW_CFront_AppleC || __TURBOC__*/

	if ((p = malloc (n)) == Nil) {
		gMemoryException.Raise ();
	}
	return (p);
#endif	/*qDebug*/
}





/*
 ********************************************************************************
 *********************************** operator delete () *************************
 ********************************************************************************
 */
void	operator	delete (void* p)
{
#if		qDebug
	Require (gMemoryAllocsEnabled);

	if (sValidateOnAlloc)	ValidateHeapStorage ();

	if (p == Nil) {
		return;
	}

	/*
	 * Cast to char*, so done as pointer arithmatic - subtle for segmented architecures.
	 * Discuss further.
	 */
	AllocationBlock*	allocBlk	=	(AllocationBlock*)(((char*)p) - offsetof (AllocationBlock, fMemory));

	if (allocBlk->fMagic == ~kHeaderMagicNumber) {
		DebugMessage ("serious mistake: double free of %p.", p);
	}
	if (allocBlk->fMagic != kHeaderMagicNumber) {
		DebugMessage ("serious mistake: header for pointer %p clobbered.", p);
	}
	if (*((UInt32*) (((char*)(&allocBlk->fMemory[0])) + Align (allocBlk->fSize))) != kTrailerMagicNumber) {
		DebugMessage ("serious mistake: header for pointer %p clobbered.", p);
	}
	Assert (allocBlk->fMagic == kHeaderMagicNumber);
	Assert (*((UInt32*) (((char*)(&allocBlk->fMemory[0])) + Align (allocBlk->fSize))) == kTrailerMagicNumber);

	allocBlk->fMagic = ~kHeaderMagicNumber;
	*((UInt32*) (((char*)(&allocBlk->fMemory[0])) + Align (allocBlk->fSize))) = ~kTrailerMagicNumber;
	gFreeCount++;
	gTotalFreed += allocBlk->fSize;


// new experimental feature - should be controlled by #if???
// reset all of memory to bad value, so we flush bugs out sooner (bugs of the sort where
// we reference something after its been deleted. Sterl did this mistake when he changed
// over the reference counted stuff, and I think this might be useful to catch other such
// mistakes (LGP 11/5/91)
	memset (p, 0xFF, allocBlk->fSize);		// at least on 68K this is probably bad value
											// not too sure what best (aka worst) value to
											// put here...


	AssertNotNil (AllocationBlock::sFirstBlock);
	Assert (AllocationBlock::sFirstBlock->fPrev == Nil);

	if (AllocationBlock::sFirstBlock == allocBlk) {
		Assert (allocBlk->fPrev == Nil);
		AllocationBlock::sFirstBlock = allocBlk->fNext;
		if (AllocationBlock::sFirstBlock != Nil) {
			AllocationBlock::sFirstBlock->fPrev = Nil;
		}
	}
	else {
		AssertNotNil (allocBlk->fPrev);
		Assert (allocBlk->fPrev->fNext == allocBlk);		// was me
		allocBlk->fPrev->fNext = allocBlk->fNext;			// by prev now points to my next
		if (allocBlk->fNext != Nil) {
			Assert (allocBlk->fNext->fPrev == allocBlk);	// was me
			allocBlk->fNext->fPrev = allocBlk->fPrev;		// by next now points to my prev
		}
	}
#if		qSun_CFront_FreeBadArg
	free ((char*)allocBlk);
#else
	free (allocBlk);
#endif

#else	/*qDebug*/

	if (p == Nil) {
		return;
	}
#if		qSun_CFront_FreeBadArg
	free ((char*)p);
#else
	free (p);
#endif

#endif	/*qDebug*/
}




/*
 ********************************************************************************
 *************************************** ReAlloc ********************************
 ********************************************************************************
 */


// NB: This version of realloc promises that if the realloc fails, the old memory remains
// in-tact... We just throw an execption, and change nothing...
// Should check each lib version we use promises this!!!!
void*	ReAlloc (void* p, size_t n)
{
	RequireNotNil (p);
	Require (gMemoryAllocsEnabled);
#if		qDebug
	// Simple, and we don't care too much about performance until we turn off debugging. We
	// could be more elaborate here someday....
	AllocationBlock*	allocBlk	=	(AllocationBlock*)(((char*)p) - offsetof (AllocationBlock, fMemory));
	if (allocBlk->fMagic == ~kHeaderMagicNumber) {
		DebugMessage ("serious mistake: realloc of free of %p.", p);
	}
	Assert (allocBlk->fMagic == kHeaderMagicNumber);
	Assert (*((UInt32*) (((char*)(&allocBlk->fMemory[0])) + Align (allocBlk->fSize))) == kTrailerMagicNumber);
	size_t oldLength = allocBlk->fSize;

	size_t copySize = Min (oldLength, n);

	void*	x = new char [n];
	memcpy (x, p, copySize);

#if		qGCC_DeleteOfVoidStarProducesWarningBug
	delete (char*)(p);
#else
	delete (p);
#endif

	gReAllocCount++;
	gReAllocBias = gReAllocBias + n - oldLength;
	
	// must munge alloc info cuz called by new, doesn't reflect realloc
	gMallocCount--;
	gTotalMalloced -= n;

	// must munge free info cuz called by new, doesn't reflect realloc
	gFreeCount--;
	gTotalFreed -= oldLength;
	
	return (x);
#else	/*qDebug*/

#if		qMPW_CFront_AppleC || __TURBOC__
	if (n == 0)	n = 1;		// turbo & MPW bug - or am I confused...
#endif	/*qMPW_CFront_AppleC || __TURBOC__*/

	if ((p = realloc (p, n)) == Nil) {
		gMemoryException.Raise ();
	}
	return (p);
#endif	/*qDebug*/
}





/*
 ********************************************************************************
 ********************************** PrintMemoryStatistics ***********************
 ********************************************************************************
 */
#if		qDebug
void	PrintMemoryStatistics ()
{
	Require (gMemoryAllocsEnabled);

#if		qMacOS
	DebugMessage ("new: %ld/%ld, delete: %ld/%ld, realloc: %ld/%ld, using: = %ld/%ld, free = %ld",
					gMallocCount, gTotalMalloced, gFreeCount, gTotalFreed, gReAllocCount, gReAllocBias,
					gMallocCount - gFreeCount, gTotalMalloced+gReAllocBias-gTotalFreed, ::FreeMem ());
#else
	DebugMessage ("new: %ld/%ld, delete: %ld/%ld, realloc: %ld/%ld, using: = %ld/%ld",
					gMallocCount, gTotalMalloced, gFreeCount, gTotalFreed, gReAllocCount, gReAllocBias,
					gMallocCount - gFreeCount, gTotalMalloced+gReAllocBias-gTotalFreed);
#endif
}




/*
 ********************************************************************************
 ********************************** GetValidateOnAlloc **************************
 ********************************************************************************
 */
Boolean GetValidateOnAlloc ()
{
	return (sValidateOnAlloc);
}


/*
 ********************************************************************************
 ********************************** SetValidateOnAlloc **************************
 ********************************************************************************
 */
void    SetValidateOnAlloc (Boolean validateOnAlloc)
{
	sValidateOnAlloc = validateOnAlloc;
}
 
 

/*
 ********************************************************************************
 ********************************** ValidateHeapStorage *************************
 ********************************************************************************
 */
Boolean	ValidateHeapStorage (Boolean reportIfGood)
{
	Require (gMemoryAllocsEnabled);
	Boolean	allOK				=	True;
	size_t	smallest			=	0xffffffff;
	size_t	biggest				=	0;
	UInt32	itemCount			=	0;
	UInt32	itemsUnder4Bytes	=	0;
	UInt32	itemsUnder10Bytes	=	0;
	size_t	sizeCount			=	0;
	for (register AllocationBlock* cur = AllocationBlock::sFirstBlock; cur != Nil; cur = cur->fNext) {
		if (cur->fMagic != kHeaderMagicNumber) {
			DebugMessage ("item at (AllocationBlock*)%p corruct - bad cookie", cur);
			DebugMessage ("\theader is %lx, tail is %lx, size= %x, and usable memory starts at %p",
						  cur->fMagic, *((UInt32*) (((char*)(&cur->fMemory[0])) + Align (cur->fSize))), cur->fSize, &cur->fMemory[0]);
			allOK = False;
		}
		if (*((UInt32*) (((char*)(&cur->fMemory[0])) + Align (cur->fSize))) != kTrailerMagicNumber) {
			DebugMessage ("item at (AllocationBlock*)%p corruct - bad trailer cookie", cur);
			DebugMessage ("\theader is %lx, tail is %lx, size= %x, and usable memory starts at %p",
						  cur->fMagic, *((UInt32*) (((char*)(&cur->fMemory[0])) + Align (cur->fSize))), cur->fSize, &cur->fMemory[0]);
			allOK = False;
		}
		itemCount++;
		sizeCount += cur->fSize;
		if (cur->fSize < smallest) {
			smallest = cur->fSize;
		}
		if (cur->fSize > biggest) {
			biggest = cur->fSize;
		}
		if (cur->fSize <= 4) {
			itemsUnder4Bytes ++;
		}
		if (cur->fSize <= 10) {
			itemsUnder10Bytes ++;
		}
	}

	if (itemCount != (gMallocCount-gFreeCount)) {
		DebugMessage ("item count = %d, and (gMallocCount(%d)-gFreeCount(%d)) = %d",
			itemCount, gMallocCount, gFreeCount, gMallocCount-gFreeCount);
		allOK = False;
	}
	if (sizeCount != (gTotalMalloced+gReAllocBias-gTotalFreed)) {
		DebugMessage ("sizeCount = %d, and gTotalMalloced+gReAllocBias-gTotalFreed = %d",
			sizeCount, gTotalMalloced+gReAllocBias-gTotalFreed);
		allOK = False;
	}

	if (allOK) {
		if (reportIfGood) {
			DebugMessage ("all OK: itemCount = %d, smallest = %d, biggest = %d, ave = %d,\nitems under 4/10 bytes = %d/%d",
				itemCount, smallest, biggest, itemCount==0? 0: (gTotalMalloced+gReAllocBias-gTotalFreed)/itemCount, itemsUnder4Bytes, itemsUnder10Bytes);
		}
	}
	else {
		DebugMessage ("serious trouble!");
	}
	return (allOK);
}


#endif	/*qDebug*/




/*
 ********************************************************************************
 ********************************** PreferredAllocSize **************************
 ********************************************************************************
 */
size_t	PreferredAllocSize (size_t originalSize)
{
	/*
	 * Do a little research, and see if we can get real figures here.
	 */
#if		qMPW_CFront
	return (RoundUpTo (originalSize+8, 32) - 8);	// just a guess
#elif	qGCC
	return (RoundUpTo (originalSize+8, 64) - 8);	// just a guess
#else
	return (RoundUpTo (originalSize+8, 32) - 8);	// No guess!!!
#endif
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

