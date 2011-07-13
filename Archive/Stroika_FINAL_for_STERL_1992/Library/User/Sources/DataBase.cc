/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DataBase.cc,v 1.7 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DataBase.cc,v $
 *		Revision 1.7  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:36:22  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/14  20:01:02  lewis
 *		Use off_t instead of FileOffset.
 *
 *		Revision 1.2  1992/07/02  05:12:48  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/01/21  21:02:50  lewis
 *		Use ForEach macro instead of direcltly using IndirectCollecionIterator.
 *
 *
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DataBase.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, FieldPtr);
	Implement (Collection, FieldPtr);
	Implement (AbSequence, FieldPtr);
	Implement (Array, FieldPtr);
	Implement (Sequence_Array, FieldPtr);
	Implement (Sequence, FieldPtr);
#endif

#if		!qRealTemplatesAvailable
	Implement (Iterator, RecPtr);
	Implement (Collection, RecPtr);
	Implement (AbSequence, RecPtr);
	Implement (Array, RecPtr);
	Implement (Sequence_Array, RecPtr);
	Implement (Sequence, RecPtr);
#endif




/*
 ********************************************************************************
 ******************************** Field *****************************************
 ********************************************************************************
 */
Field::Field (const String& key):
	fKey (key)
{
}

String	Field::GetKey () const
{
	return (fKey);
}

void	Field::DoRead (class istream& from)
{
	char	ch;
	from >> ch;
	Assert (ch == lparen);

	String	foo = kEmptyString;
	ReadString (from, foo);
	fKey = foo;
	ReadString (from, foo);
	SetValue (foo);
	from >> ch;
	Assert (ch == rparen);
}

void	Field::DoWrite (class ostream& to)
{
	Assert (to.good ());
	to << lparen << ' ';
	WriteString (to, GetKey ());
	to << ' ';
	WriteString (to, GetValue ());
	to << rparen << newline;
}








/*
 ********************************************************************************
 ******************************** TextField *************************************
 ********************************************************************************
 */
TextField::TextField (const String& key):
	Field (key),
	fValue (kEmptyString)
{
}

TextField::TextField (const String& key, const String& value):
	Field (key),
	fValue (value)
{
}
		
String	TextField::GetValue () const
{
	return (fValue);
}

void	TextField::SetValue (const String& value)
{
	fValue = value;
}








/*
 ********************************************************************************
 ******************************** DataBaseException *****************************
 ********************************************************************************
 */
DataBaseException	gDataBaseException;

DataBaseException::DataBaseException ():
	Exception ()
{
}

DataBaseException::DataBaseException (const Exception& exception):
	Exception (exception)
{
}

DataBaseException::DataBaseException (const DataBaseException& databaseException):
	Exception (databaseException)
{
}

void	DataBaseException::Raise ()
{
#if		qDebug
	gDebugStream << "throwing database exception" << newline;
#endif
	Exception::Raise ();
}








/*
 ********************************************************************************
 ******************************** Record ****************************************
 ********************************************************************************
 */
Record::Record ()
{
}






/*
 ********************************************************************************
 ****************************** Record_Collection *******************************
 ********************************************************************************
 */
Record_Collection::Record_Collection ():
	fFields ()
{
}

Record_Collection::~Record_Collection ()
{
	ForEach (FieldPtr, it, MakeFieldIterator ()) {
		Field*	field = it.Current ();
		AssertNotNil (field);
		delete field;
	}
}

void	Record_Collection::DoRead (class istream& from)
{
	char	ch;
	from >> ch;
	Assert (ch == lparen);
	
	CollectionSize fieldCount;
	from >> fieldCount;
	Assert ((fieldCount >= 0) and (fieldCount <= 1000));
	while (fieldCount-- > 0) {
		// need to write out metaclass info to support different kinds of fields
		TextField*	field = new TextField (kEmptyString);
		field->DoRead (from);
		AddField (field);
	}

	from >> ch;
	Assert (ch == rparen);
}

void	Record_Collection::DoWrite (class ostream& to)
{
	Assert (to.good ());
	to << lparen << GetFieldCount () << newline;
	ForEach (FieldPtr, it, MakeFieldIterator ()) {
		Field*	field = it.Current ();
		AssertNotNil (field);
		field->DoWrite (to);
	}
	to << rparen << newline;
}

