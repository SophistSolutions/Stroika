/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Resource.cc,v 1.13 1992/12/05 17:38:46 lewis Exp $
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
 *	$Log: Resource.cc,v $
 *		Revision 1.13  1992/12/05  17:38:46  lewis
 *		Renamed Try->try, and Catch->catch - see exception.hh for explation.
 *
 *		Revision 1.12  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.11  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.10  1992/09/15  17:19:01  lewis
 *		Got rid of reference to AbSequence.
 *
 *		Revision 1.9  1992/09/11  17:06:29  sterling
 *		*** empty log message ***
 *
 *		Revision 1.7  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.5  1992/07/21  06:01:21  lewis
 *		Make resource iterator functions return const iterators, and use Sequence_DoubleLinkList
 *		instead of Sequence_DoubleLinkListPtr - since the later is now obsolete.
 *
 *		Revision 1.4  1992/07/14  19:41:10  lewis
 *		Use FileSystem::Get_MacOS () for macos version of open.
 *
 *		Revision 1.3  1992/07/10  22:24:54  lewis
 *		React to changes in new file stuff - scope constants, and open functions, and a few renames.
 *
 *		Revision 1.2  1992/07/02  03:25:36  lewis
 *		Renamed DoublyLinkedList/Sequence_DoublyLinkedList/Sequence_DoublyLLOfPointers ->
 *		DoubleLinkList/Sequence_DoubleLinkList/Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/06/09  23:41:51  lewis
 *		Moved GDI specific resource stuff to Graphix: GraphixResources.cc
 *
 *		Revision 1.9  92/06/09  22:19:43  22:19:43  lewis (Lewis Pringle)
 *		Changed SequenceForEach to ForEachS.
 *		
 *		Revision 1.8  92/06/09  12:35:45  12:35:45  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared.
 *		
 *		Revision 1.7  92/06/09  15:22:20  15:22:20  sterling (Sterling Wight)
 *		many changes, including using templates for reference counting
 *		
 *		Revision 1.2  92/05/19  10:40:29  10:40:29  sterling (Sterling Wight)
 *		converted from stream to void*
 *
 *		Revision 1.1  92/05/13  12:32:50  12:32:50  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 */
 
#include	<fstream.h>

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"Resource.hh"

#if 0


#if		!qRealTemplatesAvailable
Implement (Shared, AbResourceBuffer);
#endif



// STERL- YOU CANNOT KEEP DOING THIS - PLEASE!!!!!!
// This calls CTOR for Resrouce () which uses kEmptyString - I havent tried it yet on UNIX, but you should
// realize by now this is VERY unsafe - Did you mean for this to be static const BTW????
// LGP 5/13/92
const Resource	kBadResource = Resource ();




/*
 ********************************************************************************
 *************************************** Resource *******************************
 ********************************************************************************
 */
Resource::Resource () :
	fRep (&Clone, Nil)
{
	static	Shared(AbResourceBuffer)	sSharedPart (&Clone, new ResourceBuffer_Portable (kEmptyString, kEmptyString, Nil, 0));
	fRep = sSharedPart;
}

Resource::Resource (const Resource& from) :
	fRep (from.fRep)
{
}

Resource::Resource (AbResourceBuffer* sharedPart) :
	fRep (&Clone, sharedPart)
{
	Require (fRep.CountReferences () == 1);
}

Resource::~Resource ()
{
}

Resource&	Resource::operator= (const Resource& newResource)
{
	fRep = newResource.fRep;
	return (*this);
}
		
String	Resource::GetType () const
{
	return (fRep->GetType ());
}

void	Resource::SetType (const String& type)
{
	fRep->SetType (type);
}

String	Resource::GetKey () const
{
	return (fRep->GetKey ());
}

void	Resource::SetKey (const String& key)
{
	fRep->SetKey (key);
}

void*		Resource::GetResource () const
{
	return (fRep->GetResource ());
}

size_t	Resource::GetLength () const
{
	return (fRep->GetLength ());
}

void	Resource::SetResource (void* resource, size_t nBytes)
{
	fRep->SetResource (resource, nBytes);
}

AbResourceBuffer*	Resource::Clone (const AbResourceBuffer& res)
{
	return (res.Clone ());
}

