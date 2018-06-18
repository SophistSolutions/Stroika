/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_inl_
#define _Stroika_Foundation_IO_Network_SocketStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ********************************** SocketStream ********************************
                 ********************************************************************************
                 */
                inline auto SocketStream::_mkPtr (const shared_ptr<Rep_>& s) -> Ptr
                {
                    return Ptr{s};
                }

                /*
                 ********************************************************************************
                 ******************************* SocketStream::Ptr ******************************
                 ********************************************************************************
                 */
                inline SocketStream::Ptr::Ptr (nullptr_t)
                {
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_SocketStream_inl_*/
