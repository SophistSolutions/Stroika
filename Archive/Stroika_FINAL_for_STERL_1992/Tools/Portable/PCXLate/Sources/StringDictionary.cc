/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/StringDictionary.cc,v 1.6 1992/10/21 04:58:50 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: StringDictionary.cc,v $
 *		Revision 1.6  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.5  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.3  1992/07/14  20:11:00  lewis
 *		Use FileSystem::Get ().Open..
 *
 *		Revision 1.2  1992/07/03  04:39:41  lewis
 *		Include fstream even under MacOS.
 *
 *
 */

#include	<stdio.h>
#if		qUnixOS
	#include	<unistd.h>
#endif
#include	<fstream.h>

#include	"File.hh"
#include	"Mapping_HashTable.hh"
#include	"StreamUtils.hh"

#include	"StringDictionary.hh"



Mapping(String,String)	ReadMap (const PathName& pathName)
{
	Mapping_HashTable(String,String)	aDictionary (&DefaultStringHashFunction, 1001);

	int	fd = FileSystem::Get ().Open (pathName, O_RDONLY);
	ifstream	inFile = int (fd);
	Assert (inFile);
	while (inFile) {
		char	line [1024];
		char	s1 [1024];
		char	s2 [1024];
		inFile.getline (line, sizeof (line));
		if (inFile and sscanf (line, "%s	%s\n", &s1, &s2) == 2) {
			aDictionary.Enter (s1, s2);
		}
		else {
			if (inFile) {
				cout << "bad line:" << line << endl;
			}
		}
	}
	::close (fd);					// not sure needed???
	return (aDictionary);
}

void	WriteMap (const PathName& pathName, const Mapping(String,String)& dictionary)
{	
#if		qMacOS
	int	fd = FileSystem::Get_MacOS ().Open (pathName, O_WRONLY | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
#else
	int	fd = FileSystem::Get ().Open (pathName, O_WRONLY | O_TRUNC | O_CREAT);
#endif
	ofstream	outFile = int (fd);
	Assert (outFile);
	ForEach (MapElement (String, String), it, dictionary) {
		outFile << it.Current ().fKey << " " << it.Current ().fElt << endl;
	}
	outFile.close ();
	::close (fd);					// not sure needed???
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

