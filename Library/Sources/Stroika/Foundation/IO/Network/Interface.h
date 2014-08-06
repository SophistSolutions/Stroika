/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_h_
#define _Stroika_Foundation_IO_Network_Interface_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Containers/Set.h"
#include    "../../Memory/Optional.h"

#include    "InternetAddress.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Incomplete POSIX IMPL
 *
 *      @todo   No Windoze impl
 *
 *      @todo   Weak API - just a rough first draft
 *
 *		@todo	Fix exception safety (fd leak)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                /**
                 *  Very rough first draft!
                 */
                struct  Interface {
                    String  fInterfaceName;
                    enum class Type {
                        eLoopback,
                        eWiredEthernet,
                        eWIFI,
                        eOther,
                    };
                    Memory::Optional<Type>                      fType;
                    Containers::Set<InternetAddress>            fBindings;  // can be IPv4 or IPv6

                    // @todo document these - 'eRunning' == LINUX RUNNING
                    enum    class   Status {
                        eConnected,
                        eRunning,
                    };
                    Memory::Optional<Containers::Set<Status>>   fStatus;
                };


                /**
                 *  Collect all the interfaces (and their status) from the operating system.
                 */
                Traversal::Iterable<Interface>  GetInterfaces ();


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Interface.inl"

#endif  /*_Stroika_Foundation_IO_Network_Interface_h_*/