String	Record_Collection::GetValue (const String& key) const
{
	Field*	field = GetField (key);
	if (field == Nil) {
		gDataBaseException.Raise ();
	}
	return (field->GetValue ());
}

void	Record_Collection::SetValue (const String& key, const String& value)
{
	Field*	field = GetField (key);
	if (field == Nil) {
		gDataBaseException.Raise ();
	}
	field->SetValue (value);
}

void	Record_Collection::AddField (Field* field)
{
	RequireNotNil (field);
	fFields.Append (field);
}

void	Record_Collection::RemoveField (Field* field)
{
	RequireNotNil (field);
	fFields.Remove (field);
}

CollectionSize	Record_Collection::GetFieldCount () const
{
	return (fFields.GetLength ());
}

Iterator(FieldPtr)*	Record_Collection::MakeFieldIterator () const
{
	return (fFields);
}

Field*	Record_Collection::GetField (const String& key) const
{
	ForEach (FieldPtr, it, MakeFieldIterator ()) {
		Field*	field = it.Current ();
		AssertNotNil (field);
		if (field->GetKey () == key) {
			return (field);
		}
	}
	return (Nil);
}






/*
 ********************************************************************************
 ***************************** FileBasedDatabase ********************************
 ********************************************************************************
 */
FileBasedDatabase::FileBasedDatabase (SaveableVersion maxVersion):
	FileBasedDocument (maxVersion)
{
}

FileBasedDatabase::~FileBasedDatabase ()
{
	ForEach (RecPtr, it, MakeRecordIterator ()) {
		Record*	record = it.Current ();
		AssertNotNil (record);
		delete record;
	}
}

Record*	FileBasedDatabase::AddBlankRecord ()
{
	Record_Collection*	record = new Record_Collection ();
	AddRecord (record);
	return (record);
}

void	FileBasedDatabase::DoRead_ (class istream& from)
{
	FileBasedDocument::DoRead_ (from);
	
	char	ch;
	from >> ch;
	Assert (ch == lparen);
	
	CollectionSize recordCount;
	from >> recordCount;
	Assert ((recordCount >= 0) and (recordCount <= 1000));
	while (recordCount-- > 0) {
		// need to write out metaclass info to support different kinds of fields
		Record_Collection*	record = new Record_Collection ();
		record->DoRead (from);
		AddRecord (record);
	}

	from >> ch;
	Assert (ch == rparen);
}

void	FileBasedDatabase::DoWrite_ (class ostream& to, int tabCount)
{
	FileBasedDocument::DoWrite_ (to, tabCount);

	Assert (to.good ());
	to << lparen << GetRecordCount () << newline;
	ForEach (RecPtr, it, MakeRecordIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->DoWrite (to);
	}
	to << rparen << newline;
}

off_t	FileBasedDatabase::EstimatedFileSize ()
{
	return (100);		// how to calculate??
}

void	FileBasedDatabase::AddRecord (Record* record)
{
	RequireNotNil (record);
	fRecords.Append (record);
}

void	FileBasedDatabase::RemoveRecord (Record* record)
{
	RequireNotNil (record);
	fRecords.Remove (record);
}

CollectionSize	FileBasedDatabase::GetRecordCount () const
{
	return (fRecords.GetLength ());
}

Iterator(RecPtr)*	FileBasedDatabase::MakeRecordIterator ()
{
	return (fRecords);
}
	
Record*	FileBasedDatabase::GetRecord (const String& key, const String& value)
{
	ForEach (RecPtr, it, MakeRecordIterator ()) {
		Record*	record = it.Current ();
		AssertNotNil (record);
		Try {
			if (record->GetValue (key) == value) {
				return (record);
			}
		}
		Catch () {
		}
	}
	return (Nil);
}

void	FileBasedDatabase::SetValue (const String& key, const String& value)
{
	ForEach (RecPtr, it, MakeRecordIterator ()) {
		Record*	record = it.Current ();
		AssertNotNil (record);
		Try {
			record->SetValue (key, value);
		}
		Catch () {
		}
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


