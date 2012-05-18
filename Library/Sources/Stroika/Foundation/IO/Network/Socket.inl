/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_inl_
#define _Stroika_Foundation_IO_Network_Socket_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                inline  NativeSocket    Socket::GetNativeSocket () const
                {
                    return fRep_->GetNativeSocket ();
                }

            }

        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Socket_inl_*/
