/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Accessor_h_
#define _Stroika_Foundation_Execution_Resources_Accessor_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {


                class Manager;

                /**
                 * This object provides access to the application-lifetime data/pointers associated with a resource
                 * retrieved from a Resource::Manager (subclass).
                 */
                class   Accessor {
                private:
                    Accessor (const Byte* start, const Byte* end);

                public:
                    nonvirtual  const Byte*     GetDataStart () const;
                    nonvirtual  const Byte*     GetDataEnd () const;
                    nonvirtual  vector<Byte>    GetData () const;

                private:
                    const Byte* fDataStart_;
                    const Byte* fDataEnd_;

                private:
                    friend  class Manager;
                };

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Resources_Accessor_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Accessor.inl"
