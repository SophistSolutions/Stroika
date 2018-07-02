/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_LinkMonitor_inl_
#define _Stroika_Foundation_IO_Network_LinkMonitor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation {
    namespace IO {
        namespace Network {

            /*
             ********************************************************************************
             ********************************** LinkMonitor *********************************
             ********************************************************************************
             */
            inline LinkMonitor::LinkMonitor (const LinkMonitor&& rhs)
                : fRep_ (move (rhs.fRep_))
            {
            }
            inline LinkMonitor& LinkMonitor::operator= (const LinkMonitor&& rhs)
            {
                fRep_ = move (rhs.fRep_);
                return *this;
            }

        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_LinkMonitor_inl_*/
