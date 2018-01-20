/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SOAP_Fault_h_
#define _Stroika_Foundation_IO_Network_SOAP_Fault_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Memory/Optional.h"
#include "../../../Streams/InputStream.h"

/*
 * TODO:
 *      INSANELY SUPER PRELIMINARY/INCOMPLETE
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                namespace SOAP {

                    using Characters::String;
                    using Memory::Byte;
                    using Memory::Optional;
                    using Streams::InputStream;
                    /*
                     *      SUPER PRIMITIVE PRELIMINARY DRAFT support for SOAP...
                     *
                     *  SOAPFault
                     */
                    struct Fault {
                        String faultcode;
                        String faultstring;
                        //BLOB    detail;     // NYI
                    };
                    Optional<Fault> Deserialize_Fault (const Streams::InputStream<Byte>::Ptr& from);
                    Optional<Fault> Deserialize_Fault (const Memory::BLOB& from);
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
#include "Fault.inl"

#endif /*_Stroika_Foundation_IO_Network_SOAP_Fault_h_*/
