/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_ 1

#include    "../../../StroikaPreComp.h"

#include    <functional>

#include    "../../Device.h"



/**
 * TODO:
 *      @todo   Lots of rethinking needed for thread  management, and CTOR arg for search, or jsut Start()/Stop() methods?
 *              But in background, wehnw estart  - we want to use shared_ptr<> rep, and thread in private area todo search
 *              and document callback can be called back on any thread
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {
                namespace   Client {


                    /**
                     */
                    class   Search {
                    public:
                        //...
                        //args - ST, strings, uuid etc.
                        Search (const String& serviceType, const std::function<void(const Device& d)>& callOnFinds);
                    public:
                        NO_DEFAULT_CONSTRUCTOR (Search);
                        NO_COPY_CONSTRUCTOR (Search);
                        NO_ASSIGNMENT_OPERATOR (Search);

                    private:
                        class Rep_;
                        shared_ptr<Rep_>    fRep_;
                    };


                }
            }
        }
    }
}
#endif  /*_Stroika_Frameworks_UPnP_SSDP_Client_Search_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Search.inl"
