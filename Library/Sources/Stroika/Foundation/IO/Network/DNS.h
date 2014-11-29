/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_DNS_h_
#define _Stroika_Foundation_IO_Network_DNS_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Containers/Collection.h"

#include    "InternetAddress.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;
                using   Containers::Collection;


                /**
                 *  DNS (Domain Name Service) Resolver.
                 */
                class DNS {
                public:
                    /**
                     *  Returns the default dns resolver.
                     */
                    static  DNS Default ();

                public:
                    /**
                     */
                    struct  HostEntry {
                        Collection<InternetAddress> fAddressList;
                        Collection<String>          fAliases;
                        String                      fHostname;
                    };

                public:
                    /**
                     */
                    nonvirtual  HostEntry   GetHostEntry (const String& hostNameOrAddress) const;

                public:
                    /**
                     */
                    nonvirtual  Collection<InternetAddress> GetHostAddresses (const String& hostNameOrAddress) const;
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DNS.inl"

#endif  /*_Stroika_Foundation_IO_Network_DNS_h_*/
