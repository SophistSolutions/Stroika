/******************************************************************************
	CEditApp.c
	
	Application methods for a tiny editor.
		
	Copyright © 1989 Symantec Corporation. All rights reserved.

 ******************************************************************************/
#include	<string.h>

#include	"Commands.h"
#include	"CBartender.h"
#include 	"CEditApp.h"
#include	"CEditDoc.h"
#include	"Global.h"

#include	"CEditPane.h"

extern	OSType		gSignature;
extern	CBartender *gBartender;

#define		kExtraMasters		10
#define		kRainyDayFund		45000
#define		kCriticalBalance	40000
#define		kToolboxBalance		20000


#define	kApplicationSignature	'LDAP'
#define	kDocumentFileType		'TEXT'
#define	kStationeryFileType		'sEXT'

#define	kLedAboutBoxID	666


/***
 * IEditApp
 *
 *	Initialize the application. Your initialization method should
 *	at least call the inherited method. If your application class
 *	defines its own instance variables or global variables, this
 *	is a good place to initialize them.
 *
 ***/

void CEditApp::IEditApp(void)

{
	CApplication::IApplication( kExtraMasters, kRainyDayFund,
						kCriticalBalance, kToolboxBalance);
}



/***
 * SetUpFileParameters
 *
 *	In this routine, you specify the kinds of files your
 *	application opens.
 *
 *
 ***/

void CEditApp::SetUpFileParameters(void)

{
	CApplication::SetUpFileParameters();	/* Be sure to call the default method */

		/**
		 **	sfNumTypes is the number of file types
		 **	your application knows about.
		 **	sfFileTypes[] is an array of file types.
		 **	You can define up to 4 file types in
		 **	sfFileTypes[].
		 **
		 **/

	sfNumTypes = 2;
	sfFileTypes[0] = kDocumentFileType;
	sfFileTypes[1] = 'sEXT';

		/**
		 **	Although it's not an instance variable,
		 **	this method is a good place to set the
		 **	gSignature global variable. Set this global
		 **	to your application's signature. You'll use it
		 **	to create a file (see CFile::CreateNew()).
		 **
		 **/

	gSignature = kApplicationSignature;
}


/***
 * SetUpMenus
 *
 *	In this method, you add special menu items and set the
 *	menu item dimming and checking options for your menus.
 *	The most common special menu items are the names of the
 *	fonts. For this tiny editor, you also want to set up the
 *	dimming and checking options so only the current font
 *	and size are checked.
 *
 ***/

void CEditApp::SetUpMenus(void)

{
		/**
		 ** Let the default method read the menus from
		 **	the MBAR 1 resource.
		 **
		 **/

	CApplication::SetUpMenus();

		/**
		 ** Add the fonts in the  system to the
		 **	Font menu. Remember, MENUfont is one
		 **	of the reserved font numbers.
		 **
		 **/

	AddResMenu(GetMHandle(MENUfont), 'FONT');

		/**
		 **	The UpdateMenus() method sets up the dimming
		 **	for menu items. By default, the bartender dims
		 **	all the menus, and each bureaucrat is reponsible
		 **	for turning on the items that correspond to the commands
		 **	it can handle.
		 **
		 **	Set up the options here. The edit pane's UpdateMenus()
		 **	method takes care of doing the work.
		 **
		 **	For Font and Size menus, you want all the items to
		 **	be enabled all the time. In other words, you don't
		 **	want the bartender to ever dim any of the items
		 **	in these two menus.
		 **
		 **/

	gBartender->SetDimOption(4, dimNONE);
	gBartender->SetDimOption(MENUfont, dimNONE);
	gBartender->SetDimOption(MENUsize, dimNONE);
	gBartender->SetDimOption(MENUstyle, dimNONE);

		/**
		 **	For Font and Size menus, one of the items
		 **	is always checked. Setting the unchecking option
		 **	to TRUE lets the bartender know that it should
		 **	uncheck all the menu items because an UpdateMenus()
		 **	method will check the right items.
		 **	For the Style menu, uncheck all the items and
		 **	let the edit pane's UpdateMenus() method check the
		 **	appropriate ones.
		 **
		 **/

	gBartender->SetUnchecking(MENUfont, TRUE);
	gBartender->SetUnchecking(MENUsize, TRUE);
	gBartender->SetUnchecking(MENUstyle, TRUE);
}



