/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_
#define	_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	Platform {
				namespace	Windows {
					//	class	StructuredException
					inline	StructuredException::StructuredException (unsigned int seCode)
						: fSECode (seCode)
						{
						}
					inline	StructuredException::operator unsigned int () const
						{
							return fSECode;
						}
					inline	TString	StructuredException::LookupMessage () const
						{
							return LookupMessage (fSECode);
						}
				}
			}
		}
	}
}

#endif	/*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_*/
