/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_SocketStream_inl_
#define	_Stroika_Foundation_IO_Network_SocketStream_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {


				inline	SocketStream::SocketStream (SocketDescriptor sd)
					: fSD_ (sd)
					{
					}

			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_SocketStream_inl_*/