/***
 * CreateDocument
 *
 *	The user chose New from the File menu.
 *	In this method, you need to create a document and send it
 *	a NewFile() message.
 *
 ***/

void CEditApp::CreateDocument()

{
	CEditDoc	*theDocument = NULL;
	
	// In the event that creating the document fails,
	// we setup an exception handler here. If any
	// of the methods called within the scope of this
	// TRY block fail, an exception will be raised and
	// control will be transferred to the CATCH block.
	// Here, the CATCH block takes care of disposing
	// of the partially created document.
	
	TRY
	{
		theDocument = new(CEditDoc);
			
			/**
			 **	Send your document an initialization
			 **	message. The first argument is the
			 **	supervisor (the application). The second
			 **	argument is TRUE if the document is printable.
			 **
			 **/
		
		theDocument->IEditDoc(this, TRUE);
	
			/**
			 **	Send the document a NewFile() message.
			 **	The document will open a window, and
			 **	set up the heart of the application.
			 **
			 **/
		theDocument->NewFile();
	}
	CATCH
	{
		ForgetObject( theDocument);
	}
	ENDTRY;
}

/***
 * OpenDocument
 *
 *	The user chose OpenÉ from the File menu.
 *	In this method you need to create a document
 *	and send it an OpenFile() message.
 *
 *	The macSFReply is a good SFReply record that contains
 *	the name and vRefNum of the file the user chose to
 *	open.
 *
 ***/
 
void CEditApp::OpenDocument(SFReply *macSFReply)

{
	CEditDoc	*theDocument = NULL;

	// In the event that opening the document fails,
	// we setup an exception handler here. If any
	// of the methods called within the scope of this
	// TRY block fail, an exception will be raised and
	// control will be transferred to the CATCH block.
	// Here, the CATCH block takes care of disposing
	// of the partially opened document.

	TRY
	{	
		theDocument = new(CEditDoc);
			
			/**
			 **	Send your document an initialization
			 **	message. The first argument is the
			 **	supervisor (the application). The second
			 **	argument is TRUE if the document is printable.
			 **
			 **/
		
		theDocument->IEditDoc(this, TRUE);
	
			/**
			 **	Send the document an OpenFile() message.
			 **	The document will open a window, open
			 **	the file specified in the macSFReply record,
			 **	and display it in its window.
			 **
			 **/
		theDocument->OpenFile(macSFReply);
	}
	CATCH
	{
		ForgetObject( theDocument);
	}
	ENDTRY;
}

void	CEditApp::UpdateMenus()
{
	CApplication::UpdateMenus();
gBartender->EnableCmd(cmdAbout);
}

void	CEditApp::DoCommand (long theCommand)
{
	switch (theCommand) {
		case	cmdAbout: {
			#if		qUseBuiltinTE
				Str255	buf1	=	"\pBuiltinTE";
			#else
				Str255	buf1;
				buf1[0] = strlen (qLed_ShortVersionString);
				memcpy (&buf1[1], qLed_ShortVersionString, buf1[0]);
			#endif

			Str255	buf2;
			buf2[0] = strlen (__DATE__);
			memcpy (&buf2[1], __DATE__, buf2[0]);
			ParamText (buf1, buf2, "\p", "\p");
			Alert (kLedAboutBoxID, NULL);
		}
		break;

		default: {
			CApplication::DoCommand(theCommand);
		}
		break;
	}
}


// replace TCL's operator new...

#if		1
#define	qUseMacTmpMemForAllocs		1


// LGP for NEW MWERKS compiler - LGP 950420
	// BASED ON MWERKS OP_NEW LIBRARY - CW7 - LGP 951112
	#include <Memory.h>

	#ifdef __MC68K__
	#pragma a6frames on
	#endif

Boolean __MMPrimitiveAllocate = 0;	// quirk of TCL 2.0???

