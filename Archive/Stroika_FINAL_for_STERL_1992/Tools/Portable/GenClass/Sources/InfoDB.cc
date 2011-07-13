/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/InfoDB.cc,v 1.11 1992/11/24 15:31:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: InfoDB.cc,v $
 *		Revision 1.11  1992/11/24  15:31:17  lewis
 *		#include <stdlib.h> since we call exit();
 *
 *		Revision 1.10  1992/11/11  04:05:05  lewis
 *		Work around gcc 2.3.1 compiler bugs.
 *
 *		Revision 1.9  1992/10/16  20:00:59  lewis
 *		Got rid of qGlarf crap.
 *
 *		Revision 1.8  1992/10/13  20:22:26  lewis
 *		Make hashtable size 1001.
 *
 *		Revision 1.7  1992/10/09  14:21:03  lewis
 *		Add #include MappingOfString_InfoRecord_cc.
 *
 *		Revision 1.4  1992/10/02  23:50:09  lewis
 *		Lots of hacks - got most of the various classes generating properly-
 *		most of the hacks were required to get Mapping to work - some of them
 *		turned out to be unneeded, but I left them in anyhow.
 *
 *		Revision 1.3  1992/09/30  03:27:56  lewis
 *		(LGP checking for STERL)Brokeup implement macros to get to
 *		compile on the mac.
 *
 *		<Copied From GenClass.cc>
 *
 */
#include	<stdlib.h>
#include	<fstream.h>

#include	"Debug.hh"
#include	"File.hh"
#include	"Sequence.hh"
#include	"StreamUtils.hh"

#include	"InfoDB.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include SequenceOfInfoDependency_cc
	#include SequenceOfRenameRecord_cc
	#include MappingOfString_InfoRecord_cc
	#include Mapping_HashTableOfString_InfoRecord_cc
#endif



static	Sequence(InfoDependency)	ReadInfoDependencies (istream& from);



/*
 ********************************************************************************
 **************************************** InfoRecord ****************************
 ********************************************************************************
 */
#if		qGCC_NoAutoGenOfDefaultCTORBug
InfoRecord::InfoRecord ()
{
}
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStartPlusPlusBug
InfoRecord::InfoRecord (const InfoRecord& from):
	fTemplateName(from.fTemplateName),
	fArguments(from.fArguments),
	fRawText(from.fRawText),
	fHeaderDependencies(from.fHeaderDependencies),
	fSourceDependencies(from.fSourceDependencies),
	fRenames(from.fRenames)
{
}

InfoRecord& InfoRecord::operator=(const InfoRecord& rhs)
{
	fTemplateName = rhs.fTemplateName;
	fArguments = rhs.fArguments;
	fRawText = rhs.fRawText;
	fHeaderDependencies = rhs.fHeaderDependencies;
	fSourceDependencies = rhs.fSourceDependencies;
	fRenames = rhs.fRenames;
	return *this;
}
#endif

Boolean operator == (const InfoRecord& lhs, const InfoRecord& rhs)
{
	return Boolean ((lhs.fTemplateName == rhs.fTemplateName) and (lhs.fArguments == rhs.fArguments));
}

ostream&	operator << (ostream& out, const InfoRecord& infoRecord)
{
	out << infoRecord.fTemplateName << endl;
	out << tab;
	ForEach (String, it, infoRecord.fArguments) {
		out << tab << it.Current ();
	}
	out << endl;
	out << tab << "'" << infoRecord.fRawText << "'" << endl;
	ForEach (InfoDependency, it2, infoRecord.fHeaderDependencies) {
		out << tab << it2.Current () << endl;
	}
	ForEach (InfoDependency, it3, infoRecord.fSourceDependencies) {
		out << tab << it3.Current () << endl;
	}
	return (out);
}




/*
 ********************************************************************************
 *********************************** RenameRecord *******************************
 ********************************************************************************
 */
#if		qGCC_NoAutoGenOfDefaultCTORBug
RenameRecord::RenameRecord ()
{
}
#endif



/*
 ********************************************************************************
 ********************************* InfoDependency *******************************
 ********************************************************************************
 */
#if		qGCC_NoAutoGenOfDefaultCTORBug
InfoDependency::InfoDependency ()
{
}
#endif



