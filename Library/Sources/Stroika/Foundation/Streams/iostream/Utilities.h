/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_iostream_Utilities_h_
#define	_Stroika_Foundation_Streams_iostream_Utilities_h_	1

#include	"../../StroikaPreComp.h"

#include	<iostream>

#include	"../../Characters/TString.h"


/*
 * TODO:
 *		o	While we have a lot of code that is iostream based, and before the Stroika stream stuff matures, many of these
 *			utiltiies maybe handy. I'm not sure we will want them long-term however.
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {
			namespace	iostream {


				using	Characters::TString;

				wstring	ReadTextStream (istream& in);
				wstring	ReadTextStream (wistream& in);

				#if 0
				void	WriteString (ostream& out, const wstring& s);
				wstring	ReadString (istream& in);
				#endif

				vector<Byte>	ReadBytes (istream& in);
				void			WriteBytes (ostream& out, const vector<Byte>& s);


			}
		}
	}
}
#endif	/*_Stroika_Foundation_Streams_iostream_Utilities_h_*/
