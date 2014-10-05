/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Client_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../HTTP/Exception.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {


                    /*
                     ********************************************************************************
                     ********************************* Transfer::Response ***************************
                     ********************************************************************************
                     */
                    inline  Response::Response (const BLOB& data, HTTP::Status status, const Mapping<String, String>& headers, const Optional<SSLResultInfo>& sslInfo)
                        : fData (data)
                        , fHeaders (headers)
                        , fStatus (status)
                        , fServerEndpointSSLInfo (sslInfo)
                    {
                    }
                    inline  BLOB    Response::GetData () const
                    {
                        return fData;
                    }
                    inline  Mapping<String, String>    Response::GetHeaders () const
                    {
                        return fHeaders;
                    }
                    inline  HTTP::Status    Response::GetStatus () const
                    {
                        return fStatus;
                    }
                    inline  Optional<Response::SSLResultInfo>    Response::GetSSLResultInfo () const
                    {
                        return fServerEndpointSSLInfo;
                    }
                    inline  bool    Response::GetSucceeded () const
                    {
                        return HTTP::Exception::IsHTTPStatusOK (fStatus);
                    }


                    /*
                     ********************************************************************************
                     ********************************** Connection::_IRep ***************************
                     ********************************************************************************
                     */
                    inline  Connection::_IRep::_IRep ()
                    {
                    }
                    inline  Connection::_IRep::~_IRep ()
                    {
                    }


                    /*
                     ********************************************************************************
                     ********************************** Connection **********************************
                     ********************************************************************************
                     */
                    inline  Connection::Connection (const shared_ptr<_IRep>& rep)
                        : fRep_ (rep)
                    {
                    }
                    inline  URL     Connection::GetURL () const
                    {
                        return fRep_->GetURL ();
                    }
                    inline  void    Connection::SetURL (const URL& url)
                    {
                        fRep_->SetURL (url);
                    }
                    inline  void    Connection::Close ()
                    {
                        fRep_->Close ();
                    }
                    inline  Response    Connection::SendAndRequest (const Request& r)
                    {
                        return fRep_->Send (r);
                    }
                    inline  Response    Connection::Send (const Request& r)
                    {
                        return fRep_->Send (r);
                    }


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Transfer_Client_inl_*/
