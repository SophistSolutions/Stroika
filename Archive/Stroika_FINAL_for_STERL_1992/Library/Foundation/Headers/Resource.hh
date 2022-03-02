/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Resource__
#define	__Resource__

/*
 * $Header: /fuji/lewis/RCS/Resource.hh,v 1.10 1992/10/10 03:56:51 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Resource.hh,v $
 *		Revision 1.10  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.8  1992/09/15  16:54:58  lewis
 *		Got rid of AbSequence.
 *
 *		Revision 1.7  1992/09/11  16:11:36  sterling
 *		use new Shared implementation
 *
 *		Revision 1.6  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.4  1992/07/21  06:31:20  lewis
 *		Make returned interators all const, and use Sequence
 *		instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  02:59:54  lewis
 *		Renamed Sequence_DoublyLinkedList/Sequence_DoublyLLOfPointers to
 *		Sequence/SequencePtr.
 *
 *		Revision 1.2  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.6  1992/06/10  02:18:20  lewis
 *		Moved declare of String Resource back here - no reason Foundation resources shouldn't be
 *		declared here.
 *
 *		Revision 1.5  92/06/09  23:40:47  23:40:47  lewis (Lewis Pringle)
 *		Moved graphix related stuff to Graphix layer.
 *
 *		Revision 1.4  92/06/09  12:35:02  12:35:02  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.3  92/06/09  15:00:12  15:00:12  sterling (Sterling Wight)
 *		many changes, including using templates for reference counting
 *
 *		Revision 1.2  92/05/19  10:09:26  10:09:26  sterling (Sterling Wight)
 *		used void* instead of stream
 *	
 *		Revision 1.1  92/05/13  12:28:41  12:28:41  lewis (Lewis Pringle)
 *		Initial revision
 *
 */

#include	"File.hh"
#include	"Saveable.hh"
#include	"Sequence.hh"
#include	"String.hh"


// SSW: 9/8/92: need to patch up but since don't use who cares???
#if 0

class	AbResourceBuffer;

#if		!qRealTemplatesAvailable
Declare (Shared, AbResourceBuffer);
#endif

class	Resource {
	public:
		Resource ();
		Resource (const Resource& from);
		Resource (AbResourceBuffer* sharedPart);		

		~Resource ();

		nonvirtual	Resource&	operator= (const Resource& newResource);
		
		nonvirtual	String			GetType () const;
		nonvirtual	void			SetType (const String& type);
		
		nonvirtual	String			GetKey () const;		
		nonvirtual	void			SetKey (const String& key);

		nonvirtual	void*			GetResource () const;
		nonvirtual	size_t	GetLength () const;
		nonvirtual	void			SetResource (void* resource, size_t nBytes);

	private:
#if		qRealTemplatesAvailable
		Shared<AbResourceBuffer>		fRep;
#else
		Shared(AbResourceBuffer)		fRep;
#endif
		
		static	AbResourceBuffer*	Clone (const AbResourceBuffer&);

	/*
	 * These are made friends so they can peek at the shared part, as an optimization
	 */
	friend	Boolean	operator== (const Resource& lhs, const Resource& rhs);
	friend	Boolean	operator!= (const Resource& lhs, const Resource& rhs);
};

class	ostream;
class	istream;
extern	ostream&	operator<< (ostream& out, const Resource& r);
extern	istream&	operator>> (istream& in, Resource& r);


extern	const Resource	kBadResource;

class	AbResourceBuffer {
	public:
		virtual	~AbResourceBuffer ();

		nonvirtual	AbResourceBuffer*	Clone () const;

		nonvirtual	String			GetType () const;
		nonvirtual	void			SetType (const String& type);
		nonvirtual	String			GetKey () const;
		nonvirtual	void			SetKey (const String& key);
		nonvirtual	void*			GetResource () const;
		nonvirtual	size_t	GetLength () const;
		nonvirtual	void			SetResource (const void* resource, size_t nBytes);
				
	protected:
		AbResourceBuffer ();

		virtual	String			GetType_ () const 				= Nil;
		virtual	void			SetType_ (const String& type)	= Nil;
		virtual	String			GetKey_ () const 				= Nil;
		virtual	void			SetKey_ (const String& key)		= Nil;
		virtual	void*			GetResource_ () const 			= Nil;
		virtual	size_t	GetLength_ () const 			= Nil;
		virtual	void			SetResource_ (const void* resource, size_t nBytes)	= Nil;
		
