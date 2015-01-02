/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_   1

#include    "../../../StroikaPreComp.h"

#include    "../../../Execution/StringException.h"

#include    "Client.h"



/*
 * TODO:
 *      (o)     Very rough draft. This code is completely untested, and really only about 1/2 written. Even the high level
 *              usage (container) pattern is just a draft.
 *
 *      (o)     Must do basic METHOD support - setting CURLOPT_GET for gets, etc.
 *              Need extended MEHTOD support for DELETE (just specify string)
 *              And for PUT/POST special setopt(CURLOPT_POST/PUT, and do reader function for pUT ajnd direct data pass for Post -
 *              since I THINK thats what curl requires - use common API if I can find a way).
 *
 *      (o)     Handle pass in of headers. Treat special headers like content type proeprly. Same for content-length.
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {


#if     qHasFeature_libcurl
                    class   LibCurlException : public Execution::StringException {
                    public:
                        using   CURLcode    =   int;       // tried directly to reference libcurl CURLcode but tricky cuz its an enum -- LGP 2012-05-08
                    public:
                        LibCurlException (CURLcode ccode);

                    public:
                        // throw Exception () type iff the status indicates a real exception code. This MAY not throw an exception of type LibCurlException,
                        // but MAY map to any other exception type
                        static  void    DoThrowIfError (CURLcode status);

                    public:
                        nonvirtual  CURLcode    GetCode () const;

                    private:
                        CURLcode    fCurlCode_;
                    };
#endif


#if     qHasFeature_libcurl
                    // Just object-slice the smart pointer to get a regular connection object - this is just a factory for
                    // LibCurl connection rep objects
                    class   Connection_LibCurl : public Connection {
                    public:
                        Connection_LibCurl (const Options& options = Options ());

                    private:
                        class   Rep_;
                    };
#endif


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
#include    "Client_libcurl.inl"

#endif  /*_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_*/