struct MemPool {
	struct MemPool		*next;				//	pointer to next pool
	size_t				size;				//	number of bytes in pool (including header)
	char				data[];				//	variable size user data section
};

static MemPool	*mempools;						//	list of memory pools
static char		*lastfree;						//	pointer to last free block
static char		*lastend;						//	pointer to last end
//static size_t	_newpoolsize	= 0x00010000L;	//	number of bytes allocated for a new pool
//static size_t	_newnonptrmax	= 0x00001000L;	//	any object bigger than this will call NewPtr(...) directly 
const	size_t	_newpoolsize	= 64*1024;		//	number of bytes allocated for a new pool
const	size_t	_newnonptrmax	= _newpoolsize;	//	any object bigger than this will call NewPtr(...) directly 

extern void (*new_handler)();


	inline	char*	DoSysAlloc (size_t n)
		{
#if		qUseMacTmpMemForAllocs
			OSErr err;	// ingored...
			Handle	h	=	::TempNewHandle (n, &err);	// try temp mem, and use our local mem if no temp mem left
			if (h == NULL) {
				h = ::NewHandle (n);
			}
			if (h == NULL) {
				return NULL;
			}
			HLock (h);
			return *h;
#else
			return NewPtr(n);
#endif
		}
	inline	void	DoSysFree (void* p)
		{
#if		qUseMacTmpMemForAllocs
			Handle	h	=	::RecoverHandle (Ptr (p));
			::HUnlock (h);
			::DisposeHandle (h);
#else
			::DisposePtr (Ptr (p));
#endif
		}


void*	operator new(size_t size)
{
	MemPool	*pool;
	char	*ptr,*end;
	long	bsize,nsize;

	if(size>0x7FFFFFF0) return 0;
	size = 4L + ((size + 3L) & 0xFFFFFFFC);	//	alloc *4 quantity plus 4 extra bytes for size

	while(1)
	{
		if(size>=_newnonptrmax && (ptr=DoSysAlloc (size))!=NULL)
		{
			*(long *)ptr=0L; return ptr+4;
		}

		if((ptr=lastfree)!=0L && (bsize=*(long *)ptr)>=(long)size)
		{	//	last free block has enough memory left
			end=lastend; goto alloc2;
		}

		for(pool=mempools; pool; pool=pool->next)
		{
alloc:		for(ptr=pool->data,end=(Ptr)pool+pool->size; ptr<end;) if((bsize=*(long *)ptr)>0)
			{
alloc2:			lastfree=0L;
				while(ptr+bsize<end && (nsize=*(long *)(ptr+bsize))>0)
				{	//	merge block with the next block
					*(long *)ptr=bsize=bsize+nsize;
				}
				if(bsize>=size)
				{	//	pool block is big enough
					if(bsize>=size+8)
					{	//	split this block
						lastfree=ptr; lastend=end;
						bsize-=size; *(long *)ptr=bsize; ptr+=bsize;
						*(long *)ptr=-size; return ptr+4;
					}
					else
					{	//	allocate whole block
						*(long *)ptr=-bsize; return ptr+4;
					}
				}
				else ptr+=bsize;
			}
			else
			{
				if(bsize==0) break;		//	corrupt heap?
				ptr-=bsize;
			}
next:;	}

		//	not enough free memory in mempools (try to allocate a new Ptr from OS)
		if((pool=(MemPool *)DoSysAlloc (_newpoolsize))!=NULL)
		{
			pool->next=mempools; mempools=pool;
			pool->size=_newpoolsize; *(long *)pool->data=pool->size-sizeof(MemPool);
			goto alloc;
		}
		else
		{	//	try to allocate a system block
			if((ptr=(char *)DoSysAlloc (size))!=NULL)
			{
				*(long *)ptr=0L; return ptr+4;
			}
		}

		if(new_handler) new_handler(); else return NULL;
	}
}

void operator delete (void* ptr)
{
	if(ptr != NULL) {
		ptr=(char *)ptr-4;
		if((*(long *)ptr=-*(long *)ptr)==0L) {
			DoSysFree ((Ptr)ptr);
		}
	}
}
#endif

