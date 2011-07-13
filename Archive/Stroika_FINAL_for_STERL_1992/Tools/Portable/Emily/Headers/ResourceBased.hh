/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ResourceBased.hh,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ResourceBased.hh,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/03/06  21:53:47  sterling
 *		motif
 *
 *		Revision 1.1  1992/02/03  17:19:27  sterling
 *		Initial revision
 *
 *
 */

#ifndef	__ResourceBased__
#define	__ResourceBased__

#define	qUseResourceBased	0


#if qUseResourceBased

#include	"Saveable.hh"
#include	"String.hh"
#include	"StreamUtils.hh"



#if		!qRealTemplatesAvailable
// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Sequence.hh"
	#include	"Set.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/
#endif		/*!qRealTemplatesAvailable*/




typedef	void (*ResourceReader) (istream& from, String& key, String& resource);
typedef	void (*ResourceWriter) (ostream& to, const String& key, const String& resource);

class	ResourceTable {
	public:
		ResourceTable ();	
		virtual ~ResourceTable ();
		
		nonvirtual	Boolean	Lookup (const String& key, String* resource);
		nonvirtual	void	AddResource (const String& key, const String& resource);
		nonvirtual	void	RemoveResource (const String& key);
	
		nonvirtual	void	ReadResources (istream& from);
		nonvirtual	void	WriteResources (ostream& to);

		nonvirtual	ResourceReader	GetResourceReader () const;
		nonvirtual	void			SetResourceReader (ResourceReader reader);	// Nil -> reset to default reader
		
		nonvirtual	ResourceWriter	GetResourceWriter () const;
		nonvirtual	void			SetResourceWrite (ResourceWriter writer);	// Nil -> reset to default writer
		
		static	ResourceTable*	GetCurrentTable ();
		static	void			SetCurrentTable (ResourceTable* table);
		
	private:
		ResourceReader		fReader;
		ResourceWriter		fWriter;
		Sequence(String)	fKeys;
		Sequence(String)	fResources;

		static	ResourceReader	sDefaultReader;
		static	ResourceWriter	sDefaultWriter;
		static	ResourceTable*	sCurrentTable;
};

class	ResourceBased  {
	public:
		ResourceBased (const String& key);
		virtual	~ResourceBased ();
		
		nonvirtual	String	GetKey () const;
		nonvirtual	void	SetKey (const String& key);
		
		virtual		String	WriteResource () 						= Nil;
		virtual		void	ReadResource (const String& resource)	= Nil;

	private:
		String	fKey;
};

#endif	/* qUseResourceBased */


#endif	/* __ResourceBased__ */