Boolean	operator== (const Resource& lhs, const Resource& rhs)
{
	if (lhs.fRep == rhs.fRep) {
		return (True);
	}
	if (lhs.GetLength () == rhs.GetLength ()) {
		if (lhs.GetType () == rhs.GetType ()) {
			if (lhs.GetKey () == rhs.GetKey ()) {
				return (Boolean (!memcmp (lhs.GetResource (), rhs.GetResource (), rhs.GetLength ())));
			}
		}
	}
	return (False);
}

Boolean	operator!= (const Resource& lhs, const Resource& rhs)
{
	return (not operator== (lhs, rhs));
}

ostream&	operator<< (ostream& out, const Resource& r)
{
	size_t length = r.GetLength ();
	WriteString (out, r.GetType ());
	WriteString (out, r.GetKey ());
	out << length << ':';
	const char*	resource = (char*)r.GetResource ();
	for (size_t i = 0; i < length; i++) {
		out << resource[i];
	}
	return (out);
}

istream&	operator>> (istream& in, Resource& r)
{
	String	type;
	String	key;
	size_t	length;
	ReadString (in, type);
	ReadString (in, key);
	in >> length;
	char delimeter;
	in >> delimeter;
	Assert (delimeter == ':');
	
	char	stackBuffer[1024];
	char*	buffer;
	if (length >= 1024) {
		buffer = new char[length+1];
	}
	else {
		buffer = stackBuffer;
	}
	
	in.getline (buffer, length+1);
	r.SetType (type);
	r.SetKey (key);
	r.SetResource (buffer, length);
	
	if (length > 1024) {
		delete buffer;
	}
	
	return (in);
}





/*
 ********************************************************************************
 ******************************** AbResourceBuffer ************************
 ********************************************************************************
 */
AbResourceBuffer::AbResourceBuffer ()
{
}

AbResourceBuffer::~AbResourceBuffer ()
{
}

AbResourceBuffer*	AbResourceBuffer::Clone () const
{
	return (Clone_ ());
}

String	AbResourceBuffer::GetType () const
{
	return (GetType_ ());
}

void	AbResourceBuffer::SetType (const String& type)
{
	SetType_ (type);
}

String	AbResourceBuffer::GetKey () const
{
	return (GetKey_ ());
}

void	AbResourceBuffer::SetKey (const String& key)
{
	SetKey_ (key);
}

void*	AbResourceBuffer::GetResource () const
{
	return (GetResource_ ());
}

size_t	AbResourceBuffer::GetLength () const
{
	return (GetLength_ ());
}

void	AbResourceBuffer::SetResource (const void* resource, size_t nBytes)
{
	SetResource_ (resource, nBytes);
}






/*
 ********************************************************************************
 ******************************* ResourceBuffer_Portable ************************
 ********************************************************************************
 */
ResourceBuffer_Portable::ResourceBuffer_Portable (const String& type, const String& key, const void* resource, size_t nBytes) :
	AbResourceBuffer (),
	fType (type),
	fKey (key),
	fResource (Nil),
	fByteCount (nBytes)
{
	Require ((resource != Nil) or (nBytes == 0));
	fResource = new char[nBytes];
	memcpy (fResource, resource, nBytes);
}

ResourceBuffer_Portable::~ResourceBuffer_Portable ()
{
	delete fResource;
}

AbResourceBuffer*	ResourceBuffer_Portable::Clone_ () const
{
	return (new ResourceBuffer_Portable (GetType (), GetKey (), GetResource (), GetLength ()));
}

String	ResourceBuffer_Portable::GetType_ () const
{
	return (fType);
}

void	ResourceBuffer_Portable::SetType_ (const String& type)
{
	fType = type;
}

String	ResourceBuffer_Portable::GetKey_ () const
{
	return (fKey);
}

void	ResourceBuffer_Portable::SetKey_ (const String& key)
{
	fKey = key;
}

void*	ResourceBuffer_Portable::GetResource_ () const
{
	return (fResource);
}

size_t	ResourceBuffer_Portable::GetLength_ () const
{
	return (fByteCount);
}

