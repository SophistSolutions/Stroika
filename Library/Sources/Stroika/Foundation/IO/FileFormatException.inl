/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileFormatException_inl_
#define	_Stroika_Foundation_IO_FileFormatException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {

		//	class	FileFormatException
			inline	FileFormatException::FileFormatException (const TString& fileName)
				: fFileName (fileName)
				{
				}

		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileFormatException_inl_*/