/*
 ********************************************************************************
 *********************************** ReadInfoRecords ****************************
 ********************************************************************************
 */

Mapping(String,InfoRecord)	ReadInfoRecords (const PathName& pathName)
{
	AbstractFileSystem::FileDescriptor	fd = FileSystem::Get ().Open (pathName, O_RDONLY);
	ifstream	from (fd);

	Mapping(String,InfoRecord)	newMapping =	Mapping_HashTable(String,InfoRecord) (&DefaultStringHashFunction, 1001);

	// Iterate over each DB entry
	while (from) {
		InfoRecord	newInfoRecord;

		for (char c = 'a'; (c != '$') and (from); c = from.get ())
			;

		if (c != '$') {
			break;
		}
		from >> newInfoRecord.fTemplateName;

		// build up list of argument names
		int argsneeded;
		from >> argsneeded;
		for (int i = 1; i <= argsneeded; i++) {
			String argName;
			from >> argName;
			newInfoRecord.fArguments.Append (argName);
		}

		/*
		 * Read in "so-called" raw-text.
		 *
		 * LEWIS: This is robust about not finding Raw text. Since we no longer use it,
		 * blow away this shit, and take all the pound signs out of collecioninfo.
		 */
		{
			char c;
			from >> c;
			if (c == '#') {
				while ((from.peek () != '#') and (from)) {
					char	bigBuf [1000];
					from.getline (bigBuf, sizeof (bigBuf));
					newInfoRecord.fRawText += String (bigBuf, from.gcount ()-1) + String ("\n");
				}
				from >> c;
				if (c != '#') {
					cout << "Missing raw text end marker." << endl;
					::exit (1);
				}
			}
			else {
				from.putback (c);
			}
		}


		newInfoRecord.fHeaderDependencies = ReadInfoDependencies (from);
		newInfoRecord.fSourceDependencies = ReadInfoDependencies (from);

		// Read renames
		int renames;
		from >> renames;
		while ((from) and (renames-- > 0)) {
			RenameRecord	r;

			from >> r.fTemplateName;
			from >> r.fShortName;
			while ((from) and (from.peek () != '\n')) {
				String s;
				from >> s;
				r.fArguments.Append (s);
			}
			newInfoRecord.fRenames.Append (r);
		}

		newMapping.Enter (newInfoRecord.fTemplateName, newInfoRecord);
	}
	

	from.close ();
	::close (fd);

	return (newMapping);
}


/*
 ********************************************************************************
 *********************************** iostream operators *************************
 ********************************************************************************
 */
ostream&	operator << (ostream& out, const RenameRecord& renameRecord)
{
	out << "(" << renameRecord.fTemplateName << tab << renameRecord.fShortName << tab << ":";
	ForEach (String, it, renameRecord.fArguments) {
		out << tab << it.Current ();
	}
	out << ")";
	return (out);
}

ostream&	operator << (ostream& out, const InfoDependency& infoDependency)
{
	out << "(" << (infoDependency.fBangDep?"!":"") << infoDependency.fName << tab << ":";
	ForEach (String, it, infoDependency.fArguments) {
		out << tab << it.Current ();
	}
	out << ")";
	return (out);
}




/*
 ********************************************************************************
 ********************************* ReadInfoDependencies *************************
 ********************************************************************************
 */
static	Sequence(InfoDependency)	ReadInfoDependencies (istream& from)
{
	Sequence(InfoDependency)	infoDependencies;
	int							dependencies	=	0;

	from >> dependencies;
	while ((from) and (dependencies-- > 0)) {
		InfoDependency	iDep;

		iDep.fBangBang = False;
		iDep.fBangDep = False;
		from >> iDep.fName;
		if ((iDep.fName.GetLength () != 0) and (iDep.fName[1] == '!')) {
			iDep.fName = iDep.fName.SubString (2);
			iDep.fBangDep = True;
			if ((iDep.fName.GetLength () != 0) and (iDep.fName[1] == '!')) {
				iDep.fName = iDep.fName.SubString (2);
				iDep.fBangBang = True;
			}
		}
		while ((from) and (from.peek () != '\n')) {
			String	s;
			from >> s;
			iDep.fArguments.Append (s);
		}
		infoDependencies.Append (iDep);
	}
	return (infoDependencies);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

