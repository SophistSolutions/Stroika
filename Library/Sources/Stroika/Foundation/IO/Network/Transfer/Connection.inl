/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {


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
                    inline  DurationSecondsType     Connection::GetTimeout () const
                    {
                        return fRep_->GetTimeout ();
                    }
                    inline  void    Connection::SetTimeout (DurationSecondsType timeout)
                    {
                        fRep_->SetTimeout (timeout);
                    }


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Transfer_Connection_inl_*/