		virtual	AbResourceBuffer*	Clone_ () const = Nil;
};

class	ResourceBuffer_Portable : public AbResourceBuffer {
	public:
		ResourceBuffer_Portable (const String& type, const String& key, const void* resource, size_t nBytes);
		~ResourceBuffer_Portable ();
		
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

	protected:
		override	String			GetType_ () const;
		override	void			SetType_ (const String& type);
		override	String			GetKey_ () const;
		override	void			SetKey_ (const String& key);
		override	void*			GetResource_ () const;
		override	size_t	GetLength_ () const;
		override	void			SetResource_ (const void* resource, size_t nBytes);
	
		override	AbResourceBuffer*	Clone_ () const;

	private:
		String			fType;
		String			fKey;
		void*			fResource;
		size_t	fByteCount;
};




class	AbResourceTableRep;
#if		!qRealTemplatesAvailable
	Declare (Iterator, Resource);
	Declare (Sequence, Resource);
	Declare (Array, Resource);
	Declare (Sequence_Array, Resource);

	Declare (Shared, AbResourceTableRep);
#endif

class	ResourceTable {
	public:
		ResourceTable ();
		ResourceTable (const ResourceTable& table);
		ResourceTable (const PathName& pathName);
		ResourceTable (AbResourceTableRep* sharedPart);
		
		~ResourceTable ();
		
		
		nonvirtual	ResourceTable&	operator= (const ResourceTable& newTable);
		nonvirtual	ResourceTable	GetSubTable (const String& key) const;

		nonvirtual	Boolean	Exists (const String& type, const String& key) const;

		// returns kBadResource if it fails to match
		nonvirtual	Resource	Lookup (const String& type, const String& key) const;

		// must not already exist
		nonvirtual	void	AddResource (const Resource& resource);
		
		// if already exists, must have same type and key
		nonvirtual	void	ChangeResource (const Resource& resource);

		// does not need to exist
		nonvirtual	void	RemoveResource (const String& type, const String& key);
		nonvirtual	void	RemoveAllResources (const String& type);
		nonvirtual	void	RemoveAllResources ();
		
		nonvirtual	Resource	BuildResource (const String& type, const String& key, const void* resource, size_t nBytes);
		nonvirtual	void		WriteResources ();

		nonvirtual	ChangeCount	GetChangeCount () const;
		nonvirtual	Boolean		GetDirty () const;

	private:
#if		qRealTemplatesAvailable
		Shared<AbResourceTableRep>	fRep;
#else
		Shared(AbResourceTableRep)	fRep;
#endif	/* qRealTemplatesAvailable */

		static	AbResourceTableRep*	Clone (const AbResourceTableRep&);
};



class	AbResourceTableRep : public Changeable {
	protected:
		AbResourceTableRep ();
	
		virtual	Boolean	Exists_ (const String& type, const String& key)	 const		= Nil;		

		virtual	Resource	Lookup_ (const String& type, const String& key) const	= Nil;		

		virtual	void	AddResource_ (const Resource& resource)						= Nil;
		virtual	void	ChangeResource_ (const Resource& resource)					= Nil;

		virtual	void	RemoveResource_ (const String& type, const String& key)		= Nil;
		virtual	void	RemoveAllResources_ (const String& type)					= Nil;
		virtual	void	RemoveAllResources_ ()										= Nil;
		
		virtual	Resource	BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes)	= Nil;
		virtual	void		WriteResources_ () = Nil;
	
		virtual	SequenceIterator(Resource)*	MakeResourceIterator () const	= Nil;
		virtual	size_t				CountResources () const			= Nil;

	friend	class	ResourceTable;
};

class	ResourceTableShared_Portable : public AbResourceTableRep {
	public:
		ResourceTableShared_Portable ();
		ResourceTableShared_Portable (const PathName& pathName);
		
	protected:	
		override	Boolean		Exists_ (const String& type, const String& key) const;		
		override	Resource	Lookup_ (const String& type, const String& key) const;		
		override	void		AddResource_ (const Resource& resource);
		override	void		ChangeResource_ (const Resource& resource);
		override	void		RemoveResource_ (const String& type, const String& key);
		override	void		RemoveAllResources_ (const String& type);
		override	void		RemoveAllResources_ ();
	
