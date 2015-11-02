/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                        : fData_ (data)
                        , fHeaders_ (headers)
                        , fStatus_ (status)
                        , fServerEndpointSSLInfo_ (sslInfo)
                    {
                    }
                    inline  Response::Response (BLOB&& data, HTTP::Status status, Mapping<String, String>&& headers)
                        : fData_ (move (data))
                        , fHeaders_ (move (headers))
                        , fStatus_ (status)
                    {
                    }
                    inline  Response::Response (BLOB&& data, HTTP::Status status, Mapping<String, String>&& headers, const Optional<SSLResultInfo>&& sslInfo)
                        : fData_ (move (data))
                        , fHeaders_ (move (headers))
                        , fStatus_ (status)
                        , fServerEndpointSSLInfo_ (move (sslInfo))
                    {
                    }
                    inline  BLOB    Response::GetData () const
                    {
                        return fData_;
                    }
                    inline  Mapping<String, String>    Response::GetHeaders () const
                    {
                        return fHeaders_;
                    }
                    inline  HTTP::Status    Response::GetStatus () const
                    {
                        return fStatus_;
                    }
                    inline  Optional<Response::SSLResultInfo>    Response::GetSSLResultInfo () const
                    {
                        return fServerEndpointSSLInfo_;
                    }
                    inline  bool    Response::GetSucceeded () const
                    {
                        return HTTP::Exception::IsHTTPStatusOK (fStatus_);
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
#if     qDebug
                        if (url.GetHostRelativePath ().StartsWith (L"/")) {
                            DbgTrace (L"Connection::SetURL (URL has host-relative path (%s) starting with / - which is not technically illegal, but often a bug", url.GetHostRelativePath ().c_str ());
                        }
#endif
                        fRep_->SetURL (url);
                    }
                    inline  void    Connection::Close ()
                    {
                        fRep_->Close ();
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
