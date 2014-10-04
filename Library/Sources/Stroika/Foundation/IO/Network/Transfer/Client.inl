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
