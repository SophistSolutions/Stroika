/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ResourceBased.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ResourceBased.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/03/06  21:52:58  sterling
 *		motif
 *
 *		Revision 1.1  1992/02/03  17:39:17  sterling
 *		Initial revision
 *
 *
 */

#include	"ResourceBased.hh"



#if qUseResourceBased

static	void	DefaultReader (istream& from, String& key, String& resource)
{
	char c;
	from >> c;
	Require (c == '{');
	
	ReadString (key);
	ReadString (resource);
	
	from >> c;
	Require (c == '}');
}

static	void	DefaultWriter (ostream& to, const String& key, const String& resource)
{
	to << '{' << newline << WriteString (key) << newline << WriteString (resource) << newline << '}' << newline;
}

/*
 ********************************************************************************
 ******************************** ResourceTable *********************************
 ********************************************************************************
 */
ResourceReader	ResourceTable::sDefaultReader = DefaultReader;
ResourceWriter	ResourceTable::sDefaultWriter = DefaultWriter;
ResourceTable*	ResourceTable::sCurrentTable = Nil;

ResourceTable::ResourceTable () :
	fReader (sDefaultReader),
	fWriter (sDefaultWriter)
{
}
		
ResourceTable::~ResourceTable ()
{
	Require (sCurrentTable != this);
}

ResourceTable*	ResourceTable::GetCurrentTable ()
{
	return (sCurrentTable);
}

void	ResourceTable::SetCurrentTable (ResourceTable* table)
{
	sCurrentTable = table;
}

Boolean	ResourceTable::Lookup (const String& key, String* resource)
{
	CollectionSize index = 0;
	ForEach (String, it, fKeys) {
		index++;
		if (it.Current () == key) {
			*resource = fResources[index];
			return (True);
		}
	}
	return (False);
}

void	ResourceTable::AddResource (const String& key, const String& resource)
{
	Require (not fKeys.Contains (key));
	
	fKeys.Append (key);
	fResources.Append (resource);
}

void	ResourceTable::RemoveResource (const String& key)
{
	String	resource;
	if (Lookup (key, &resource)) {
		fKeys.Remove (key);
		fResources.Remove (key);
	}
}

void	ResourceTable::ReadResources (istream& from)
{
	while (from) {
		String	key;
		String	resource;
		(*GetResourceReader ()) (from, key, resource);
		AddResource (key, resource);
	}
}

void	ResourceTable::WriteResources (ostream& to)
{
	ForEach (String, it, fKeys) {
		String	key = it.Current ();
		String	resource;
		if (Lookup (key, &resource)) {
			(*GetResourceWriter ()) (to, key, resource);
		}
	}
}

ResourceReader	ResourceTable::GetResourceReader () const
{
	EnsureNotNil (fReader);
	return (fReader);
}

void	ResourceTable::SetResourceReader (ResourceReader reader)
{
	fReader = reader;
	if (fReader == Nil) {
		fReader = sDefaultReader;
	}
}
		
ResourceWriter	ResourceTable::GetResourceWriter () const
{
	EnsureNotNil (fWriter);
	return (fWriter);
}

void	ResourceTable::SetResourceWrite (ResourceWriter writer)
{
	fWriter = writer;
	if (fWriter == Nil) {
		fWriter = sDefaultWriter;
	}
}


/*
 ********************************************************************************
 ******************************** ResourceBased *********************************
 ********************************************************************************
 */
ResourceBased::ResourceBased (const String& key) :
	fKey (key)
{
}

ResourceBased::~ResourceBased ()
{
}
		
String	ResourceBased::GetKey () const
{
	return (fKey);
}

void	ResourceBased::SetKey (const String& key)
{
	fKey = key;
}
		
#endif	/* qUseResourceBased */
