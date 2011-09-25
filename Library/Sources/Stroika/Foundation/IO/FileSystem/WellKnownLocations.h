/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_
#define	_Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Characters/TString.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	FileSystem {

				using	Characters::TString;

				// These GetSpecialDir_XXX routines always return a valid directory (if createIfNotPresent) - and
				// it always ends in a '\\'
				//
				TString	GetSpecialDir_MyDocuments (bool createIfNotPresent = true);
				TString	GetSpecialDir_AppData (bool createIfNotPresent = true);
	#if			qPlatform_Windows
				TString	GetSpecialDir_WinSxS ();	// empty string if doesn't exist
	#endif

				TString	GetSpecialDir_GetTempDir ();


			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"WellKnownLocations.inl"
