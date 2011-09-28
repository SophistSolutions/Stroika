/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_iostream_FStreamSupport_h_
#define	_Stroika_Foundation_Streams_iostream_FStreamSupport_h_	1

#include	"../../StroikaPreComp.h"

#include	<fstream>

#include	"../../Characters/TSTring.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {
			namespace	iostream {


				using	Characters::TString;


				// like ifstream::open CTOR - but throws FILE exceptions on failure, and defaults to binary mode
				void		OpenInputFileStream (ifstream* ifStream,
												const TString& fileName,
												ios_base::openmode _Mode = ios_base::in | ios_base::binary,
												int _Prot = (int)ios_base::_Openprot
									);

				// like ifstream::open CTOR - but throws FILE exceptions on failure, and defaults to binary mode
				ifstream&	OpenInputFileStream (ifstream& ifStream,
												const TString& fileName,
												ios_base::openmode _Mode = ios_base::in | ios_base::binary,
												int _Prot = (int)ios_base::_Openprot
									);


				// like ofstream::open CTOR - but throws FILE exceptions on failure, and defaults to binary mode
				void		OpenOutputFileStream (ofstream* ofStream,
												const TString& fileName,
												ios_base::openmode _Mode = ios_base::out | ios_base::binary,
												int _Prot = (int)ios_base::_Openprot
									);

				// like ofstream::open CTOR - but throws FILE exceptions on failure, and defaults to binary mode
				ofstream&	OpenOutputFileStream (ofstream& ofStream,
												const TString& fileName,
												ios_base::openmode _Mode = ios_base::out | ios_base::binary,
												int _Prot = (int)ios_base::_Openprot
									);


			}
		}
	}
}
#endif	/*_Stroika_Foundation_Streams_iostream_FStreamSupport_h_*/
