/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Exception_h_
#define _Stroika_Foundation_IO_Network_HTTP_Exception_h_    1

#include    "../../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../../Configuration/Common.h"
#include    "../../../Execution/StringException.h"

#include    "Status.h"



/*
 * TODO:
 *      (o)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {


                    using   Foundation::Execution::StringException;


                    // See http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1.1
                    class   Exception : public StringException {
                    public:
                        // If no reason is given, a default is generated based on the status code
                        Exception (Status status, const wstring& reason = wstring ());

                    public:
                        // throw Exception () type iff the status indicates a real HTTP exception code (i.e. ignore 2xx values etc)
                        static  void    DoThrowIfError (Status status, const wstring& reason = wstring ());
                        static  void    DoThrowIfError (const wstring& status, const wstring& reason = wstring ());

                        static  bool    IsHTTPStatusOK (Status status);


                    public:
                        /*
                         * Stroika contains a table mapping some HTTP status values to English text. This can be used to access
                         * that mapping. Its thin - but it contains most of the biggies. If forceAlwaysFound is true, it will
                         * always generate some response (sprintf the # with some verbiage) and if false it will return an empty string
                         * for Status values not in our table.
                         */
                        static  wstring GetStandardTextForStatus (Status s, bool forceAlwaysFound = false);

                    public:
                        nonvirtual  Status      GetStatus () const;
                        // If this was constructed with a specific reason - it will be returned here. Otherwise an autogenerated one based
                        // on the status will be returned.
                        //
                        // This is effectively a human readable, and perhaps more detailed - explanation of the exception
                        nonvirtual  wstring     GetReason () const;

                    private:
                        Status  fStatus_;
                        wstring fReason_;
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
#include    "Exception.inl"

#endif  /*_Stroika_Foundation_IO_Network_HTTP_Exception_h_*/
