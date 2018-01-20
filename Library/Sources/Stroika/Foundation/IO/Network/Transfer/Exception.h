/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Exception_h_
#define _Stroika_Foundation_IO_Network_Transfer_Exception_h_ 1

#include "../../../StroikaPreComp.h"

#include "../HTTP/Exception.h"

#include "Response.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                namespace Transfer {

                    /**
                     */
                    struct Exception : HTTP::Exception {
                    public:
                        Exception (const Response& response);

                    public:
                        nonvirtual Response GetResponse () const;

                    private:
                        Response fResponse_;
                    };
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Exception_h_*/
