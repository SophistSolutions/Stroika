/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_StringException_inl_
#define	_Stroia_Foundation_Execution_StringException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			//	class	StringException
				inline	StringException::StringException (const wstring& reasonForError)
					: fError (reasonForError)
					{
					}
				inline	StringException::operator wstring () const
					{
						return fError;
					}

		}
	}
}
#endif	/*_Stroia_Foundation_Execution_StringException_inl_*/