void	ResourceBuffer_Portable::SetResource_ (const void* resource, size_t nBytes)
{
	if (fByteCount != nBytes) {
		delete fResource;
		fResource = new char[nBytes];
		fByteCount = nBytes;
	}
	memcpy (fResource, resource, nBytes);
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (ResourceBuffer_Portable);
BlockAllocatedImplement (ResourceBuffer_Portable);
#endif

void*	ResourceBuffer_Portable::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<ResourceBuffer_Portable>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(ResourceBuffer_Portable)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	ResourceBuffer_Portable::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<ResourceBuffer_Portable>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(ResourceBuffer_Portable)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}







#if		!qRealTemplatesAvailable
	Implement (Iterator, Resource);
	Implement (Array, Resource);
	Implement (Sequence_Array, Resource);
	Implement (Sequence, Resource);
	
	Implement (Shared, AbResourceTableRep);
#endif

/*
 ********************************************************************************
 *********************************** ResourceTable ******************************
 ********************************************************************************
 */
ResourceTable::ResourceTable () :
#if qMacResources
	fRep (&Clone, new ResourceTableShared_Mac (PathName::kBad))
#else
	fRep (&Clone, new ResourceTableShared_Portable ())
#endif
{
}

ResourceTable::ResourceTable (const ResourceTable& table) :
	fRep (&Clone, table.fRep)
{
}

ResourceTable::ResourceTable (const PathName& pathName) :
#if qMacResources
	fRep (&Clone, new ResourceTableShared_Mac (pathName))
#else
	fRep (&Clone, new ResourceTableShared_Portable (pathName))
#endif
{
}

ResourceTable::ResourceTable (AbResourceTableRep* sharedPart) :
	fRep (&Clone, sharedPart)
{
}

ResourceTable::~ResourceTable ()
{
	if (GetDirty ()) {
		WriteResources ();
	}
}

ResourceTable&	ResourceTable::operator= (const ResourceTable& newTable)
{
	fRep = newTable.fRep;
	return (*this);
}

ResourceTable	ResourceTable::GetSubTable (const String& key) const
{
#if qMacResources
#error
#else
	return (ResourceTable (new ResourceSubTable_Portable (fRep.GetPointer (), key)));
#endif
}

Boolean	ResourceTable::Exists (const String& type, const String& key) const
{
	return (fRep->Exists_ (type, key));
}
		
Resource	ResourceTable::Lookup (const String& type, const String& key) const
{
	return (fRep->Lookup_ (type, key));
}

void	ResourceTable::AddResource (const Resource& resource)
{
	fRep->AddResource_ (resource);
	Ensure (Exists (resource.GetType (), resource.GetKey ())); 
}

void	ResourceTable::ChangeResource (const Resource& resource)
{
	Require (Exists (resource.GetType (), resource.GetKey ())); 
	fRep->ChangeResource_ (resource);
}

void	ResourceTable::RemoveResource (const String& type, const String& key)
{
	fRep->RemoveResource_ (type, key);
	Ensure (not Exists (type, key)); 
}

void	ResourceTable::RemoveAllResources (const String& type)
{
	fRep->RemoveAllResources_ (type);
}

void	ResourceTable::RemoveAllResources ()
{
	fRep->RemoveAllResources_ ();
}

Resource	ResourceTable::BuildResource (const String& type, const String& key, const void* resource, size_t nBytes)
{
	return (fRep->BuildResource_ (type, key, resource, nBytes));
}

void	ResourceTable::WriteResources ()
{
	fRep->WriteResources_ ();
}


ChangeCount	ResourceTable::GetChangeCount () const
{
	return (fRep->GetChangeCount ());
}

Boolean	ResourceTable::GetDirty () const
{
	return (fRep->GetDirty ());
}


/*
 ********************************************************************************
 **************************** AbResourceTableRep ***********************
 ********************************************************************************
 */
AbResourceTableRep::AbResourceTableRep () :
	Changeable ()
{
}

/*
 ********************************************************************************
 *************************** ResourceTableShared_Portable ***********************
 ********************************************************************************
 */
ResourceTableShared_Portable::ResourceTableShared_Portable () :
	AbResourceTableRep (),
	fResources (),
	fPathName (PathName::kBad)
{
}

