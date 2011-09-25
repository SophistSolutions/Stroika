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
				// 
				// See also http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
				//
				namespace	WellKnownLocations {

					TString	GetMyDocuments (bool createIfNotPresent = true);

					/*
					 * Return directory whcih contains top-level application data - which should be persistent. Store long-term applicaiton
					 * data (which is not user specific) such as databases etc here.
					 */
					TString	GetApplicationData (bool createIfNotPresent = true);

					/*
					 * This returns the directory where an appliation may write temporary files - files which should not
					 * be preserved across reboots (though the OS may not enforce this). This directory may or may not be current-user-specific.
					 */
					TString	GetTemporary ();

					#if			qPlatform_Windows
						// empty string if doesn't exist
						TString	GetWinSxS ();
					#endif

				}

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
