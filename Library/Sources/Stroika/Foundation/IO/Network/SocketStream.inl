/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_inl_
#define _Stroika_Foundation_IO_Network_SocketStream_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                inline  SocketStream::SocketStream (Socket sd)
                    : fSD_ (sd) {
                }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_SocketStream_inl_*/