ResourceTableShared_Portable::ResourceTableShared_Portable (const PathName& pathName) :
	AbResourceTableRep (),
	fResources (),
	fPathName (pathName)
{
	if (fPathName != PathName::kBad) {
		try {
#if		qMacOS
			int	fd = FileSystem::Get_MacOS ().Open (fPathName, O_RDONLY, 'RTBL', 'TEXT');
#else
			int	fd = FileSystem::Get ().Open (fPathName, O_RDONLY);
#endif
			ifstream	inFile = fd;
			Assert (inFile);
			try {
				DoRead (inFile);
			}
			catch () {
			}
			inFile.close ();
			::close (fd);				// not sure why needed...
		}
		catch () {
		}
	}
}

Boolean	ResourceTableShared_Portable::Exists_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (True);
			}
		}
	}
	return (False);
}

Resource	ResourceTableShared_Portable::Lookup_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (current);
			}
		}
	}
	return (kBadResource);
}

void	ResourceTableShared_Portable::AddResource_ (const Resource& resource)
{
	Require (not Exists_ (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
	PlusChangeCount ();
}

void	ResourceTableShared_Portable::ChangeResource_ (const Resource& resource)
{
	PlusChangeCount ();
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == resource.GetType ()) {
			if (current.GetKey () == resource.GetKey ()) {
				fResources.SetAt (resource, it.CurrentIndex ());
				return;
			}
		}
	}
	Assert (not Exists_ (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
}

void	ResourceTableShared_Portable::RemoveResource_ (const String& type, const String& key)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				fResources.RemoveAt (it.CurrentIndex ());
				PlusChangeCount ();
				return;
			}
		}
	}
	Ensure (not Exists_ (type, key));
}

void	ResourceTableShared_Portable::RemoveAllResources_ (const String& type)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			fResources.RemoveAt (it.CurrentIndex ());
		}
	}
	PlusChangeCount ();
}

Resource	ResourceTableShared_Portable::BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes)
{
	return (Resource (new ResourceBuffer_Portable (type, key, resource, nBytes)));
}

void	ResourceTableShared_Portable::RemoveAllResources_ ()
{
	fResources.RemoveAll ();
	PlusChangeCount ();
}

SequenceIterator(Resource)*	ResourceTableShared_Portable::MakeResourceIterator () const
{
	return (fResources.MakeSequenceIterator (eSequenceForward));
}

size_t	ResourceTableShared_Portable::CountResources () const
{
	return (fResources.GetLength ());
}

void	ResourceTableShared_Portable::WriteResources_ ()
{
	Exception*	theException	=	Nil;

#if		qMacOS
	int	fd = FileSystem::Get_MacOS ().Open (fPathName, O_WRONLY | O_TRUNC | O_CREAT, 'RTBL', 'TEXT');
#else
	int	fd = FileSystem::Get ().Open (fPathName, O_WRONLY | O_TRUNC | O_CREAT);
#endif
	try {
		ofstream	outFile = int (fd);
		Assert (outFile);
		try {
			DoWrite (outFile);
		}
		catch () {
			outFile.close ();
			outFile.ofstream::~ofstream ();
			_this_catch_->Raise ();
		}
		Assert (outFile);
		outFile.close ();
	}
	catch () {
		theException = _this_catch_;
	}
	::close (fd);					// not sure needed???

	if (theException != Nil) {
		theException->Raise ();
	}
}

void	ResourceTableShared_Portable::DoRead (class istream& from)
{
	Assert (CountResources () == 0);

	size_t length = 0;
	from >> length;
	for (size_t i = 1; i <= length; i++) {
		Resource entry;
		from >> entry;
		AddResource_ (entry);
	}
	Ensure (CountResources () == length);
}

void	ResourceTableShared_Portable::DoWrite (class ostream& to) const
{
	size_t entryCount = CountResources ();
	to << newline << entryCount << newline;
	ForEach (Resource, it, MakeResourceIterator ()) {
		to << it.Current ();
		to << newline;
	}
}


/*
 ********************************************************************************
 ***************************** ResourceSubTable_Portable ************************
 ********************************************************************************
 */
