/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Database__
#define	__Database__

#include	"Exception.hh"
#include	"Sequence.hh"
#include	"String.hh"

#include	"Document.hh"




// Terminology misuse here - I think - references to Key should be Attribute, and we
// should have
//	first make typedef String Attribute;
//	then make class Attribuute with String contructure so we can have more efficent internal
//	rep for them.
//

#if		!qRealTemplatesAvailable
	typedef	class	Field*	FieldPtr;
	Declare (Iterator, FieldPtr);
	Declare (Collection, FieldPtr);
	Declare (AbSequence, FieldPtr);
	Declare (Array, FieldPtr);
	Declare (Sequence_Array, FieldPtr);
	Declare (Sequence, FieldPtr);
#endif

class	Field {
	public:
		nonvirtual	String	GetKey () const;
		
		virtual	void		DoRead (class istream& from);
		virtual	void		DoWrite (class ostream& to);

		virtual	String	GetValue () const				= Nil;
		virtual	void	SetValue (const String& value)	= Nil;
		
	protected:
		Field (const String& key);
		
	private:
		String	fKey;
};

		
class	TextField : public Field {
	public:
		TextField (const String& key);
		TextField (const String& key, const String& value);
		
		override	String	GetValue () const;
		override	void	SetValue (const String& value);
	private:
		String	fValue;
};


class	DataBaseException : public Exception {
	public:
		DataBaseException ();
		DataBaseException (const Exception& exception);
		DataBaseException (const DataBaseException& databaseException);
		override	void	Raise ();
};



extern	DataBaseException	gDataBaseException;


class	Record {
	public:
		virtual	void		DoRead (class istream& from)	= Nil;
		virtual	void		DoWrite (class ostream& to)		= Nil;

		virtual	String	GetValue (const String& key) const					= Nil;
		virtual	void	SetValue (const String& key, const String& value)	= Nil;

		virtual	void	AddField (Field* field) 	= Nil;
		virtual	void	RemoveField (Field* field)  = Nil;
		virtual	Field*	GetField (const String& key) const = Nil;

	protected:
		Record ();
};

class	Record_Collection : public Record {
	public:
		Record_Collection ();
		~Record_Collection ();

		override	void		DoRead (class istream& from);
		override	void		DoWrite (class ostream& to);

		override	String	GetValue (const String& key) const;
		override	void	SetValue (const String& key, const String& value);

		override	void	AddField (Field* field);
		override	void	RemoveField (Field* field);
		nonvirtual	CollectionSize	GetFieldCount () const;
		nonvirtual	Iterator(FieldPtr)*	MakeFieldIterator () const;

		override	Field*	GetField (const String& key) const;

	private:
		Sequence(FieldPtr)	fFields;
};




#if		!qRealTemplatesAvailable
	typedef	class	Record*	RecPtr;
	Declare (Iterator, RecPtr);
	Declare (Collection, RecPtr);
	Declare (AbSequence, RecPtr);
	Declare (Array, RecPtr);
	Declare (Sequence_Array, RecPtr);
	Declare (Sequence, RecPtr);
#endif

class	FileBasedDatabase : public FileBasedDocument {
	public:
		FileBasedDatabase (SaveableVersion maxVersion);
		virtual ~FileBasedDatabase ();

		override	off_t	EstimatedFileSize ();

		nonvirtual	Record*	AddBlankRecord ();
		nonvirtual	void	AddRecord (Record* record);
		nonvirtual	void	RemoveRecord (Record* record);
		nonvirtual	CollectionSize	GetRecordCount () const;
		nonvirtual	Iterator(RecPtr)*	MakeRecordIterator ();
	
		nonvirtual	Record*	GetRecord (const String& key, const String& value);

		nonvirtual	void	SetValue (const String& key, const String& value);

	protected:
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount);
	private:
		Sequence(RecPtr)	fRecords;
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __Database__ */

