/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileBusyException_inl_
#define	_Stroika_Foundation_IO_FileBusyException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {

		//	class	FileBusyException
			inline	FileBusyException::FileBusyException (const TString& fileName)
				: fFileName (fileName)
				{
				}

		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileBusyException_inl_*/