ResourceSubTable_Portable::ResourceSubTable_Portable (AbResourceTableRep* parentTable, const String& key) :
	AbResourceTableRep (),
	fParentTable (parentTable),
	fResources (),
	fResource ()
{
	fResource = fParentTable.Lookup ("SubTable", key);
	if (fResource == kBadResource) {
		fResource = Resource (new ResourceBuffer_Portable ("SubTable", key, Nil, 0));
	}
	else {
		// unparse to get all the stuff out
	}
}

void	ResourceSubTable_Portable::WriteResources_ ()
{
	if (GetDirty ()) {
		// parse table into resource format
		
		fParentTable.ChangeResource (fResource);
	}
}

Boolean	ResourceSubTable_Portable::Exists_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (True);
			}
		}
	}
	return (False);
}

Resource	ResourceSubTable_Portable::Lookup_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (current);
			}
		}
	}
	return (kBadResource);
}

void	ResourceSubTable_Portable::AddResource_ (const Resource& resource)
{
	Require (not Exists_ (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
	PlusChangeCount ();
}

void	ResourceSubTable_Portable::ChangeResource_ (const Resource& resource)
{
	PlusChangeCount ();
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == resource.GetType ()) {
			if (current.GetKey () == resource.GetKey ()) {
				fResources.SetAt (resource, it.CurrentIndex ());
				return;
			}
		}
	}
	Assert (not Exists_ (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
}

void	ResourceSubTable_Portable::RemoveResource_ (const String& type, const String& key)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				fResources.RemoveAt (it.CurrentIndex ());
				PlusChangeCount ();
				return;
			}
		}
	}
	Ensure (not Exists_ (type, key));
}

void	ResourceSubTable_Portable::RemoveAllResources_ (const String& type)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			fResources.RemoveAt (it.CurrentIndex ());
		}
	}
	PlusChangeCount ();
}

Resource	ResourceSubTable_Portable::BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes)
{
	return (Resource (new ResourceBuffer_Portable (type, key, resource, nBytes)));
}

void	ResourceSubTable_Portable::RemoveAllResources_ ()
{
	fResources.RemoveAll ();
	PlusChangeCount ();
}

SequenceIterator(Resource)*	ResourceSubTable_Portable::MakeResourceIterator () const
{
	return (fResources.MakeSequenceIterator (eSequenceForward));
}

size_t	ResourceSubTable_Portable::CountResources () const
{
	return (fResources.GetLength ());
}




// more efficient to hand implement
static	const	String	kStringResourceType	= String ("String");
Boolean	GetStringFromResource (const ResourceTable& table, const String& key, String& string)
{
	Resource r = table.Lookup (kStringResourceType, key);
	if (r == kBadResource) {
		return (False);
	}
	string = String (r.GetResource (), r.GetLength ());
	return (True);
}

void	PutStringIntoResource (ResourceTable& table, const String& key, const String& string)
{
	table.AddResource (table.BuildResource (kStringResourceType, key, string.Peek (), string.GetLength ()));
}




#if qMacResources

#include	"OSRenamePre.hh"

#include	<Memory.h>
#include	<Resources.h>

#include	"OSRenamePost.hh"

struct	MacResTypeEntry {
	MacResTypeEntry (const String& type, UInt32 resType, HandleToResource toResource, ResourceToHandle fromResource)	:
		fType (type),
		fResType (resType),
		fToResource (toResource),
		fFromResource (fromResource)
	{
	}
	
	String				fType;
	UInt32				fResType;
	HandleToResource	fToResource;
	ResourceToHandle	fFromResource;
};


Implement (Sequence, MacResTypeEntryPtr);
Implement (Sequence_DoubleLinkList, MacResTypeEntryPtr);

// Register default types
Resource		DefaultHandleToResource (osHandle h)
{
	ResourceBuffer_Portable* b = new ResourceBuffer_Portable (kEmptyString, kEmptyString, *h, ::GetHandleSize (h));
	return (Resource (b));
}

osHandle	DefaultResourceToHandle (const Resource& resource)
{
	Size length = resource.GetLength ();
	
	osHandle h = ::NewHandle (length);
	AssertNotNil (h);
	
	char	handleFlags = ::HGetState (h);
	::HLock (h);
	
	memcpy (*h, resource.GetResource (), length);
	
	::HSetState (h, handleFlags);
	
	return (h);
}