		override	Resource	BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes);
		override	void		WriteResources_ ();

		override	SequenceIterator(Resource)*	MakeResourceIterator () const;
		override	size_t				CountResources () const;

	private:		
		nonvirtual	void	DoRead (class istream& from);
		nonvirtual	void	DoWrite (class ostream& to) const;

		PathName	fPathName;
		Sequence(Resource)	fResources;
};

class	ResourceSubTable_Portable : public AbResourceTableRep {
	public:
		ResourceSubTable_Portable (AbResourceTableRep* parentTable, const String& key);

	protected:	
		override	Boolean		Exists_ (const String& type, const String& key) const;		
		override	Resource	Lookup_ (const String& type, const String& key) const;		
		override	void		AddResource_ (const Resource& resource);
		override	void		ChangeResource_ (const Resource& resource);
		override	void		RemoveResource_ (const String& type, const String& key);
		override	void		RemoveAllResources_ (const String& type);
		override	void		RemoveAllResources_ ();

		override	Resource	BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes);
		override	void		WriteResources_ ();

		override	SequenceIterator(Resource)*	MakeResourceIterator () const;
		override	size_t				CountResources () const;

	private:	
		ResourceTable	fParentTable;
		Resource		fResource;
		Sequence(Resource)	fResources;
};










// use indirection temporarily so easy to turn off
#define	qMacResources	qMacToolkit && 0

#if		qMacResources

typedef	Resource (*HandleToResource) (char** handle);
typedef	char** (*ResourceToHandle) (const Resource& resource);

struct	MacResTypeEntry;

#if		qMPW_MacroOverflowProblem
#define	MacResTypeEntryPtr		mrePtr
#endif

typedef	class	MacResTypeEntry*	MacResTypeEntryPtr;
Declare (Sequence, MacResTypeEntryPtr);
Declare (Sequence, MacResTypeEntryPtr);

class	ResourceTableShared_Mac : public AbResourceTableRep {
	public:
		ResourceTableShared_Mac (const PathName& pathName);
		~ResourceTableShared_Mac ();
		
		nonvirtual	void	WriteResources ();
		
		static	void	RegisterType (const String& type, UInt32 resType, HandleToResource toResource, ResourceToHandle fromResource);
		static	void	UnregisterType (const String& type);

	protected:	
		override	Boolean		Exists_ (const String& type, const String& key) const;		
		override	Resource	Lookup_ (const String& type, const String& key) const;		
		override	void		AddResource_ (const Resource& resource);
		override	void		ChangeResource_ (const Resource& resource);
		override	void		RemoveResource_ (const String& type, const String& key);
		override	void		RemoveAllResources_ (const String& type);
		override	void		RemoveAllResources_ ();
	
		override	Resource	BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes);

	private:
		nonvirtual	void	ReadResources ();
		nonvirtual	void	DoRead ();
		nonvirtual	void	DoWrite () const;
		static	MacResTypeEntry*	GetRegisteredType (const String& type);

		PathName	fPathName;
		Sequence(Resource)					fResources;
		static Sequence(MacResTypeEntryPtr)	sRegisteredTypes;
};


class	MacResourceTypeRegister {
	public:
		MacResourceTypeRegister ();
		~MacResourceTypeRegister ();
};

extern	Resource	DefaultHandleToResource (char** h);
extern	char**		DefaultResourceToHandle (const Resource& resource);

#endif /* qMacResources */






#define	ResourceDeclare(T)	\
extern	Boolean	Get##T##FromResource (const ResourceTable& table, const String& key, T& resource);	\
extern	void	Put##T##IntoResource (ResourceTable& table, const String& key, const T& resource);

#define	ResourceImplement(T)	\
static	const	String	k##T##ResourceType	= String (#T);	\
Boolean	Get##T##FromResource (const ResourceTable& table, const String& key, T& resource)	\
{	\
	Resource	r = table.Lookup (k##T##ResourceType, key);	\
	if (r != kBadResource) {	\
		StringStream	from ((char*)r.GetResource (), r.GetLength (), ios::out);	\
		from >> resource;	\
		return (True);	\
	}	\
	return (False);	\
}	\
\
void	Put##T##IntoResource (ResourceTable& table, const String& key, const T& resource)	\
{\
	OStringStream	to;	\
	to << resource;	\
	String s = String (to);	\
	table.AddResource (table.BuildResource (k##T##ResourceType, key, s.Peek (), s.GetLength ()));	\
}



Declare (Resource, String);


#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif /* __Resource__ */


