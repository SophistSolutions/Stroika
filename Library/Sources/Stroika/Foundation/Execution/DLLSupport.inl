/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_DLLSupport_inl_
#define	_Stroia_Foundation_Execution_DLLSupport_inl_	1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"
#include	"../Characters/StringUtils.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

	//	class	DLLLoader
		inline	DLLLoader::operator DLLHandle ()
			{
				EnsureNotNil (fModule);
				return fModule;
			}
		inline	ProcAddress	DLLLoader::GetProcAddress (const TChar* procName) const
			{
				AssertNotNil (fModule);
				RequireNotNil (procName);
			#if		qPlatform_Windows
				return ::GetProcAddress (fModule, procName);
			#else
				ProcAddress	addr = dlsym (fModule, procName);
				if (addr == NULL) {
					dlerror ();	// clear any old error
					addr = dlsym (fModule, procName);
					// interface seems to be defined only for char*, not wide strings: may need to map procName as well
					const char*	err = dlerror ();
					if (err != NULL) {
						throw DLLException (err);
					}
				}
				return addr;
			#endif
			}
#if		!qPlatform_Windows
			inline	DLLException::DLLException (const char* message) :
				StringException (Characters::ACPStringToWide (message))
			{
			}
#endif
		}
	}
}
#endif	/*_Stroia_Foundation_Execution_DLLSupport_inl_*/