/*
 ********************************************************************************
 ******************************** ResourceTableShared_Mac ***********************
 ********************************************************************************
 */

Sequence_DoubleLinkList(MacResTypeEntryPtr)	ResourceTableShared_Mac::sRegisteredTypes;

ResourceTableShared_Mac::ResourceTableShared_Mac (const PathName& pathName) :
	AbResourceTableRep (),
	fPathName (pathName),
	fResources ()
{
	ReadResources ();
}

ResourceTableShared_Mac::~ResourceTableShared_Mac ()
{
	if (GetDirty ()) {
		WriteResources ();
	}
}

Boolean	ResourceTableShared_Mac::Exists_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (True);
			}
		}
	}
	return (False);
}

Resource	ResourceTableShared_Mac::Lookup_ (const String& type, const String& key) const
{
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				return (current);
			}
		}
	}
	return (kBadResource);
}

void	ResourceTableShared_Mac::AddResource_ (const Resource& resource)
{
	Require (not Exists (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
	PlusChangeCount ();
}

void	ResourceTableShared_Mac::ChangeResource_ (const Resource& resource)
{
	PlusChangeCount ();
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == resource.GetType ()) {
			if (current.GetKey () == resource.GetKey ()) {
				fResources.SetAt (resource, it.CurrentIndex ());
				return;
			}
		}
	}
	Assert (not Exists (resource.GetType (), resource.GetKey ()));
	fResources.Append (resource); 
}

void	ResourceTableShared_Mac::RemoveResource_ (const String& type, const String& key)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			if (current.GetKey () == key) {
				fResources.RemoveAt (it.CurrentIndex ());
				PlusChangeCount ();
				return;
			}
		}
	}
	Ensure (not Exists (type, key));
}

void	ResourceTableShared_Mac::RemoveAllResources_ (const String& type)
{
	ForEachT (SequenceIterator, Resource, it, fResources) {
		Resource current = it.Current ();
		if (current.GetType () == type) {
			fResources.RemoveAt (it.CurrentIndex ());
		}
	}
	PlusChangeCount ();
}

void	ResourceTableShared_Mac::RemoveAllResources_ ()
{
	fResources.RemoveAll ();
	PlusChangeCount ();
}

Resource	ResourceTableShared_Mac::BuildResource_ (const String& type, const String& key, const void* resource, size_t nBytes)
{
	return (Resource (new ResourceBuffer_Portable (type, key, resource, nBytes)));
}


void	ResourceTableShared_Mac::RegisterType (const String& type, UInt32 resType, HandleToResource toResource, ResourceToHandle fromResource)
{
	MacResTypeEntry*	entry = GetRegisteredType (type);
	if (entry == Nil) {
		entry = new MacResTypeEntry (type, resType, toResource, fromResource);
		sRegisteredTypes.Append (entry);
	}
	else {
		entry->fResType = resType;
		entry->fToResource = toResource;
		entry->fFromResource = fromResource;
	}
	Ensure (GetRegisteredType (type) != Nil);
}

void	ResourceTableShared_Mac::UnregisterType (const String& type)
{
	ForEach (MacResTypeEntryPtr, it, sRegisteredTypes) {
		MacResTypeEntry* current = it.Current ();
		if (current->fType == type) {
			sRegisteredTypes.Remove (current);
			delete current;
			break;
		}
	}
	Ensure (GetRegisteredType (type) == Nil);
}

void	ResourceTableShared_Mac::ReadResources ()
{
	try {
		int	resFD = 0;
		resFD = OpenResFile (fPathName, O_RDWR);
		if (resFD != -1) {
			::UseResFile (resFD);
			Assert (::ResError () == 0);
			try {
				DoRead ();
			}
			catch () {
			}
			::CloseResFile (resFD);
			Assert (::ResError () == 0);
		}
	}
	catch () {
	}
}

void	ResourceTableShared_Mac::WriteResources ()
{
	try {
		int	resFD = 0;
		resFD = OpenResFile (fPathName, O_RDWR | O_CREAT);
		Assert (::ResError () == 0);
		Assert (resFD != -1);	// should probably throw an exception
		::UseResFile (resFD);
		Assert (::ResError () == 0);
		try {
			DoWrite ();
		}
		catch () {
		}
		::CloseResFile (resFD);
		Assert (::ResError () == 0);
	}
	catch () {
	}
}

MacResTypeEntry*	ResourceTableShared_Mac::GetRegisteredType (const String& type)
{
	ForEach (MacResTypeEntryPtr, it, sRegisteredTypes) {
		if (it.Current ()->fType == type) {
			return (it.Current ());
		}
	}
	return (Nil);
}

void	ResourceTableShared_Mac::DoRead ()
{
	// this method assumes that the resource file currently in use is the right one
	// safe assumption as am a private method
	
	ForEach (MacResTypeEntryPtr, it, sRegisteredTypes) {
		MacResTypeEntry* current = it.Current ();
		AssertNotNil (current->fToResource);
		
		// get every resource from current file of this type
		int totalResources = ::Count1Resources (current->fResType);
		for (int i = 1; i <= totalResources; i++) {
			osHandle h = ::Get1IndResource (current->fResType, i);
			AssertNotNil (h);
			
			Int16		theID;
			UInt32		theType;
			osStr255	resName;
			::GetResInfo (h, &theID, &theType, resName);
			
			// Policy decision: only snag named resources
			if (resName[0] != 0) {
				String	key = String ((void*)&resName[1], resName[0]);
				
				char	handleFlags = ::HGetState (h);
				::HLock (h);
	
				Resource r = (*current->fToResource) (h);
				r.SetType (current->fType);
				r.SetKey (key);
				
				::HSetState (h, handleFlags);
	
				ChangeResource (r);
			}
			::ReleaseResource (h);
		}
	}
}

void	ResourceTableShared_Mac::DoWrite () const
{
	// this method assumes that the resource file currently in use is the right one
	// safe assumption as am a private method
	
	ForEach (Resource, it, fResources) {
		Resource current = it.Current ();
		MacResTypeEntry* macResType = GetRegisteredType (current.GetType ());
		if (macResType == Nil) {
			// what to do?, should I drop on floor or get upset???
			// I think I need to drop it on the floor, and we do not guarantee
			// that you can convert between portable and native implementations
			// without losing information
#if qDebug
			DebugMessage ("tried to write macintosh resource of unknown type: %s", current.GetType ().Peek ());
#endif
		}
		else {
			osStr255	resName;
			current.GetKey ().ToPStringTrunc (resName, sizeof (resName));
	
			// first nuke any existing guy
			osHandle	h = ::Get1NamedResource (macResType->fResType, resName);
			if (h != Nil) {
				::RmveResource (h);
				::DisposHandle (h);
				h = Nil;
			}
			
			// now build new handle
			AssertNotNil (macResType->fFromResource);
			h = (*macResType->fFromResource) (current);
			AssertNotNil (h);
			
			// and add it to resource file
			::AddResource (h, macResType->fResType, ::Unique1ID (macResType->fResType), resName);
		}
	}
	
	SetClean ();
}



MacResourceTypeRegister::MacResourceTypeRegister ()
{
	ResourceTableShared_Mac::RegisterType (kColorResourceType, 	'CoLr', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kFontResourceType, 	'FoNt', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kPixelMapResourceType, 'PxMp', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kPointResourceType, 	'PoNt', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kRectResourceType, 	'ReCt', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kStringResourceType, 	'STR ', DefaultHandleToResource, DefaultResourceToHandle);
	ResourceTableShared_Mac::RegisterType (kTileResourceType, 	'TiLe', DefaultHandleToResource, DefaultResourceToHandle);
}

MacResourceTypeRegister::~MacResourceTypeRegister ()
{
	ResourceTableShared_Mac::UnregisterType (kColorResourceType);
	ResourceTableShared_Mac::UnregisterType (kFontResourceType);
	ResourceTableShared_Mac::UnregisterType (kPixelMapResourceType);
	ResourceTableShared_Mac::UnregisterType (kPointResourceType);
	ResourceTableShared_Mac::UnregisterType (kRectResourceType);
	ResourceTableShared_Mac::UnregisterType (kStringResourceType);
	ResourceTableShared_Mac::UnregisterType (kTileResourceType);
}

#endif	/* qMacResources */


#endif

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
